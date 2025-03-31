/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "ATR833Remote.h"
#include "nmea_util.h"
// #include "comm/Messages.h"
#include "logdef.h"
#include <cstring>
#include <string>


// Binary frame start
constexpr uint8_t ATR833_STX_START = 0x02;
constexpr uint8_t ATR833_SYNC_CHAR = 0x72;

// Single byte commands
constexpr uint8_t ATR833_Keep_Alive =  0x10;
constexpr uint8_t ATR833_Swap_ActSby = 0x11;
constexpr uint8_t ATR833_Idle_state = 0x60;
constexpr uint8_t ATR833_Reception = 0x61;
constexpr uint8_t ATR833_Transmission = 0x62;
constexpr uint8_t ATR833_Dual_watch = 0x64;
constexpr uint8_t ATR833_Reception_on_stby_frequency = 0x65;
constexpr uint8_t ATR833_Low_voltage = 0x66;
constexpr uint8_t ATR833_Voltage_OK = 0x67;
constexpr uint8_t ATR833_Transmission_timeout = 0x68;
constexpr uint8_t ATR833_Reception_on_active_frequency = 0x69;
constexpr uint8_t ATR833_Power_off = 0x70;
constexpr uint8_t ATR833_Power_off_stopped = 0x72;
constexpr uint8_t ATR833_Request_All_Data = 0x82;

// Two byte commands
constexpr uint8_t ATR833_Set_Standby_Freq = 0x12;
constexpr uint8_t ATR833_Set_Active_Frequency = 0x13;

// One byte commands
constexpr uint8_t ATR833_Set_Intercom_Vol = 0x14;
constexpr uint8_t ATR833_Set_Radio_Vol = 0x16;
constexpr uint8_t ATR833_Set_Squelch_Level = 0x17;
constexpr uint8_t ATR833_Set_Vox_Level = 0x18;
constexpr uint8_t ATR833_Set_Dual_Watch = 0x19;
constexpr uint8_t ATR833_Set_External_NF_Vol = 0x1A;
constexpr uint8_t ATR833_Set_Sidetone_L_Vol = 0x1B;
constexpr uint8_t ATR833_Set_Sidetone_R_Vol = 0x1C;
constexpr uint8_t ATR833_Power_off_count_down = 0x71;

// sent from ATR833
constexpr uint8_t ATR833_ACK = 0x06;   // no CS   2 byte with ID got
constexpr uint8_t ATR833_NACK = 0x15;  // no CS  3 byte with ID got and error code


// state machine definition


typedef enum  {
	GET_ATR833_FRAME = 0,  // must be 0 == START_TOKEN
	GET_ATR833_COMMAND,
	GET_ATR833_MHZ,
	GET_ATR833_KHZ,
	GET_ATR833_SINGLE_DATABYTE,
	GET_ATR833_CHECKSUM,
	GET_ATR833_ACK_ID,
	GET_ATR833_NACK_ID,
	GET_ATR833_ALL_DATA,
	GET_ATR833_NACK_ERROR
} krt2_state_t;


ATR833Remote::ATR833Remote(int mp, ProtocolState &sm, DataLink &dl)
    : ProtocolItf(RADIO_ATR833_DEV, mp, sm, dl)
{
    ESP_LOGI(FNAME, "ATR833Remote Protocol @DeviceId %d", RADIO_ATR833_DEV);
    _sm._state = GET_ATR833_FRAME;
}

