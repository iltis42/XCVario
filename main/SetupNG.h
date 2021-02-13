/*
 * Setup.h
 *
 *  Created on: August 23, 2020
 *      Author: iltis
 */

#ifndef MAIN_SETUP_NG_H_
#define MAIN_SETUP_NG_H_

extern "C" {
#include "esp_partition.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"
}

#include <string>
#include <stdio.h>
#include "esp_system.h"
#include "stdio.h"
#include <esp_log.h>
#include <string>
#include "string.h"
#include "BTSender.h"
#include "Polars.h"
#include <iostream>
#include <vector>
#include "logdef.h"
#include "MPU.h"
#include <WString.h>


/*
 *
 * NEW Simplified and distributed non volatile config data API with template classes:
 * SetupNG<int>  airspeed_mode( "AIRSPEED_MODE", MODE_IAS );
 *
 *  int as = airspeed_mode.get();
 *  airspeed_mode.set( MODE_TAS );
 *
 */

typedef enum display_type { UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P } display_t;
typedef enum chopping_mode { NO_CHOP, VARIO_CHOP, S2F_CHOP, BOTH_CHOP } chopping_mode_t;
typedef enum rs232linemode { RS232_NORMAL, RS232_INVERTED } rs232lm_t;
typedef enum nmea_protocol  { OPENVARIO, BORGELT, CAMBRIDGE, XCVARIO, GENERIC } nmea_proto_t;
typedef enum airspeed_mode  { MODE_IAS, MODE_TAS } airspeed_mode_t;
typedef enum e_display_style  { DISPLAY_AIRLINER, DISPLAY_RETRO } display_style_t;
typedef enum e_s2f_type  { S2F_HW_SWITCH, S2F_HW_PUSH_BUTTON } e_s2f_type;
typedef enum e_serial_route_type { RT_DISABLE = 0, RT_XCVARIO = 1, RT_WIRELESS = 2, RT_S1 = 4, RT_XCVARIO_S1 = 5  } e_serial_routing_t;
typedef enum e_wireless_type { WL_DISABLE, WL_BLUETOOTH, WL_WLAN, WL_WLAN_CLIENT } e_wireless_t;
typedef enum e_audiomode_type { AM_VARIO, AM_S2F, AM_SWITCH, AM_AUTOSPEED } e_audiomode_t;
typedef enum e_audio_tone_mode { ATM_SINGLE_TONE, ATM_DUAL_TONE } e_audio_tone_mode_t;
typedef enum e_audio_chopping_style { AUDIO_CHOP_SOFT, AUDIO_CHOP_HARD } e_audio_chopping_style_t;
typedef enum e_audio_range { AUDIO_RANGE_5_MS, AUDIO_RANGE_10_MS, AUDIO_RANGE_VARIABLE } e_audio_range_t;
typedef enum e_flap_sensor { FLAP_SENSOR_DISABLE, FLAP_SENSOR_GPIO_2, FLAP_SENSOR_GPIO_34 } e_flap_sensor_t;
typedef enum e_audio_brutto_netto { AUDIO_BRUTTO, AUDIO_NETTO } e_audio_brutto_netto_t;
typedef enum e_vario_mode { VARIO_BRUTTO, VARIO_NETTO, CRUISE_NETTO } e_vario_mode_t;


const int baud[] = { 0, 4800, 9600, 19200, 38400, 57600, 115200 };

class SetupCommon {
public:
	SetupCommon() {  memset( _ID, 0, sizeof( _ID )); };
	~SetupCommon() {};
	virtual bool init() = 0;
	virtual bool erase() = 0;
	virtual bool mustReset() = 0;
	virtual const char* key() = 0;
	static std::vector<SetupCommon *> entries;
	static bool initSetup( bool &present );  // returns false if FLASH was completely blank
	static char *getID();
private:
	static char _ID[14];
};

template<typename T>

class SetupNG: public SetupCommon
{
public:
	SetupNG() {};

	SetupNG( const char * akey, T adefault, bool reset=true ) {
		// ESP_LOGI(FNAME,"SetupNG(%s)", akey );
		if( strlen( akey ) > 15 )
			ESP_LOGE(FNAME,"SetupNG(%s) key > 15 char !", akey );
		entries.push_back( this );  // an into vector
		_nvs_handle = 0;
		_key = akey;
		_default = adefault;
		_reset = reset;

	};

