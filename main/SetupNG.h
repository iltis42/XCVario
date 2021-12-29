/*
 * SetupNG.h
 *
 *  Created on: August 23, 2020
 *      Author: iltis
 */

#pragma once

#include "BTSender.h"
#include "Polars.h"
#include "MPU.hpp" // change from .h to .hpp for Windows toolchain compatibility

#include <esp_partition.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <nvs.h>

#include <freertos/FreeRTOS.h>
#include <esp_timer.h>
#include <freertos/queue.h>
#include <esp_system.h>
#include <esp_log.h>

#include <string>
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include "Compass.h"
#include "SetupCommon.h"


/*
 *
 * NEW Simplified and distributed non volatile config data API with template classes:
 * SetupNG<int>  airspeed_mode( "AIRSPEED_MODE", MODE_IAS );
 *
 *  int as = airspeed_mode.get();
 *  airspeed_mode.set( MODE_TAS );
 *
 */


typedef enum display_type { UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P } xcv_display_t;
typedef enum chopping_mode { NO_CHOP, VARIO_CHOP, S2F_CHOP, BOTH_CHOP } chopping_mode_t;
typedef enum rs232linemode { RS232_NORMAL, RS232_INVERTED } rs232lm_t;
typedef enum nmea_protocol  { OPENVARIO, BORGELT, CAMBRIDGE, XCVARIO, XCVARIO_DEVEL, GENERIC } nmea_proto_t;
typedef enum airspeed_mode  { MODE_IAS, MODE_TAS } airspeed_mode_t;
typedef enum altitude_display_mode  { MODE_QNH, MODE_QFE } altitude_display_mode_t;
typedef enum e_display_style  { DISPLAY_AIRLINER, DISPLAY_RETRO, DISPLAY_UL } display_style_t;
typedef enum e_display_variant { DISPLAY_WHITE_ON_BLACK, DISPLAY_BLACK_ON_WHITE } display_variant_t;
typedef enum e_s2f_type  { S2F_HW_SWITCH, S2F_HW_PUSH_BUTTON, S2F_HW_SWITCH_INVERTED } e_s2f_type;
typedef enum e_serial_route_type { RT_DISABLE = 0, RT_XCVARIO = 1, RT_WIRELESS = 2, RT_S1 = 4, RT_XCVARIO_S1 = 5  } e_serial_routing_t;
typedef enum e_wireless_type { WL_DISABLE, WL_BLUETOOTH, WL_WLAN_MASTER, WL_WLAN_CLIENT, WL_WLAN_STANDALONE } e_wireless_t;
typedef enum e_audiomode_type { AM_VARIO, AM_S2F, AM_SWITCH, AM_AUTOSPEED, AM_FROM_MASTER } e_audiomode_t;
typedef enum e_audio_tone_mode { ATM_SINGLE_TONE, ATM_DUAL_TONE } e_audio_tone_mode_t;
typedef enum e_audio_chopping_style { AUDIO_CHOP_SOFT, AUDIO_CHOP_HARD } e_audio_chopping_style_t;
typedef enum e_audio_range { AUDIO_RANGE_5_MS, AUDIO_RANGE_10_MS, AUDIO_RANGE_VARIABLE } e_audio_range_t;
typedef enum e_flap_sensor { FLAP_SENSOR_DISABLE, FLAP_SENSOR_GPIO_2, FLAP_SENSOR_GPIO_34, FLAP_SENSOR_GPIO_26 } e_flap_sensor_t;
typedef enum e_cruise_audio { AUDIO_S2F, AUDIO_VARIO } e_cruise_audio_2;
typedef enum e_vario_mode { VARIO_BRUTTO, VARIO_NETTO, CRUISE_NETTO } e_vario_mode_t;
typedef enum e_airspeed_sensor_type { PS_ABPMRR, PS_TE4525, PS_MP3V5004, PS_NONE } e_airspeed_sensor_type_t;
typedef enum e_netto_mode { NETTO_NORMAL, NETTO_RELATIVE } e_netto_mode_t;
typedef enum e_hardware_rev { HW_UNKNOWN=0, HW_LONG_VARIO=1, HW_XCVARIO_20=2, HW_XCVARIO_21=3 } e_hardware_rev_t;
typedef enum e_gload_mode { GLOAD_OFF=0, GLOAD_DYNAMIC=1, GLOAD_ALWAYS_ON=2 } e_gload_mode_t;
typedef enum e_windanalyser_mode { WA_OFF=0, WA_STRAIGHT=1, WA_CIRCLING=2, WA_BOTH=3 } e_windanalyser_mode_t;
typedef enum e_battery_display { BAT_PERCENTAGE, BAT_VOLTAGE, BAT_VOLTAGE_BIG } e_battery_display_t;
typedef enum e_wind_display { WD_NONE, WD_DIGITS, WD_ARROW, WD_BOTH, WD_COMPASS } e_wind_display_t;
typedef enum e_wind_reference { WR_NORTH, WR_HEADING, WR_GPS_COURSE } e_wind_reference_t;
typedef enum e_unit_type{ UNIT_NONE, UNIT_TEMPERATURE, UNIT_ALT, UNIT_SPEED, UNIT_VARIO, UNIT_QNH } e_unit_type_t;
typedef enum e_temperature_unit { T_CELCIUS, T_FAHRENHEIT, T_KELVIN } e_temperature_unit_t;
typedef enum e_alt_unit { ALT_UNIT_METER, ALT_UNIT_FT, ALT_UNIT_FL } e_alt_unit_t;
typedef enum e_speed_unit { SPEED_UNIT_KMH, SPEED_UNIT_MPH, SPEED_UNIT_KNOTS } e_speed_unit_t;
typedef enum e_vario_unit { VARIO_UNIT_MS, VARIO_UNIT_FPM, VARIO_UNIT_KNOTS } e_vario_unit_t;
typedef enum e_qnh_unit { QNH_HPA, QNH_INHG } e_qnh_unit_t;
typedef enum e_compasss_sensor_type { CS_DISABLE, CS_I2C, CS_I2C_NO_TILT, CS_CAN } e_compasss_sensor_type_t;