dl_control_t ATR833Remote::nextBytes(const char* c, int len)
{
    int pos = _sm._frame.size() - 1; // c already in the buffer
    dl_action_t ret = NOACTION;
    // ESP_LOGI(FNAME, "state %d, pos %d next char %c", _sm._state, pos, *c);
    switch (_sm._state)
    {
    case GET_ATR833_FRAME:   // the frame start 0x02
        if (*c == ATR833_STX_START)
        {
            ESP_LOGI(FNAME, "STX Start at %d", pos);
            _sm._state = ATR833_SYNC_CHAR;
            cs=0;
        }
        break;

    case ATR833_SYNC_CHAR:  // second 'sync' char according to spec 'r' or 0x72, its part of CS
    	cs ^= *c;
    	 _sm._state = GET_ATR833_COMMAND;
    	break;

    case GET_ATR833_COMMAND: // the third byte, the message ID, depending on that processing follows
        switch (*c)
        {
        // Single byte commands following CS
        case ATR833_Keep_Alive:
        case ATR833_Swap_ActSby:
        case ATR833_Idle_state:
        case ATR833_Reception:
        case ATR833_Transmission:
        case ATR833_Dual_watch:
        case ATR833_Reception_on_stby_frequency:
        case ATR833_Low_voltage:
        case ATR833_Voltage_OK:
        case ATR833_Transmission_timeout:
        case ATR833_Reception_on_active_frequency:
        case ATR833_Power_off:
        case ATR833_Power_off_stopped:
        	cs ^= *c;
            _sm._state = GET_ATR833_CHECKSUM;
            break;

        // Single data byte commands, then CS
        case ATR833_Set_Intercom_Vol:
        case ATR833_Set_Radio_Vol:
        case ATR833_Set_Squelch_Level:
        case ATR833_Set_Vox_Level:
        case ATR833_Set_Dual_Watch:
        case ATR833_Set_External_NF_Vol:
        case ATR833_Set_Sidetone_L_Vol:
        case ATR833_Set_Sidetone_R_Vol:
        case ATR833_Power_off_count_down:
        	cs ^= *c;
            _sm._state = GET_ATR833_SINGLE_DATABYTE;
            break;

        case ATR833_ACK:   // sent by ATR833 with no CS and comes with msg ID got
        	_sm._state = GET_ATR833_ACK_ID;
        	break;
        case ATR833_NACK:  // dito but comes with msg ID and also errorcode
        	_sm._state = GET_ATR833_NACK_ID;
        	break;

        // 2 bytes frequency
        case ATR833_Set_Standby_Freq:
        case ATR833_Set_Active_Frequency:
            _sm._state = GET_ATR833_MHZ;
            break;

        case ATR833_Request_All_Data:  // this is a request that is answered with 12 bytes of data, according to spec this should have CS
            cs ^= *c;
            _sm._frame_len = 12;  // now comes 12 bytes
            _sm._state = GET_ATR833_ALL_DATA;
            break;

        default:
            break;
        }
        break; // end STX START

    case GET_ATR833_ACK_ID:  // ACK is just this, comes with one byte ID of message got
    	_sm._state = GET_ATR833_FRAME;
    	ret = DO_ROUTING;
    	break;

    case GET_ATR833_NACK_ID:  // NACK comes with error
      	_sm._state = GET_ATR833_NACK_ERROR;
      	break;

    case GET_ATR833_NACK_ERROR: // error from ATR833, we log that as error
        _sm._state = GET_ATR833_FRAME;
    	ESP_LOGE(FNAME, "ATR833 sends NACK, error: %d", *c );
        ret = DO_ROUTING;
        break;

    // State handlers
    case GET_ATR833_SINGLE_DATABYTE:
    	ESP_LOGV(FNAME, "ATR833 single databyte: %d", *c );
        _sm._state = GET_ATR833_CHECKSUM;
        cs ^= *c;
        break;

    case GET_ATR833_MHZ:
        cs ^= *c;
        // ESP_LOGI(FNAME, "ATR833 Mhz:%d", mhz );
        _sm._state = GET_ATR833_KHZ;
        break;

    case GET_ATR833_KHZ:
    	 cs ^= *c;
        // ESP_LOGI(FNAME, "ATR833 Khz:%d", khz );
        _sm._state = GET_ATR833_CHECKSUM;
        break;

    case GET_ATR833_ALL_DATA:
       	// ESP_LOGI(FNAME, "KRT2 NAME, len: %d",  _sm._frame_len );
       	_sm._frame_len--;
        if (_sm._frame_len == 0)
        {
            _sm._state = GET_ATR833_CHECKSUM;
        }
        break;

    case GET_ATR833_CHECKSUM:
    	ESP_LOGI(FNAME, "ATR833 Checksum RX:%d calc:%d  OKAY:%d", *c, cs, cs == *c );
        if (cs == *c)
        {
            ret = DO_ROUTING;
        }
        _sm._state = GET_ATR833_FRAME;
        break;

    default:
        break;
    }
    // ESP_LOGI(FNAME,"ATR833 ret:%d", ret );
    return dl_control_t(ret);
}