	inline T* getPtr() {
		return &_value;
	};
	inline T& getRef() {
		return _value;
	};
	inline T get() {
		return _value;
	};
	const char * key() {
		return _key;
	}

	bool set( T aval, bool save=true ) {
		String val( aval );
		if( save )
			ESP_LOGI( FNAME,"set val: %s", val.c_str() );
		_value = T(aval);
		if( !open() ) {
			ESP_LOGE(FNAME,"NVS Error open nvs handle !");
			return( false );
		}
		bool ret=true;
		if( save )
			ret = commit();
		return ret;
	};

	bool commit() {
		ESP_LOGI(FNAME,"NVS commit(): ");
		if( !open() ) {
			ESP_LOGE(FNAME,"NVS error");
			return false;
		}
		ESP_LOGI(FNAME,"NVS commit(key:%s , addr:%08x, len:%d, nvs_handle: %04x)", _key, (unsigned int)(&_value), sizeof( _value ), _nvs_handle);
		esp_err_t _err = nvs_set_blob(_nvs_handle, _key, (void *)(&_value), sizeof( _value ));
		if(_err != ESP_OK) {
			ESP_LOGE(FNAME,"NVS set blob error %d", _err );
			close();
			return( false );
		}
		_err = nvs_commit(_nvs_handle);
		if(_err != ESP_OK)  {
			ESP_LOGE(FNAME,"NVS nvs_commit error");
			close();
			return false;
		}
		ESP_LOGI(FNAME,"success");
		close();
		return true;
	};

	bool exists() {
		if( !open() ) {
			ESP_LOGE(FNAME,"Error open nvs handle !");
			return false;
		}
		size_t required_size;
		esp_err_t _err = nvs_get_blob(_nvs_handle, _key, NULL, &required_size);
		if ( _err != ESP_OK )
			return false;
		return true;
	};

	virtual bool init() {
		if( !open() ) {
			ESP_LOGE(FNAME,"Error open nvs handle !");
			return false;
		}
		size_t required_size;
		esp_err_t _err = nvs_get_blob(_nvs_handle, _key, NULL, &required_size);
		if ( _err != ESP_OK ){
			ESP_LOGE(FNAME, "NVS nvs_get_blob error: returned error ret=%d", _err );
			set( _default );  // try to init
		}
		else {
			if( required_size > sizeof( T ) ) {
				ESP_LOGE(FNAME,"NVS error: size too big: %d > %d", required_size , sizeof( T ) );
				erase();
				set( _default );  // try to init
				close();
				return false;
			}
			else {
				// ESP_LOGI(FNAME,"NVS size okay: %d", required_size );
				_err = nvs_get_blob(_nvs_handle, _key, &_value, &required_size);
				if ( _err != ESP_OK ){
					ESP_LOGE(FNAME, "NVS nvs_get_blob returned error ret=%d", _err );
					erase();
					set( _default );  // try to init
				}
				else {
					String val( _value );
					ESP_LOGI(FNAME,"NVS key %s exists len: %d value: %s", _key, required_size, val.c_str() );
					// std::cout << _value << "\n";
				}
			}
		}
		close();
		return true;
	};


	virtual bool erase() {
		open();
		esp_err_t _err = nvs_erase_key(_nvs_handle, _key);
		if(_err != ESP_OK)
			return false;
		else {
			ESP_LOGI(FNAME,"NVS erased %s by handle %d", _key, _nvs_handle );
			if( set( _default ) )
				return true;
			else
				return false;
		}
	};

	virtual bool mustReset() {
		return _reset;
	};

	bool erase_all() {
		open();
		esp_err_t _err = nvs_erase_all(_nvs_handle);
		if(_err != ESP_OK)
			return false;
		else
			ESP_LOGI(FNAME,"NVS erased all by handle %d", _nvs_handle );
		if( commit() )
			return true;
		else
			return false;
	};

private:
	T _value;
	T _default;
	const char * _key;
	nvs_handle_t  _nvs_handle;
	bool _reset;

