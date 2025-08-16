/*
 * SetupNG.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#include "setup/SetupNG.h"

#include "setup/SetupMenuCommon.h"
#include "setup/CruiseMode.h"
#include "math/Quaternion.h"
#include "ESP32NVS.h"
#include "ESPAudio.h"
#include "BMPVario.h"
#include "glider/Polars.h"
#include "mpu/types.hpp"  // MPU data types and definitions
#include "sensor.h"
#include "S2fSwitch.h"
#include "wind/StraightWind.h"
#include "wind/CircleWind.h"
#include "ESPAudio.h"
#include "Flap.h"
#include "OneWireESP32.h"
#include "comm/DeviceMgr.h"
#include "comm/CanBus.h"
#include "comm/Configuration.h"
#include "comm/SerialLine.h"
#include "protocol/NMEA.h"
#include "protocol/AliveMonitor.h"
#include "screen/element/Battery.h"
#include "screen/element/Altimeter.h"
#include "logdefnone.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp32/rom/uart.h>
#include <esp_system.h>
#include <esp_http_server.h>

#include <cstdio>
#include <cmath>
#include <string>
#include <iostream>
#include <map>

// Specialization for float
template<typename T>
bool SetupNG<T>::isValid() const { return true; }
template<>
bool SetupNG<float>::isValid() const {
	if ( std::isnan(_value) ) { return false; }
	if ( _limt ) {
		// check min/max limit
		if ( _value < _limt->_min ) { return false; }
		if ( _value > _limt->_max ) { return false; }
	}
	return true;
}
template<>
bool SetupNG<int>::isValid() const {
	if ( _limt ) {
		// check min/max limit
		if ( _value < (int)_limt->_min ) { return false; }
		if ( _value > (int)_limt->_max ) { return false; }
	}
	return true;
}


void change_mc() {
	Speed2Fly.change_mc();
}

void change_ballast() {
	Speed2Fly.change_ballast();
}

void change_crew_weight() {
	ProtocolItf *prtcl = DEVMAN->getProtocol(NAVI_DEV, XCVARIO_P);
	if ( prtcl ) {
		(static_cast<NmeaPrtcl*>(prtcl))->sendXCVCrewWeight(crew_weight.get());
	}
	change_ballast();
}

void change_empty_weight(){
	ProtocolItf *prtcl = DEVMAN->getProtocol(NAVI_DEV, XCVARIO_P);
	if ( prtcl ) {
		(static_cast<NmeaPrtcl*>(prtcl))->sendXCVEmptyWeight(empty_weight.get());
	}
	change_ballast();
}

void change_bal_water(){
	ProtocolItf *prtcl = DEVMAN->getProtocol(NAVI_DEV, XCVARIO_P);
	if ( prtcl ) {
		(static_cast<NmeaPrtcl*>(prtcl))->sendXCVWaterWeight(ballast_kg.get());
	}
	change_ballast();
}

void polar_update(){
	Polars::extract(glider_type_index.get());
	Speed2Fly.setPolar();
}

void modifyPolar() {
	Speed2Fly.modifyPolar();
}

void change_cruise() {
	VCMode.updateCache();
}

void resetSWindAge() {
	if( swind_dir.get() != 0 && swind_speed.get() != 0 ) { // do not reset age at initial sync
		StraightWind::resetAge();
	}
}
void resetCWindAge() {
	if( cwind_dir.get() != 0 && cwind_speed.get() != 0 ) {
		CircleWind::resetAge();
	}
}

void change_volume() {
	float vol = audio_volume.get();
	AUDIO->setVolume( vol );
	ESP_LOGI(FNAME,"change_volume -> %f", vol );
}

void change_max_volume() {
	float max = max_volume.get();
	if ( audio_volume.get() > max ) {   // enforce max_volume
		audio_volume.set( max );
		ESP_LOGI(FNAME,"change volume -> %f to fit max", max );
	}
}

void flap_act() {
    if (flap_enable.get()) {
        FLAP = Flap::theFlap();
    }
    else if ( FLAP ) {
        delete FLAP;
		FLAP = nullptr;
    }
}

void flap_speed_act() {
	if( FLAP ) {
		FLAP->initSpeeds();
	}
}

void flap_label_act() {
	if( FLAP ) {
		FLAP->initLabels();
	}
}

void send_config( httpd_req *req ){
	SetupCommon::giveConfigChanges( req );
};

int restore_config(int len, char *data){
	return( SetupCommon::restoreConfigChanges( len, data ) );
};

void chg_mpu_target(){
	mpu_target_temp = mpu_temperature.get();
};

void chg_display_orientation(){
	ESP_LOGI(FNAME, "display changed");
	imu_reference.setDefault();
};

static void ch_airborne_state(){
	ESP_LOGI(FNAME, "airborne state changed");

}


SetupNG<float>          MC(  "MacCready", 0.5, true, SYNC_BIDIR, PERSISTENT, change_mc, QUANT_VSPEED, LIMITS(0.0, 9.9, 0.1) );
SetupNG<float>  		QNH( "QNH", 1013.25, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_QNH, LIMITS(900, 1100.0, 0.250) );
SetupNG<float> 			polar_wingload( "POLAR_WINGLOAD", 34.40, true, SYNC_FROM_MASTER, PERSISTENT, change_ballast, QUANT_NONE, LIMITS(10.0, 100.0, 0.1) );
const limits_t polar_speed_limits = {0.0, 450.0, 1};
SetupNG<float> 			polar_speed1( "POLAR_SPEED1",   80, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar, QUANT_HSPEED, &polar_speed_limits);
const limits_t polar_sink_limits = {-6.0, 0.0, 0.01};
SetupNG<float> 			polar_sink1( "POLAR_SINK1",    -0.66, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar, QUANT_VSPEED, &polar_sink_limits);
SetupNG<float> 			polar_speed2( "POLAR_SPEED2",   125, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar, QUANT_HSPEED, &polar_speed_limits);
SetupNG<float> 			polar_sink2( "POLAR_SINK2",    -0.97, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar, QUANT_VSPEED, &polar_sink_limits);
SetupNG<float> 			polar_speed3( "POLAR_SPEED3",   175, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar, QUANT_HSPEED, &polar_speed_limits);
SetupNG<float> 			polar_sink3( "POLAR_SINK3",    -2.24, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar, QUANT_VSPEED, &polar_sink_limits);
SetupNG<float> 			polar_max_ballast( "POLAR_MAX_BAL",  80, true, SYNC_FROM_MASTER, PERSISTENT, change_ballast, QUANT_MASS, LIMITS(0, 500, 1));
SetupNG<float> 			polar_wingarea( "POLAR_WINGAREA", 10.5, true, SYNC_FROM_MASTER, PERSISTENT, change_ballast, QUANT_NONE, LIMITS(0, 50, 0.1));

SetupNG<float>  		speedcal( "SPEEDCAL", 0.0, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-100, 100, 1));
SetupNG<float>  		vario_delay( "VARIO_DELAY", 3.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(2.0, 10.0, 0.1));
SetupNG<float>  		vario_av_delay( "VARIO_AV_DELAY", 20.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(2.0, 60.0, 1)); // changed to 20 seconds (quasi standard) what equals to a half circle
SetupNG<float>  		center_freq( "AUDIO_CENTER_F", 500.0, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_NONE, LIMITS(200.0, 2000.0, 10.0));
SetupNG<float>  		tone_var( "OCTAVES", 2.0, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_NONE, LIMITS(1.2, 4, 0.05));
SetupNG<int>  			dual_tone( "DUAL_TONE", 0, true, SYNC_BIDIR );
SetupNG<float>  		high_tone_var( "HIGH_TONE_VAR", 12.0, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0, 50, 1.0));
SetupNG<float>  		deadband( "DEADBAND", 0.3, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_VSPEED, LIMITS(.0, 5.0, 0.1));
SetupNG<float>  		deadband_neg("DEADBAND_NEG" , -0.3, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_VSPEED, LIMITS(-5.0, .0, 0.1));
SetupNG<float>  		scale_range( "VARIO_RANGE", 5.0, true, SYNC_NONE, PERSISTENT, 0, QUANT_VSPEED, LIMITS(1.0, 30.0, 1));
SetupNG<int>			log_scale( "LOG_SCALE", 0 );
SetupNG<float>  		ballast( "BALLAST" , 0.0, true, SYNC_NONE, VOLATILE, 0 );  // ballast increase from reference weight in %
SetupNG<float>  		ballast_kg( "BAL_KG" , 0.0, true, SYNC_BIDIR, PERSISTENT, change_bal_water, QUANT_MASS, LIMITS(0.0, 500, 1));
SetupNG<float>			empty_weight( "EMPTY_WGT", 361.2, true, SYNC_BIDIR, PERSISTENT, change_empty_weight, QUANT_MASS, LIMITS(0, 1000, 1));
SetupNG<float>			crew_weight( "CREW_WGT", 80, true, SYNC_BIDIR, PERSISTENT, change_crew_weight, QUANT_MASS, LIMITS(0, 300, 1));
SetupNG<float>			gross_weight( "GROSS_WGT", 350, true, SYNC_NONE, VOLATILE ); // derived from above
SetupNG<float>  		bugs( "BUGS", 0.0, true, SYNC_BIDIR, VOLATILE, modifyPolar, QUANT_NONE, LIMITS(0.0, 50, 1));

SetupNG<int>  			cruise_mode( "CRUISE", AUDIO_VARIO, false, SYNC_BIDIR, VOLATILE, change_cruise ); // use the CruiseMode wrapper to access and modify
SetupNG<float>  		OAT( "OAT", DEVICE_DISCONNECTED_C, true, SYNC_FROM_MASTER, VOLATILE );   // outside temperature
SetupNG<float>  		swind_dir( "SWDD", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetSWindAge );
SetupNG<float>  		swind_speed( "SWDS", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetSWindAge );
SetupNG<float>  		swind_sideslip_lim( "SWSL", 2.0, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0, 45.0, 0.1));
SetupNG<float>  		cwind_dir( "CWDD", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetCWindAge );
SetupNG<float>  		cwind_speed( "CWDS", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetCWindAge );
SetupNG<int>  			extwind_sptc_dir( "EWDD", 0.0, false, SYNC_BIDIR, VOLATILE ); // synoptic and
SetupNG<int>  			extwind_sptc_speed( "EWDS", 0.0, false, SYNC_BIDIR, VOLATILE );
SetupNG<int>  			extwind_inst_dir( "EIWDD", 0.0, false, SYNC_BIDIR, VOLATILE ); // instant external wind
SetupNG<int>  			extwind_inst_speed( "EIWDS", 0.0, false, SYNC_BIDIR, VOLATILE );
SetupNG<int>  			extwind_status( "EWST", -1, false, SYNC_BIDIR, VOLATILE );
SetupNG<float>  		mag_hdm( "HDM", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		mag_hdt( "HDT", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		average_climb( "AVCL", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		flap_pos( "FLPS", -100.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		altitude( "ALTI", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		ias( "IASV", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		te_vario( "TEVA", 0.0, true, SYNC_FROM_MASTER, VOLATILE );

SetupNG<int>  			xcv_alive( "AL_XCV", ALIVE_NONE, false, SYNC_NONE, VOLATILE );
SetupNG<int>  			mags_alive( "AL_MAGS", ALIVE_NONE, false, SYNC_NONE, VOLATILE );
SetupNG<int>  			flarm_alive( "AL_FLARM", ALIVE_NONE, false, SYNC_NONE, VOLATILE );
SetupNG<int>  			airborne("AIRBORNE", 0, false, SYNC_FROM_MASTER, VOLATILE, &ch_airborne_state);

SetupNG<int>  			s2f_ideal( "S2F_IDEAL", 100.0, false, SYNC_FROM_MASTER, VOLATILE);
SetupNG<int>  			s2f_switch_mode( "AUDIO_MODE" ,  AM_AUTOSPEED, false, SYNC_BIDIR, PERSISTENT );
SetupNG<float>  		s2f_threshold( "S2F_SPEED", 100.0, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_HSPEED, LIMITS(20.0, 250.0, 1.0));
SetupNG<float>  		s2f_flap_pos( "S2F_FLAP", 1, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-3, 3, 0.1));
static const limits_t percentage_limits = {0, 100, 1.0};
SetupNG<float>  		s2f_gyro_deg( "S2F_GYRO", 10, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_NONE, &percentage_limits);
SetupNG<float>  		s2f_auto_lag( "S2F_HYST", 10, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_NONE, LIMITS(2, 20, 1));

SetupNG<float> 			audio_volume("AUD_VOL", 10, true, SYNC_BIDIR, VOLATILE, change_volume, QUANT_NONE, &percentage_limits);
SetupNG<int>  			audio_split_vol( "AUD_SPLIT", 0 );
SetupNG<int>  			audio_variable_frequency( "AUD_VAFQ", 0);
SetupNG<int>  			chopping_mode( "CHOPPING_MODE",  VARIO_CHOP );
SetupNG<int>  			chopping_style( "CHOP_STYLE",  AUDIO_CHOP_SOFT );
SetupNG<int>  			amplifier_shutdown( "AMP_DIS", AMP_STAY_ON );
SetupNG<int>            audio_equalizer( "AUD_EQ", AUDIO_EQ_DISABLE, false );

SetupNG<float>  		wifi_max_power( "WIFI_MP" , 50, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(10.0, 100.0, 5.0));
SetupNG<int>  			factory_reset( "FACTORY_RES" , 0 );
SetupNG<int>  			audio_range( "AUDIO_RANGE" , AUDIO_RANGE_5_MS );
SetupNG<int>  			alt_select( "ALT_SELECT" , AS_BARO_SENSOR );
SetupNG<int>  			fl_auto_transition( "FL_AUTO" , 0 );
SetupNG<int>  			alt_display_mode( "ALT_DISP_MODE" , Altimeter::MODE_QNH );
SetupNG<float>  		transition_alt( "TRANS_ALT", 50, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0, 400, 10)); // Transition Altitude
SetupNG<int>  			glider_type_index( "GLIDER_TYPE_IDX", 1000, true, SYNC_FROM_MASTER, PERSISTENT, polar_update );

SetupNG<float>  		as_offset( "AS_OFFSET" , -1 );
static const limits_t bat_limits = {0.0, 28.0, 0.1};
SetupNG<float>  		bat_low_volt( "BAT_LOW_VOLT" , 11.5, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, &bat_limits);
SetupNG<float>  		bat_red_volt( "BAT_RED_VOLT", 11.75, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, &bat_limits);
SetupNG<float>  		bat_yellow_volt( "BAT_YELLOW_VOLT" , 12.0, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, &bat_limits);
SetupNG<float>  		bat_full_volt( "BAT_FULL_VOLT", 12.8, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, &bat_limits);
SetupNG<float>  		core_climb_period( "CORE_CLIMB_P" , 60, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, LIMITS(60, 300, 1));
SetupNG<float>  		core_climb_min( "CORE_CLIMB_MIN" , 0.5, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.0, 2.0, 0.1));
SetupNG<float>  		core_climb_history( "CORE_CLIMB_HIST" , 45, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, LIMITS(1, 300, 1));
SetupNG<float>  		mean_climb_major_change( "MEAN_CLMC", 0.5, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.1, 5.0, 0.1));
SetupNG<float>  		elevation( "ELEVATION", NOTSET_ELEVATION, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_ALT, LIMITS(NOTSET_ELEVATION, 3000, 1));
SetupNG<float>  		default_volume( "DEFAULT_VOL", 25.0, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, &percentage_limits);
SetupNG<float>  		max_volume( "MAXI_VOL", 60.0, true, SYNC_FROM_MASTER, PERSISTENT, change_max_volume, QUANT_NONE, &percentage_limits);
SetupNG<float>  		frequency_response( "FREQ_RES", 30.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-70.0, 70.0, 1.0));
SetupNG<float>  		s2f_deadband( "DEADBAND_S2F", 10.0, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_HSPEED, LIMITS(.0, 25.0, 1));
SetupNG<float>  		s2f_deadband_neg( "DB_S2F_NEG", -10.0, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_HSPEED, LIMITS(-25.0, .0, 1));
SetupNG<float>  		s2f_delay( "S2F_DELAY", 5.0, true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.10001, 10.0, 0.1));
SetupNG<float>  		factory_volt_adjust("FACT_VOLT_ADJ", 0.00815, false, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-25.0, 25.0, 0.01));

SetupNG<int>  			display_type( "DISPLAY_TYPE",  UNIVERSAL );
SetupNG<int>  			display_test( "DISPLAY_TEST", 0, false, SYNC_NONE, VOLATILE );
SetupNG<int>  			display_orientation("DISPLAY_ORIENT" , DISPLAY_NORMAL, true, SYNC_NONE, PERSISTENT, chg_display_orientation );
SetupNG<int>  			flap_enable( "FLAP_ENABLE", 0, true, SYNC_BIDIR, PERSISTENT, flap_act);
SetupNG<float>  		flap_minus_3( "FLAP_MINUS_3", 200, true, SYNC_BIDIR, PERSISTENT, flap_speed_act, QUANT_HSPEED, &polar_speed_limits);
SetupNG<float>  		flap_minus_2( "FLAP_MINUS_2", 165, true, SYNC_BIDIR, PERSISTENT, flap_speed_act, QUANT_HSPEED, &polar_speed_limits);
SetupNG<float>  		flap_minus_1( "FLAP_MINUS_1", 105, true, SYNC_BIDIR, PERSISTENT, flap_speed_act, QUANT_HSPEED, &polar_speed_limits);
SetupNG<float>  		flap_0(       "FLAP_0", 88, true, SYNC_BIDIR, PERSISTENT, flap_speed_act, QUANT_HSPEED, &polar_speed_limits);
SetupNG<float>  		flap_plus_1(  "FLAP_PLUS_1", 78, true, SYNC_BIDIR, PERSISTENT, flap_speed_act, QUANT_HSPEED, &polar_speed_limits);
SetupNG<float>  		flap_plus_2(  "FLAP_PLUS_2", 70, true, SYNC_BIDIR, PERSISTENT, flap_speed_act, QUANT_HSPEED, &polar_speed_limits);
SetupNG<int>  			alt_unit( "ALT_UNIT", ALT_UNIT_METER );
SetupNG<int>  			alt_quantization( "ALT_QUANT", Altimeter::ALT_QUANT_10 );
SetupNG<int>  			ias_unit( "IAS_UNIT", SPEED_UNIT_KMH );
SetupNG<int>  			vario_unit( "VARIO_UNIT", VARIO_UNIT_MS );
SetupNG<int>  			temperature_unit( "TEMP_UNIT", T_CELCIUS );
SetupNG<int>  			dst_unit( "DST_UNIT", DST_UNIT_M );
SetupNG<int>  			qnh_unit("QNH_UNIT", QNH_HPA );
SetupNG<int>  			rot_default( "ROTARY_DEFAULT", 0 );
SetupNG<int>  			serial1_speed( "SERIAL1_SPEED", BAUD_19200, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(BAUD_4800, BAUD_115200, 1));
SetupNG<int>  			serial1_pin_swap( "SERIAL1_PINS", 0 );
SetupNG<int>  			serial1_ttl_signals( "SERIAL1_TTL", RS232_TTL );
SetupNG<int>  			serial1_tx_enable( "SER1_TX_ENA", 1 );
SetupNG<int>  			serial2_speed( "SERIAL2_SPEED", BAUD_38400, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(BAUD_4800, BAUD_115200, 1));
SetupNG<int>  			serial2_pin_swap( "SERIAL2_PINS", 0 );
SetupNG<int>  			serial2_ttl_signals( "SERIAL2_TTL", RS232_NORMAL );
SetupNG<int>  			serial2_tx_enable( "SER2_TX_ENA", 1 );
SetupNG<int>  			software_update( "SOFTWARE_UPDATE", 0 );
SetupNG<int>  			battery_display( "BAT_DISPLAY", Battery::BAT_PERCENTAGE );
SetupNG<int>  			airspeed_mode( "AIRSPEED_MODE", MODE_IAS );
SetupNG<int>  			nmea_protocol( "NMEA_PROTOCOL", XCVARIO_P );
SetupNG<int>		    log_level( "LOG_LEVEL", 3 );
SetupNG<float>		    audio_factor( "AUDIO_FACTOR", 1, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.1, 2, 0.025));
SetupNG<float>		    te_comp_adjust ( "TECOMP_ADJ", 0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-100, 100, 0.1));
SetupNG<int>		    te_comp_enable( "TECOMP_ENA", TE_TEK_PROBE );
SetupNG<int>		    rotary_dir( "ROTARY_DIR", 0 );
SetupNG<int>		    rotary_inc( "ROTARY_INC", 1 );
SetupNG<int>		    student_mode( "STUD_MOD", 0 );
SetupNG<float>		    password( "PASSWORD", 0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0, 1000, 1));
SetupNG<int>		    autozero( "AUTOZERO", 1 );  // AS autozero at factory
SetupNG<int>		    ahrs_rpyl_dataset("RPYL", 0 );
SetupNG<int>		    ahrs_autozero("AHRSAZ", 0 );
SetupNG<float>		    ahrs_gyro_factor("AHRSMGYF", 100, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &percentage_limits);
SetupNG<float>		    ahrs_min_gyro_factor("AHRSLGYF", 20, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &percentage_limits);
SetupNG<float>		    ahrs_dynamic_factor("AHRSGDYN", 5, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.5, 10, 0.1));
SetupNG<int>		    ahrs_roll_check("AHRSRCHECK", 0 );
SetupNG<float>       	gyro_gating("GYRO_GAT", 1.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0, 10, 0.1));
SetupNG<int>		    display_style("DISPLAY_STYLE", 1 );
SetupNG<int>		    s2f_switch_type("S2FHWSW", S2F_HW_SWITCH );
SetupNG<int>		    hardwareRevision("HWREV", HW_UNKNOWN );
SetupNG<int>		    ahrs_licence_dig1("AHRS_LIC_1_2", 0, false );
SetupNG<int>		    ahrs_licence_dig2("AHRS_LIC_2", 0, false );
SetupNG<int>		    ahrs_licence_dig3("AHRS_LIC_3", 0, false );
SetupNG<int>		    ahrs_licence_dig4("AHRS_LIC_4", 0, false );
// SetupNG<int>		    dummy("DUMMY", 0 );
SetupNG<int>		    wk_sens_pos_plus_3("WKSP3", 1000);
SetupNG<int>		    wk_sens_pos_plus_2("WKSP2", 1500);
SetupNG<int>		    wk_sens_pos_plus_1("WKSP1", 2000);
SetupNG<int>		    wk_sens_pos_0("WKSP0", 2500);
SetupNG<int>		    wk_sens_pos_minus_1("WKSM1", 3000);
SetupNG<int>		    wk_sens_pos_minus_2("WKSM2", 3500);
SetupNG<int>		    wk_sens_pos_minus_3("WKSM3", 4000);
SetupNG<int>            stall_warning( "STALL_WARN", 0 );
SetupNG<float>			stall_speed( "STALL_SPEED", 70, true, SYNC_FROM_MASTER, PERSISTENT, 0, QUANT_HSPEED, &polar_speed_limits);
SetupNG<int>            flarm_warning( "FLARM_LEVEL", 1 );
static const limits_t volume_limits = {0, 100, 1};
SetupNG<float>          flarm_volume( "FLARM_VOL", 100, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &volume_limits);
SetupNG<float>          flarm_alarm_time( "FLARM_ALM", 5, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(1, 15, 1));
SetupNG<int>            flap_sensor( "FLAP_SENS", 0, false, SYNC_FROM_MASTER, PERSISTENT, flap_act); // fixme synch logic broken
SetupNG<float>          compass_dev_0( "CP_DEV_0", 0 );
SetupNG<float>          compass_dev_45( "CP_DEV_45", 0 );
SetupNG<float>          compass_dev_90( "CP_DEV_90", 0 );
SetupNG<float>          compass_dev_135( "CP_DEV_135", 0 );
SetupNG<float>          compass_dev_180( "CP_DEV_180", 0 );
SetupNG<float>          compass_dev_225( "CP_DEV_225", 0 );
SetupNG<float>          compass_dev_270( "CP_DEV_279", 0 );
SetupNG<float>          compass_dev_315( "CP_DEV_315", 0 );
SetupNG<float>          compass_x_bias( "CP_X_BIAS", 0 );
SetupNG<float>          compass_y_bias( "CP_Y_BIAS", 0 );
SetupNG<float>          compass_z_bias( "CP_Z_BIAS", 0 );
SetupNG<float>          compass_x_scale( "CP_X_SCALE", 1.0 );
SetupNG<float>          compass_y_scale( "CP_Y_SCALE", 1.0 );
SetupNG<float>          compass_z_scale( "CP_Z_SCALE", 1.0 );
SetupNG<int>            compass_calibrated( "CP_CALIBRATED", 0 );
SetupNG<float>          compass_declination( "CP_DECL", 0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-180, 180, 1.0));
SetupNG<int>            compass_declination_valid( "CP_DECL_VALID", 0 );
SetupNG<float>          compass_damping( "CPS_DAMP", 1.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.1, 10.0, 0.1));
SetupNG<int>            compass_nmea_hdm( "CP_NMEA_HDM", 0 );
SetupNG<int>            compass_nmea_hdt( "CP_NMEA_HDT", 0 );
SetupNG<float>          compass_i2c_cl("CP_I2C_CL", 100, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(10.0, 400.0, 10));
SetupNG<float>          wind_as_filter( "WINDASF", 0.02, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0, 0.05, 0.001));
SetupNG<float>          wind_gps_lowpass( "WINDGPSLP", 1.00, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.1, 10.0, 0.1));
SetupNG<float>          wind_dev_filter( "WINDDEVF", 0.010, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0, 0.05, 0.001));
SetupNG<int> 			wind_enable( "WIND_ENA", WA_OFF );
SetupNG<int> 			wind_northup( "WNDG_NUP", 0 );
SetupNG<int> 			wind_logging( "WIND_LOG", 0 );
SetupNG<float> 			wind_as_calibration("WIND_AS_CAL", 1.0 );
SetupNG<float> 			wind_filter_lowpass("SWINDAVER", 60, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(5, 120, 1));
SetupNG<float> 			wind_straight_course_tolerance("WINDSTOL", 7.5, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(2.0, 30.0, 0.1));
SetupNG<float> 			wind_straight_speed_tolerance("WINDSSTOL", 15, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(1.0, 30.0, 1));
SetupNG<int> 			wind_display( "WIND_DIS", WD_NONE, true, SYNC_FROM_MASTER );
SetupNG<int> 			wind_reference( "WIND_REF", WR_HEADING );
SetupNG<float> 			wind_max_deviation("WIND_MDEV", 30.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.0, 180.0, 1.0));
SetupNG<int> 			s2f_blockspeed( "S2G_BLOCKSPEED", 0, true, SYNC_BIDIR );  // considering netto vario and g load for S2F or not
SetupNG<int> 			needle_color("NEEDLE_COLOR", VN_COLOR_ORANGE );
SetupNG<int>			s2f_arrow_color("S2F_ARRCOL", AC_WHITE_WHITE );
SetupNG<int> 			wk_label_plus_3( "WKLP3", 41,  true, SYNC_BIDIR, PERSISTENT, flap_label_act);  //  L
SetupNG<int> 			wk_label_plus_2( "WKLP2", 11,  true, SYNC_BIDIR, PERSISTENT, flap_label_act);  //  2
SetupNG<int> 			wk_label_plus_1( "WKLP1", 10,  true, SYNC_BIDIR, PERSISTENT, flap_label_act);  //  1
SetupNG<int> 			wk_label_null_0( "WKL0",   9,  true, SYNC_BIDIR, PERSISTENT, flap_label_act);  //  0
SetupNG<int> 			wk_label_minus_1( "WKLM1", 8,  true, SYNC_BIDIR, PERSISTENT, flap_label_act);  // -1
SetupNG<int> 			wk_label_minus_2( "WKLM2", 7,  true, SYNC_BIDIR, PERSISTENT, flap_label_act);  // -2
SetupNG<int> 			wk_label_minus_3( "WKLM3", 42,  true, SYNC_BIDIR, PERSISTENT, flap_label_act); //  S
SetupNG<float>       	flap_takeoff("FLAPTO", 1,  true, SYNC_BIDIR, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-3, 3, 1));
SetupNG<int> 			audio_mute_menu( "AUDIS", 0 );
SetupNG<int> 			audio_mute_sink( "AUDISS", 0 );
SetupNG<int> 			audio_mute_gen( "AUDISG", AUDIO_ON );
SetupNG<int>			vario_mode("VAMOD", CRUISE_ONLY_NETTO, true, SYNC_NONE, PERSISTENT, change_cruise);  // switch to netto mode when cruising
SetupNG<int>			airspeed_sensor_type("PTYPE", PS_NONE, false);
SetupNG<int>			cruise_audio_mode("CAUDIO", 0 );
SetupNG<int>			netto_mode("NETMOD", NETTO_RELATIVE, true, SYNC_NONE, PERSISTENT, change_cruise);  // regard polar sink
SetupNG<float>			v_max("VMAX", 270, true, SYNC_FROM_MASTER, PERSISTENT, 0, QUANT_HSPEED, &polar_speed_limits);
static const limits_t pos_g_limits = {1.0, 8.0, 0.1};
SetupNG<float>			gload_pos_thresh("GLOADPT", 4, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &pos_g_limits);
static const limits_t neg_g_limits = {-8.0, 1.0, 0.1};
SetupNG<float>			gload_neg_thresh("GLOADNT", -3, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &neg_g_limits);
SetupNG<float>			gload_pos_limit_low("GLOADPLL", 3, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &pos_g_limits);
SetupNG<float>			gload_neg_limit_low("GLOADNLL", -2, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &neg_g_limits);
SetupNG<float>			gload_pos_limit("GLOADPL", 5, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &pos_g_limits);
SetupNG<float>			gload_neg_limit("GLOADNL", -3, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &neg_g_limits);
SetupNG<float>			gload_pos_max("GLOADPM", 1);
SetupNG<float>			gload_neg_max("GLOADNM", 0);
SetupNG<float>			airspeed_max("ASMAX", 0 );
SetupNG<float>		    gload_alarm_volume("GLOADAVOL", 100, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, &volume_limits);
SetupNG<int>        	display_variant("DISPLAY_VARIANT", 0 );
SetupNG<int>        	compass_dev_auto("COMPASS_DEV", 0 );
SetupNG<float>       	max_circle_wind_diff("CI_WINDDM", 60.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0, 90.0, 1.0));
SetupNG<float>       	max_circle_wind_delta_deg("CIMDELD", 20.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.0, 60.0, 0.1));
SetupNG<float>       	max_circle_wind_delta_speed("CIMDELS", 5.0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(0.0, 20.0, 0.1));
SetupNG<float>       	circle_wind_lowpass("CI_WINDLOW", 5, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(1, 10, 1));
SetupNG<int> 			can_speed( "CANSPEED", CAN_SPEED_1MBIT, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(CAN_SPEED_250KBIT, CAN_SPEED_1MBIT, 1));
SetupNG<float> 			master_xcvario( "MSXCV", 0, false, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(1000, 9999, 1));
SetupNG<int> 			menu_long_press("MENU_LONG", 0 );
SetupNG<int> 			screen_gmeter("SCR_GMET", SCREEN_OFF, false);
SetupNG<int> 			screen_horizon("SCR_HORIZ", SCREEN_OFF);
SetupNG<int> 			vario_centeraid("SCR_CA", 0, false);
SetupNG<int> 			vario_upper_gauge("SCR_GT", GAUGE_NONE, false);
SetupNG<int> 			vario_lower_gauge("SCR_GB", GAUGE_NONE, false);
SetupNG<int> 			vario_mc_gauge("SCR_GMC", 1, false);
SetupNG<bitfield_compass>  calibration_bits("CALBIT", { 0,0,0,0,0,0 } );
SetupNG<int> 			gear_warning("GEARWA", 0 );
SetupNG<t_tenchar_id>	custom_wireless_id("WLID", t_tenchar_id("") );
SetupNG<int> 			logging("LOGGING", 0 );
SetupNG<float>      	display_clock_adj("DSCLADHJ", 0, true, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-2, 2, 0.1));

SetupNG<float>				glider_ground_aa("GLD_GND_AA", 12.0, true, SYNC_FROM_MASTER, PERSISTENT, nullptr, QUANT_NONE, LIMITS(-5, 20, 1));
SetupNG<Quaternion>			imu_reference("IMU_REFERENCE", Quaternion(), false);
SetupNG<mpud::raw_axes_t>	gyro_bias("GYRO_BIAS", {} );
SetupNG<mpud::raw_axes_t>	accl_bias("ACCL_BIAS", {} );
SetupNG<float>              mpu_temperature("MPUTEMP", 45.0, true, SYNC_FROM_MASTER, PERSISTENT, chg_mpu_target, QUANT_NONE, LIMITS(-1, 60, 1)); // default for AHRS chip temperature (XCV 2023)
SetupNG<int> 			xcv_role("XCVROLE", MASTER_ROLE, false, SYNC_NONE, PERSISTENT, nullptr, QUANT_NONE, LIMITS(MASTER_ROLE, SECOND_ROLE, 1));
// Those device entries are serving as factory reset minimum configuration
SetupNG<DeviceNVS>		anemoi_devsetup("ANEMOI", DeviceNVS() );
SetupNG<DeviceNVS>		auto_connect("AUTOCON", DeviceNVS() );
SetupNG<DeviceNVS>		flarm_devsetup("FLARM", DeviceNVS() );
SetupNG<DeviceNVS>		master_devsetup("MASTER", DeviceNVS() );
SetupNG<DeviceNVS>		second_devsetup("SECOND", DeviceNVS() );
SetupNG<DeviceNVS>		magleg_devsetup("MAGLEG", DeviceNVS() );
SetupNG<DeviceNVS>		magsens_devsetup("MAGSENS", DeviceNVS() );
SetupNG<DeviceNVS>		navi_devsetup("NAVI", DeviceNVS() );
SetupNG<DeviceNVS>		flarm_host_setup("NAVIFLARM", DeviceNVS() );
SetupNG<DeviceNVS>		flarm_host2_setup("NAVIFLDOWN", DeviceNVS() );
SetupNG<DeviceNVS>		radio_host_setup("NAVIRADIO", DeviceNVS() );
SetupNG<DeviceNVS>		krt_devsetup("KRTRADIO", DeviceNVS() );
SetupNG<DeviceNVS>		atr_devsetup("ATRIRADIO", DeviceNVS() );

template class SetupNG<DeviceNVS>;