typedef enum e_sync { SYNC_NONE, SYNC_FROM_MASTER, SYNC_FROM_CLIENT, SYNC_BIDIR } e_sync_t;       // determines if data is synched from/to client. BIDIR means sync at commit from both sides
typedef enum e_reset { RESET_NO, RESET_YES } e_reset_t;   // determines if data is reset to defaults on factory reset
typedef enum e_volatility { VOLATILE, PERSISTENT, SEMI_VOLATILE } e_volatility_t;  // stored in RAM, FLASH, or into FLASH after a while
typedef enum e_can_speed { CAN_SPEED_OFF, CAN_SPEED_250KBIT, CAN_SPEED_500KBIT, CAN_SPEED_1MBIT } e_can_speed_t;  // stored in RAM, FLASH, or into FLASH after a while
typedef enum e_can_mode { CAN_MODE_MASTER, CAN_MODE_CLIENT, CAN_MODE_STANDALONE } e_can_mode_t;
typedef enum e_altimeter_select { AS_TE_SENSOR, AS_BARO_SENSOR, AS_EXTERNAL } e_altimeter_select_t;
typedef enum e_menu_screens { SCREEN_VARIO, SCREEN_GMETER, SCREEN_FLARM, SCREEN_THERMAL_ASSISTANT } e_menu_screens_t;
typedef enum e_s2f_arrow_color { AC_WHITE_WHITE, AC_BLUE_BLUE, AC_GREEN_RED } e_s2f_arrow_color_t;
typedef enum e_vario_needle_color { VN_COLOR_WHITE, VN_COLOR_ORANGE, VN_COLOR_RED }  e_vario_needle_color_t;
typedef enum e_data_monitor { MON_OFF, MON_BLUETOOTH, MON_WIFI_8880, MON_WIFI_8881, MON_WIFI_8882, MON_S1, MON_S2, MON_CAN  }  e_data_monitor_t;
typedef enum e_display_orientation { DISPLAY_NORMAL, DISPLAY_TOPDOWN } e_display_orientation_t;


const int baud[] = { 0, 4800, 9600, 19200, 38400, 57600, 115200 };
void change_bal();

typedef struct setup_flags{
	bool _reset    :1;
	bool _wait_ack :1;
	bool _volatile :1;
	uint8_t _sync  :2;
	uint8_t _unit  :3;
} t_setup_flags;


