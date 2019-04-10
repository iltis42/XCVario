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
#include "Polars.h"


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
	float _voltmeter_factory_adj;
	float _range;
	float _ballast;
	float _MC;
	float _s2f_speed;
	uint8_t _audio_mode;
	char  _bt_name[32];
	uint8_t _blue_enable;
	uint8_t _factory_reset;   // 0 = no,  1= yes
	uint8_t _audio_range;     // 0 = fix, 1=variable
	uint8_t _alt_select;      // 0 = TE, 1=Baro
	uint8_t _glider_type;     // 0 = Custom Polar
	t_polar _polar;
	float   _offset;
	uint32_t _checksum;
} setup_t;


class Setup {
public:
	Setup() {
		memset( _ID, 0, sizeof( _ID ) );
	}
	// Setup( bool &operationMode, float &speedcal );
	virtual ~Setup() {};
	void factorySetting();
	inline setup_t *get() { return &_setup; };
	void begin();
	void commit();
	void  help();
	bool checkSum( bool set=false );
	inline char  *getBtName() { return _setup._bt_name; }
	inline float getVarioDelay() { return _setup._vario_delay; }
	inline const t_polar getPolar() {  return _setup._polar;  };
	char* getID();

private:
	setup_t _setup;
	static char _ID[14];
};


#endif /* MAIN_SETUP_H_ */
