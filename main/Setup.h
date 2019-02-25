/*
 * Setup.h
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#ifndef MAIN_SETUP_H_
#define MAIN_SETUP_H_
#include <string>
#include "esp_system.h"
#include "stdio.h"
#include <esp_log.h>
#include <string>
#include "string.h"
#include "BTSender.h"


typedef struct {
	float _speedcal;
	float _vario_delay;
	float _center_freq;
	float _QNH;
	float _tone_var;
	float _deadband;
	float _deadband_neg;
	float _analog_adj;
	float _contrast_adj;
	float _voltmeter_adj;
	float _range;
	float _v1;
	float _w1;
	float _v2;
	float _w2;
	float _v3;
	float _w3;
	float _ballast;
	float _MC;
	float _s2f_speed;
	uint8_t _audio_mode;
	char  _bt_name[32];
	uint8_t _blue_enable;
	uint8_t _factory_reset;   // 0 = no,  1= yes
	uint8_t _audio_range;     // 0 = fix, 1=variable
	uint8_t _alt_select;      // 0 = TE, 1=Baro
	float   _offset;
	uint32_t _checksum;
} setup_t;

class Setup {
public:
	Setup(bool *operationMode): _operationMode( operationMode ) {
		_ticker = 0;
		_index = 0;
		_menucount=0;
		_btsender = 0;
		memset(_input,0,sizeof(_input));
	}
	// Setup( bool &operationMode, float &speedcal );
	virtual ~Setup() {};
	void factorySetting();
	setup_t *get() { return &_setup; };
	void begin(BTSender *btsender);
	void commit();
	void  help();
	bool checkSum( bool set=false );
	bool  operationMode() { return _operationMode; }
	char  *getBtName() { return _setup._bt_name; }
	float getVarioDelay() { return _setup._vario_delay; }

private:
	bool *_operationMode;
	uint64_t _ticker;
	uint8_t _input[128];
	int  _index;
	int  _menucount;
	setup_t _setup;
	BTSender *_btsender;
    float _test_ms = 1.0;
};


#endif /* MAIN_SETUP_H_ */