template<typename T>
class SetupNG: public SetupCommon
{
public:
	char typeName(void){
		if( typeid( T ) == typeid( float ) )
			return 'F';
		else if( typeid( T ) == typeid( int ) )
			return 'I';
		return 'U';
	}
	SetupNG( const char * akey,
			T adefault,  				   // unique identification TAG
			bool reset=true,               // reset data on factory reset
			e_sync_t sync=SYNC_NONE,
			e_volatility vol=PERSISTENT, // sync with client device is applicable
			void (* action)()=0,
			e_unit_type_t unit = UNIT_NONE
	)
	{
		// ESP_LOGI(FNAME,"SetupNG(%s)", akey );
		if( strlen( akey ) > 15 )
			ESP_LOGE(FNAME,"SetupNG(%s) key > 15 char !", akey );
		instances->push_back( this );  // add into vector
		_key = akey;
		_default = adefault;
		flags._reset = reset;
		flags._sync = sync;
		flags._volatile = vol;
		flags._wait_ack = false;
		flags._unit = unit;
		_action = action;
	}

	inline T* getPtr() {
		return &_value;
	}
	inline T& getRef() {
		return _value;
	}
	inline T get() const {
		return _value;
	}
	const char * key() {
		return _key;
	}
	virtual T getGui() const { return get(); } // tb. overloaded for blackboard
	virtual const char* unit() const { return ""; } // tb. overloaded for blackboard

	bool set( T aval, bool dosync=true, bool doAct=true ) {
		if( _value == aval ){
			// ESP_LOGI(FNAME,"Value already in config: %s", _key );
			return( true );
		}
		_value = aval;

		if ( dosync ) {
			sync();
		}
		else if( (flags._sync == SYNC_BIDIR) && isClient() ){
			sendAck();
		}
		if( doAct ){
			if( _action != 0 ) {
				(*_action)();
			}
		}
		if( flags._volatile == VOLATILE ){
			return true;
		}
		return commit( false );
	}

	e_unit_type_t unitType() {
		return (e_unit_type_t)flags._unit;
	}

	void ack( T aval ){
		if( aval != _value ){
			ESP_LOGI(FNAME,"sync to value client has acked");
			_value = aval;
			flags._wait_ack = false;
		}
	}

	void sendAck(){
		sendSetup( flags._sync, _key, typeName(), (void *)(&_value), sizeof( _value ), true );
	}

	bool sync(){
		if( SetupCommon::mustSync( flags._sync ) ){
			// ESP_LOGI( FNAME,"Now sync %s", _key );
			sendSetup( flags._sync, _key, typeName(), (void *)(&_value), sizeof( _value ) );
			if( isMaster() && flags._sync == SYNC_BIDIR )
				flags._wait_ack = true;
			return true;
		}
		return false;
	}

	bool commit(bool dosync=true) {
		ESP_LOGI(FNAME,"NVS commit(): ");
		if( dosync )
			sync();

		if( flags._volatile != PERSISTENT ){
			return true;
		}
        nvs_handle_t h = 0;
		if( !open(h) ) {
			return false;
		}
		ESP_LOGI(FNAME,"NVS commit(key:%s , addr:%08x, len:%d, nvs_handle: %04x)", _key, (unsigned int)(&_value), sizeof( _value ), h);
		esp_err_t err = nvs_set_blob(h, _key, (void *)(&_value), sizeof( _value ));
		if(err != ESP_OK) {
			ESP_LOGE(FNAME,"NVS set blob error %d", err );
			close(h);
			return( false );
		}
		if(lazyCommit) {
            _dirty=true;
            close(h);
            return true;
        }
		err = nvs_commit(h);
		close(h);
		if(err != ESP_OK)  {
			return false;
		}
		ESP_LOGI(FNAME,"success");
		return true;
	}

	bool exists() {
		if( flags._volatile != PERSISTENT ) {
            return true;
        }
        nvs_handle_t h = 0;
		if( !open(h) ) {
			return false;
		}
		size_t required_size;
		esp_err_t err = nvs_get_blob(h, _key, NULL, &required_size);
        close(h);
		if ( err != ESP_OK )
			return false;
		return true;
	}



