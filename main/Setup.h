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


typedef enum display_type { UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P } display_t;
typedef enum chopping_mode { NO_CHOP, VARIO_CHOP, S2F_CHOP, BOTH_CHOP } chopping_mode_t;

typedef struct {
	uint32_t _config_version;
	float _speedcal;
	float _vario_delay;
	float _center_freq;
	float _QNH;
	float _tone_var;
	uint8_t _dual_tone;
	float   _high_tone_var;
	float _deadband;
	float _deadband_neg;
	float _analog_adj;
	float _contrast_adj;
	float _voltmeter_adj;
	float _range;
	float _ballast;
	float _MC;
	float _s2f_speed;
	uint8_t _audio_mode;
	uint8_t _chopping_mode;
	char  _bt_name[32];
	uint8_t _blue_enable;
	uint8_t _factory_reset;   // 0 = no,  1= yes
	uint8_t _audio_range;     // 0 = fix, 1=variable
	uint8_t _alt_select;      // 0 = TE, 1=Baro
	uint8_t _glider_type;     // 0 = Custom Polar
	t_polar _polar;
	float   _offset;
	uint8_t _ps_display;
	float   _bat_low_volt;
	float   _bat_red_volt;
	float   _bat_yellow_volt;
	float   _bat_full_volt;
	uint8_t _display_type;
	float   _core_climb_min;
	float   _core_climb_history;
	float   _elevation;
	uint8_t _display_orientation;  // 0 = normal, 1 = upside down
	uint8_t _flap_enable;
	float    _flap_minus_2;  // -1 -2  165
	float    _flap_minus_1;  // 0 -1  105
	float    _flap_0;        // 1-0   88
	float    _flap_plus_1;   // 2-1   78
	float    _default_volume;
	uint8_t  _alt_unit;
	uint8_t  _ias_unit;
	uint8_t  _vario_unit;
	uint8_t  _rot_default;
	uint8_t  _serial2_speed;
	uint8_t  _serial2_rxloop;
	uint8_t  _serial2_tx;
	uint8_t  _serial2_tx_inverted;
	uint8_t  _serial2_rx_inverted;
	uint8_t  _software_update;
	float    _factory_volt_adjust;
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
	inline char  *getBtName() { return getID(); }
	inline float getVarioDelay() { return _setup._vario_delay; }
	inline const t_polar getPolar() {  return _setup._polar;  };
	char* getID();

private:
	setup_t _setup;
	static char _ID[14];
};


#endif /* MAIN_SETUP_H_ */
