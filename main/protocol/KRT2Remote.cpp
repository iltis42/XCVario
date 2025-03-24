/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "KRT2Remote.h"
#include "nmea_util.h"
#include "comm/DeviceMgr.h"
#include "comm/Messages.h"
#include "logdef.h"
#include <cstring>
#include <string>

const uint8_t KRT2_STX_START = 0x02;

// Single byte commands:
const uint8_t KRT2_SET_SET25     = 0x36;
const uint8_t KRT2_SET_SET83     = 0x38;
const uint8_t KRT2_EXCHG_ACTSTBY = 0x43;
const uint8_t KRT2_STBY_NEXTM    = 0x57;
const uint8_t KRT2_ACTIV_DUAL    = 0x4F;
const uint8_t KRT2_DEACTIV_DUAL  = 0x6F;

// single byte error replies
const uint8_t  KRT2_ADC_error     = 0x61;
const uint8_t  KRT2_ANTENNA_error = 0x62;
const uint8_t  KRT2_FPPA_error    = 0x63;
const uint8_t  KRT2_SYNTH_error   = 0x64;
const uint8_t  KRT2_PLL_error     = 0x65;
const uint8_t  KRT2_KEYBL_error   = 0x66;
const uint8_t  KRT2_I2C_error     = 0x67;
const uint8_t  KRT2_ANTSW_error   = 0x68;
const uint8_t  KRT2_CLEAR_error   = 0x46;

// one byte data command
const uint8_t KRT2_SET_SETSTL = 0x31;
const uint8_t KRT2_SET_PTT  = 0x32;
const uint8_t KRT2_SET_ICV  = 0x33;
const uint8_t KRT2_SET_EXV  = 0x34;
const uint8_t KRT2_SET_BMICG = 0x49;
const uint8_t KRT2_SET_CMICG = 0x4A;

// 3 byte data vol, squelch, vox 1-10[20]
const uint8_t KRT2_VOL_SQL_VOX = 0x41;

// 2 byte freq and 8 byte data
const uint8_t KRT2_SET_STBYFREQ = 0x52;
const uint8_t KRT2_SET_ACTFREQ = 0x55;

// state machine definition

/*
typedef enum  {
	GET_KRT2_FRAME,
	GET_KRT2_COMMAND,
	GET_KRT2_MHZ,
	GET_KRT2_KHZ,
	GET_KRT2_NAME,
	GET_KRT2_VOX,
	GET_KRT2_VOL,
	GET_KRT2_SQL,
	GET_KRT2_PTT,
	GET_KRT2_ICV,
	GET_KRT2_SINGLE_DATABYTE,
	GET_KRT2_CHECKSUM
} gen_state_t;
*/

KRT2Remote::KRT2Remote(DeviceId did, int mp, ProtocolState &sm, DataLink &dl)
: ProtocolItf(did, mp, sm, dl)
{
	ESP_LOGI(FNAME, "KRT2Remote Protocol @DeviceId: %d", did );
}

KRT2Remote::~KRT2Remote()
{

}

datalink_action_t KRT2Remote::nextByte(const char c)
{
	int pos = _sm._frame.size() - 1; // c already in the buffer
	datalink_action_t ret=NOACTION;
	ESP_LOGD(FNAME, "state %d, pos %d next char %c", _sm._state, pos, c);
	switch(_sm._state) {
		case GET_KRT2_FRAME:
			if( c == KRT2_STX_START ){
				ESP_LOGI(FNAME, "STX Start at %d", pos );
				_sm._state = GET_KRT2_COMMAND;
			}
		break;
		case GET_KRT2_COMMAND:
			switch(c){
				// Single byte commands
				case KRT2_EXCHG_ACTSTBY: // commands, single byte
				case KRT2_SET_SET83:
				case KRT2_SET_SET25:
				case KRT2_STBY_NEXTM:
				case KRT2_ACTIV_DUAL:
				case KRT2_DEACTIV_DUAL:
				case KRT2_ADC_error:     // error replies, single byte
				case KRT2_ANTENNA_error:
				case KRT2_FPPA_error:
				case KRT2_SYNTH_error:
				case KRT2_PLL_error:
				case KRT2_KEYBL_error:
				case KRT2_I2C_error:
				case KRT2_ANTSW_error:
				case KRT2_CLEAR_error:
					_sm._state = GET_KRT2_FRAME;
					ret = datalink_action_t(COMPLETE_BIT | FORWARD_BIT);
				break;

				// Single data byte commands
				case KRT2_SET_PTT:
				case KRT2_SET_ICV:
				case KRT2_SET_EXV:
				case KRT2_SET_SETSTL:
				case KRT2_SET_BMICG:
				case KRT2_SET_CMICG:
					_sm._state = GET_KRT2_SINGLE_DATABYTE;
				break;

				// Three byte commands
				case KRT2_VOL_SQL_VOX:
					_sm._state = GET_KRT2_VOL;
				break;
				// 10 byte commands, 2 bytes frequency, 8 bytes station name
				case KRT2_SET_STBYFREQ:
				case KRT2_SET_ACTFREQ:
					data_len = 8;
					_sm._state = GET_KRT2_NAME;
				break;
				default:
				break;
			}
			break;  // end STX START

			// State handlers
			case GET_KRT2_SINGLE_DATABYTE:
				_sm._state = GET_KRT2_FRAME;
				ret = datalink_action_t(COMPLETE_BIT | FORWARD_BIT);
				break;
			case GET_KRT2_VOL:
				_sm._state = GET_KRT2_SQL;
				break;
			case GET_KRT2_SQL:
				mhz = c;  // its sql but we recycle mhz for CS here
				_sm._state = GET_KRT2_VOX;
				break;
			case GET_KRT2_VOX:
				khz = c;  // its vox but we recycle khz for CS here
				_sm._state = GET_KRT2_VOL_CS;
				break;
			case GET_KRT2_VOL_CS:
				if( c == (mhz+khz) ){
					_sm._state = GET_KRT2_FRAME;
					ret = datalink_action_t(COMPLETE_BIT | FORWARD_BIT);
				}
				break;

			case GET_KRT2_MHZ:
				mhz = c;
				_sm._state = GET_KRT2_KHZ;
				break;

			case GET_KRT2_KHZ:
				khz = c;
				cs = mhz^khz;  // CS is XOR of mhz and khz according to spec
				_sm._state = GET_KRT2_NAME;
				break;

			case GET_KRT2_NAME:
				data_len--;
				if( !data_len ){
					_sm._state = GET_KRT2_CHECKSUM;
				}
				break;
			case GET_KRT2_CHECKSUM:
				if( cs == c )
					ret = datalink_action_t(COMPLETE_BIT | FORWARD_BIT);
				else
					ret = NOACTION;
				break;

			default:
				break;
		}
	return ret;
}