	virtual bool init() {
		if( flags._volatile != PERSISTENT ){
			ESP_LOGI(FNAME,"NVS volatile set default");
			set( _default );
			return true;
		}
        nvs_handle_t h = 0;
		if( !open(h) ) {
			return false;
		}
		size_t required_size;
		esp_err_t err = nvs_get_blob(h, _key, NULL, &required_size);
		if ( err != ESP_OK ){
			ESP_LOGE(FNAME, "%s: NVS nvs_get_blob error: returned error ret=%d", _key, err );
			set( _default );  // try to init
			commit(false);
		}
		else {
			if( required_size > sizeof( T ) ) {
				ESP_LOGE(FNAME,"NVS error: size too big: %d > %d", required_size , sizeof( T ) );
				erase();
				set( _default );  // try to init
				close(h);
				return false;
			}
			else {
				// ESP_LOGI(FNAME,"NVS size okay: %d", required_size );
				err = nvs_get_blob(h, _key, &_value, &required_size);
				if ( err != ESP_OK ){
					ESP_LOGE(FNAME, "NVS nvs_get_blob returned error ret=%d", err );
					erase();
					set( _default );  // try to init
					commit(false);
				}
				else {
					ESP_LOGI(FNAME,"NVS key %s exists len: %d", _key, required_size );
				}
			}
		}
		close(h);
		return true;
	}


	virtual bool erase() {
		if( flags._volatile != PERSISTENT ){
			return true;
		}
        nvs_handle_t h = 0;
		open(h);
		esp_err_t err = nvs_erase_key(h, _key);
		if(err != ESP_OK)
			return false;
		else {
			ESP_LOGI(FNAME,"NVS erased %s by handle %d", _key, h );
			if( set( _default ) )
				return true;
			else
				return false;
		}
	}

	virtual bool mustReset() {
		return flags._reset;
	}

    inline T getDefault() const { return _default; }
	inline uint8_t getSync() { return flags._sync; }

private:
	T _value;
	T _default;
	const char * _key;
	t_setup_flags flags;
	void (* _action)();
};

extern SetupNG<float> 		QNH;
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
extern SetupNG<int>			log_scale;
extern SetupNG<float>  		ballast;
extern SetupNG<float>  		ballast_kg;
extern SetupNG<float>		empty_weight;
extern SetupNG<float>		crew_weight;
extern SetupNG<float>		gross_weight;

extern SetupNG<float>  		s2f_speed;

extern SetupNG<int>  		audio_variable_frequency;
extern SetupNG<int>  		audio_mode;
extern SetupNG<int>  		chopping_mode;
extern SetupNG<int>  		chopping_style;
extern SetupNG<int>  		amplifier_shutdown;

extern SetupNG<int>  		wireless_type;
extern SetupNG<float>  		wifi_max_power;
extern SetupNG<int>  		factory_reset;
extern SetupNG<int>  		audio_range;
extern SetupNG<int>  		alt_select;
extern SetupNG<int>  		fl_auto_transition;
extern SetupNG<int>  		alt_display_mode;
extern SetupNG<float>  		transition_alt;
extern SetupNG<int>  		glider_type;
extern SetupNG<int>  		glider_type_index;
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
extern SetupNG<float>  		audio_volume;
extern SetupNG<float>  		default_volume;
extern SetupNG<float>  		frequency_response;
extern SetupNG<float>  		max_volume;
extern SetupNG<float>  		s2f_deadband;
extern SetupNG<float>  		s2f_deadband_neg;
extern SetupNG<float>  		s2f_delay;
extern SetupNG<float>  		factory_volt_adjust;
extern SetupNG<float>  		bugs;
extern SetupNG<int>  		cruise_mode;
extern SetupNG<float>  		OAT;
extern SetupNG<float>  		OAT;   // outside temperature
extern SetupNG<float>  		swind_dir;   // straight wind direction
extern SetupNG<float>  		swind_speed;
extern SetupNG<float>  		cwind_dir;   // cirling wind direction
extern SetupNG<float>  		cwind_speed;
extern SetupNG<float>  		mag_hdm;
extern SetupNG<float>  		mag_hdt;
extern SetupNG<float>  		average_climb;
extern SetupNG<float>  		flap_pos;
extern SetupNG<float>  		altitude;
extern SetupNG<float>  		ias;
extern SetupNG<float>  		te_vario;

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
extern SetupNG<int>  		alt_quantization;
extern SetupNG<int>  		ias_unit;
extern SetupNG<int>  		vario_unit;
extern SetupNG<int>  		temperature_unit;
extern SetupNG<int>  		qnh_unit;
extern SetupNG<int>  		rot_default;