	bool open() {
		if( _nvs_handle == 0) {
			esp_err_t _err = nvs_open("SetupNG", NVS_READWRITE, &_nvs_handle);
			if(_err != ESP_OK){
				ESP_LOGE(FNAME,"ESP32NVS open storage error %d", _err );
				_nvs_handle = 0;
				return( false );
			}
			else {
				// ESP_LOGI(FNAME,"ESP32NVS handle: %04X  OK", _nvs_handle );
				return( true );
			}
		}
		return true;
	};
	void close() {
		if( _nvs_handle != 0) {
			nvs_close(_nvs_handle);
			_nvs_handle = 0;
		}
	};
};

extern SetupNG<float>  		QNH;
extern SetupNG<float> 		polar_wingload;
extern SetupNG<float> 		polar_speed1;
extern SetupNG<float> 		polar_sink1;
extern SetupNG<float> 		polar_speed2;
extern SetupNG<float> 		polar_sink2;
extern SetupNG<float> 		polar_speed3;
extern SetupNG<float> 		polar_sink3;
extern SetupNG<float> 		polar_max_ballast;
extern SetupNG<float> 		polar_wingarea;

extern SetupNG<float>  		speedcal;
extern SetupNG<float>  		vario_delay;
extern SetupNG<float>  		vario_av_delay;
extern SetupNG<float>  		center_freq;
extern SetupNG<float>  		tone_var;
extern SetupNG<int>  		dual_tone;
extern SetupNG<float>  		high_tone_var;
extern SetupNG<float>  		deadband;
extern SetupNG<float>  		deadband_neg;
extern SetupNG<float>  		range;
extern SetupNG<float>  		ballast;
extern SetupNG<float>  		MC;
extern SetupNG<float>  		s2f_speed;

extern SetupNG<int>  		audio_mode;
extern SetupNG<int>  		chopping_mode;
extern SetupNG<int>  		chopping_style;
extern SetupNG<int>  		amplifier_shutdown;

extern SetupNG<int>  		blue_enable;
extern SetupNG<int>  		factory_reset;
extern SetupNG<int>  		audio_range;
extern SetupNG<int>  		alt_select;
extern SetupNG<int>  		fl_auto_transition;
extern SetupNG<float>  		transition_alt;
extern SetupNG<int>  		glider_type;
extern SetupNG<int>  		ps_display;

extern SetupNG<float>  		as_offset;
extern SetupNG<float>  		bat_low_volt;
extern SetupNG<float>  		bat_red_volt;
extern SetupNG<float>  		bat_yellow_volt;
extern SetupNG<float>  		bat_full_volt;
extern SetupNG<float>  		core_climb_period;
extern SetupNG<float>  		core_climb_min;
extern SetupNG<float>  		core_climb_history;
extern SetupNG<float>  		elevation;
extern SetupNG<float>  		default_volume;
extern SetupNG<float>  		s2f_deadband;
extern SetupNG<float>  		s2f_deadband_neg;
extern SetupNG<float>  		s2f_delay;
extern SetupNG<float>  		factory_volt_adjust;
extern SetupNG<float>  		bugs;

extern SetupNG<int>  		display_type;
extern SetupNG<int>  		display_orientation;
extern SetupNG<int>  		flap_enable;
extern SetupNG<float>  		flap_minus_3;
extern SetupNG<float>  		flap_minus_2;
extern SetupNG<float>  		flap_minus_1;
extern SetupNG<float>  		flap_0;
extern SetupNG<float>  		flap_plus_1;
extern SetupNG<float>  		flap_plus_2;
extern SetupNG<int>  		alt_unit;
extern SetupNG<int>  		ias_unit;
extern SetupNG<int>  		vario_unit;
extern SetupNG<int>  		rot_default;