extern SetupNG<int>  		serial1_speed;
extern SetupNG<int>  		serial1_rxloop;
extern SetupNG<int>  		serial1_tx;
extern SetupNG<int>			serial1_pins_twisted;
extern SetupNG<int>  		serial1_tx_inverted;
extern SetupNG<int>  		serial1_rx_inverted;
extern SetupNG<int>  		serial1_tx_enable;
extern SetupNG<int>  		serial2_speed;
extern SetupNG<int>  		serial2_tx;
extern SetupNG<int>			serial2_pins_twisted;
extern SetupNG<int>  		serial2_tx_inverted;
extern SetupNG<int>  		serial2_rx_inverted;
extern SetupNG<int>  		serial2_tx_enable;
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
extern SetupNG<float>		ahrs_gyro_factor;
extern SetupNG<int>		    display_style;
extern SetupNG<int>		    display_variant;
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
extern SetupNG<float>     	flap_pos_max;
extern SetupNG<float>     	flap_neg_max;
extern SetupNG<int>       	compass_enable;
extern SetupNG<int>       	compass_dev_auto;
extern SetupNG<float>       compass_dev_0;
extern SetupNG<float>       compass_dev_45;
extern SetupNG<float>       compass_dev_90;
extern SetupNG<float>       compass_dev_135;
extern SetupNG<float>       compass_dev_180;
extern SetupNG<float>       compass_dev_225;
extern SetupNG<float>       compass_dev_270;
extern SetupNG<float>       compass_dev_315;
extern SetupNG<float>       compass_x_bias;
extern SetupNG<float>       compass_y_bias;
extern SetupNG<float>       compass_z_bias;
extern SetupNG<float>       compass_x_scale;
extern SetupNG<float>       compass_y_scale;
extern SetupNG<float>       compass_z_scale;
extern SetupNG<int>         compass_calibrated;
extern SetupNG<float>       compass_declination;
extern SetupNG<int>         compass_declination_valid;
extern SetupNG<float>		compass_damping;
extern SetupNG<int>         compass_nmea_hdm;
extern SetupNG<int>         compass_nmea_hdt;
extern SetupNG<float>		compass_i2c_cl;
extern SetupNG<int> 		s2f_blockspeed;
extern SetupNG<int>			needle_color;
extern SetupNG<int>			s2f_arrow_color;
extern SetupNG<float>  		s2f_hysteresis;
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
extern SetupNG<int>			airspeed_sensor_type;
extern SetupNG<int>			cruise_audio_mode;
extern SetupNG<int>			netto_mode;
extern SetupNG<float>		v_max;
extern SetupNG<int>			gload_mode;
extern SetupNG<float>		gload_pos_thresh;
extern SetupNG<float>		gload_neg_thresh;
extern SetupNG<float>		gload_pos_limit;
extern SetupNG<float>		gload_neg_limit;
extern SetupNG<float>		gload_pos_max;
extern SetupNG<float>		gload_neg_max;
extern SetupNG<int> 		wind_enable;
extern SetupNG<int> 		wind_logging;
extern SetupNG<float> 		wind_as_min;
extern SetupNG<float> 		wind_as_calibration;
extern SetupNG<float> 		wind_filter_lowpass;
extern SetupNG<float>       wind_gps_lowpass;
extern SetupNG<float>       wind_as_filter;
extern SetupNG<float>       wind_dev_filter;
extern SetupNG<int> 		wind_display;
extern SetupNG<int> 		wind_reference;
extern SetupNG<float> 		wind_max_deviation;
extern SetupNG<float>       max_circle_wind_diff;
extern SetupNG<float>       circle_wind_lowpass;
extern SetupNG<int> 		can_speed;
extern SetupNG<int> 		can_tx;
extern SetupNG<int> 		can_mode;
extern SetupNG<float> 		master_xcvario;
extern SetupNG<int> 		master_xcvario_lock;
extern SetupNG<int> 		menu_long_press;
extern SetupNG<int> 		menu_screens;
extern SetupNG<int> 		data_monitor;
extern SetupNG<t_bitfield_compass> 	calibration_bits;


extern uint8_t g_col_background;
extern uint8_t g_col_highlight;

extern int last_volume;

void change_ballast();
void change_mc();