extern SetupNG<int>  		serial1_speed;
extern SetupNG<int>  		serial1_rxloop;
extern SetupNG<int>  		serial1_tx;
extern SetupNG<int>  		serial1_tx_inverted;
extern SetupNG<int>  		serial1_rx_inverted;
extern SetupNG<int>  		serial2_speed;
extern SetupNG<int>  		serial2_tx;
extern SetupNG<int>			serial2_pins_twisted;
extern SetupNG<int>  		serial2_tx_inverted;
extern SetupNG<int>  		serial2_rx_inverted;
extern SetupNG<int>  		software_update;
extern SetupNG<int>  		battery_display;
extern SetupNG<int>  		airspeed_mode;
extern SetupNG<int>  		nmea_protocol;
extern SetupNG<int>		    log_level;
extern SetupNG<float>		audio_factor;
extern SetupNG<float>		te_comp_adjust;
extern SetupNG<int>		    te_comp_enable;
extern SetupNG<int>		    rotary_dir;
extern SetupNG<int>		    rotary_dir_21;
extern SetupNG<int>		    rotary_inc;
extern SetupNG<int>		    student_mode;
extern SetupNG<float>		password;
extern SetupNG<int>		    autozero;
extern SetupNG<int>		    attitude_indicator;
extern SetupNG<int>		    ahrs_autozero;
extern SetupNG<int>		    display_style;
extern SetupNG<int>		    s2f_switch_type;
extern SetupNG<mpud::raw_axes_t> gyro_bias;
extern SetupNG<mpud::raw_axes_t> accl_bias;
extern SetupNG<int>		    hardwareRevision;
extern SetupNG<int>		    ahrs_licence_dig1;
extern SetupNG<int>		    ahrs_licence_dig2;
extern SetupNG<int>		    ahrs_licence_dig3;
extern SetupNG<int>		    ahrs_licence_dig4;
extern SetupNG<int>		    dummy;
extern SetupNG<int>		    wk_sens_pos_plus_3;
extern SetupNG<int>		    wk_sens_pos_plus_2;
extern SetupNG<int>		    wk_sens_pos_plus_1;
extern SetupNG<int>		    wk_sens_pos_0;
extern SetupNG<int>		    wk_sens_pos_minus_1;
extern SetupNG<int>		    wk_sens_pos_minus_2;
extern SetupNG<int>		    wk_sens_pos_minus_3;
extern SetupNG<int>       	stall_warning;
extern SetupNG<float>		stall_speed;
extern SetupNG<int>       	flarm_warning;
extern SetupNG<float>     	flarm_volume;
extern SetupNG<int>       	flarm_sim;
extern SetupNG<int>       	flap_sensor;
extern SetupNG<int>       	audio_netto_mode;
extern SetupNG<float>     	flap_pos_max;
extern SetupNG<float>     	flap_neg_max;
extern SetupNG<int>       	compass_enable;
extern SetupNG<float>       compass_dev_45;
extern SetupNG<float>       compass_dev_90;
extern SetupNG<float>       compass_dev_135;
extern SetupNG<float>       compass_dev_180;
extern SetupNG<float>       compass_dev_225;
extern SetupNG<float>       compass_dev_270;
extern SetupNG<float>       compass_dev_335;
extern SetupNG<float>       compass_dev_360;
extern SetupNG<float>       compass_x_bias;
extern SetupNG<float>       compass_y_bias;
extern SetupNG<float>       compass_z_bias;
extern SetupNG<float>       compass_x_scale;
extern SetupNG<float>       compass_y_scale;
extern SetupNG<float>       compass_z_scale;
extern SetupNG<float>       compass_calibration_time;
extern SetupNG<int>         compass_calibrated;
extern SetupNG<float>       compass_declination;
extern SetupNG<int>         compass_declination_valid;
extern SetupNG<int>         compass_nmea_hdm;
extern SetupNG<int>         compass_nmea_hdt;
extern SetupNG<int>         audio_netto_mode;
extern SetupNG<int> 		s2f_with_gload;
extern SetupNG<int> 		s2f_blockspeed;
extern SetupNG<int> 		wk_label_plus_3;
extern SetupNG<int> 		wk_label_plus_2;
extern SetupNG<int> 		wk_label_plus_1;
extern SetupNG<int> 		wk_label_null_0;
extern SetupNG<int> 		wk_label_minus_1;
extern SetupNG<int> 		wk_label_minus_2;
extern SetupNG<int> 		wk_label_minus_3;
extern SetupNG<float>       flap_takeoff;
extern SetupNG<int> 		audio_disable;
extern SetupNG<int>			vario_mode;

#endif /* MAIN_SETUP_NG_H_ */
