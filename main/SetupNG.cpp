/*
 * SetupNG.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#include "SetupNG.h"
#include "quaternion.h"
#include <string>
#include <stdio.h>
#include "esp_system.h"
#include <esp_log.h>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ESP32NVS.h"
#include "esp32/rom/uart.h"
#include <iostream>
#include <map>
#include <math.h>
#include "ESPAudio.h"
#include "BMPVario.h"
#include <esp32/rom/miniz.h>
#include "Polars.h"
#include "esp_task_wdt.h"
#include <logdef.h>
#include "mpu/types.hpp"  // MPU data types and definitions
#include "sensor.h"
#include "Router.h"
#include "Switch.h"
#include "CircleWind.h"
#include "Protocols.h"
#include "ESPAudio.h"
#include "Flap.h"
#include "DallasRmt.h"
#include <esp_http_server.h>
#include "WifiApp.h"

void change_mc() {
	Speed2Fly.change_mc();
}

void change_ballast() {
	Speed2Fly.change_ballast();
}

void change_crew_weight(){
	if( Protocols::getXcvProtocolVersion() > 1 ){
		Protocols::sendNmeaXCVCmd( "crew-weight", crew_weight.get() );
	}
	change_ballast();
}

void change_empty_weight(){
	ESP_LOGI(FNAME,"change_empty_weight, pv: %d", Protocols::getXcvProtocolVersion()  );
	if( Protocols::getXcvProtocolVersion() > 1 ){
		Protocols::sendNmeaXCVCmd( "empty-weight", empty_weight.get() );
	}
	change_ballast();
}

void change_bal_water(){
	if( Protocols::getXcvProtocolVersion() > 1 ){
		Protocols::sendNmeaXCVCmd( "bal-water", ballast_kg.get() );
	}
	change_ballast();
}

void polar_set(){
	Speed2Fly.setPolar();
}

void polar_update_name(){
	Polars::begin();
	polar_set();
}

void modifyPolar() {
	Speed2Fly.modifyPolar();
}

void resetSWindAge() {
	if( swind_dir.get() != 0 && swind_speed.get() != 0 )  // do not reset age at initial sync
		StraightWind::resetAge();
}
void resetCWindAge() {
	if( cwind_dir.get() != 0 && cwind_speed.get() != 0 )
		CircleWind::resetAge();
}

float last_volume=0;   // is this used?

void change_volume() {
	float vol = audio_volume.get();
	Audio::setVolume( vol );
	ESP_LOGI(FNAME,"change_volume -> %f", vol );
}

void change_max_volume() {
	float max = max_volume.get();
	if ( audio_volume.get() > max ) {   // enforce max_volume
		audio_volume.set( max );
		ESP_LOGI(FNAME,"change volume -> %f to fit max", max );
	}
	update_volume_menu_max();   // make volume menu use the new max
}

void flap_act() {
    if (flap_enable.get()) {
        Flap::init(MYUCG);
    }
    else if ( FLAP ) {
        delete FLAP;
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


SetupNG<float>          MC(  "MacCready", 0.5, true, SYNC_BIDIR, PERSISTENT, change_mc, UNIT_VARIO );
SetupNG<float>  		QNH( "QNH", 1013.25, true, SYNC_BIDIR, PERSISTENT, 0, UNIT_QNH );
SetupNG<float> 			polar_wingload( "POLAR_WINGLOAD", 34.40, true, SYNC_FROM_MASTER, PERSISTENT, change_ballast );
SetupNG<float> 			polar_speed1( "POLAR_SPEED1",   80, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar );
SetupNG<float> 			polar_sink1( "POLAR_SINK1",    -0.66, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar );
SetupNG<float> 			polar_speed2( "POLAR_SPEED2",   125, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar );
SetupNG<float> 			polar_sink2( "POLAR_SINK2",    -0.97, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar );
SetupNG<float> 			polar_speed3( "POLAR_SPEED3",   175, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar );
SetupNG<float> 			polar_sink3( "POLAR_SINK3",    -2.24, true, SYNC_FROM_MASTER, PERSISTENT, modifyPolar );
SetupNG<float> 			polar_max_ballast( "POLAR_MAX_BAL",  160, true, SYNC_FROM_MASTER, PERSISTENT, change_ballast );
SetupNG<float> 			polar_wingarea( "POLAR_WINGAREA", 10.5, true, SYNC_FROM_MASTER, PERSISTENT, change_ballast );

SetupNG<float>  		speedcal( "SPEEDCAL", 0.0, true, SYNC_FROM_MASTER );
SetupNG<float>  		vario_delay( "VARIO_DELAY", 3.0 );
SetupNG<float>  		vario_av_delay( "VARIO_AV_DELAY", 20.0 );  // changed to 20 seconds (quasi standard) what equals to a half circle
SetupNG<float>  		center_freq( "AUDIO_CENTER_F", 500.0, true, SYNC_FROM_MASTER  );
SetupNG<float>  		tone_var( "OCTAVES", 2.0, true, SYNC_FROM_MASTER );
SetupNG<int>  			dual_tone( "DUAL_TONE", 0, true, SYNC_FROM_MASTER  );
SetupNG<float>  		high_tone_var( "HIGH_TONE_VAR", 12.0, true, SYNC_FROM_MASTER  );
SetupNG<float>  		deadband( "DEADBAND", 0.3, true, SYNC_FROM_MASTER, PERSISTENT, 0, UNIT_VARIO  );
SetupNG<float>  		deadband_neg("DEADBAND_NEG" , -0.3, true, SYNC_FROM_MASTER, PERSISTENT, 0, UNIT_VARIO );
SetupNG<float>  		range( "VARIO_RANGE", 5.0, true, SYNC_FROM_MASTER, PERSISTENT, 0, UNIT_VARIO );
SetupNG<int>			log_scale( "LOG_SCALE", 0 );
SetupNG<float>  		ballast( "BALLAST" , 0.0, true, SYNC_NONE, VOLATILE, 0 );  // ballast increase from reference weight in %
SetupNG<float>  		ballast_kg( "BAL_KG" , 0.0, true, SYNC_BIDIR, PERSISTENT, change_bal_water );
SetupNG<float>			empty_weight( "EMPTY_WGT", 250, true, SYNC_BIDIR, PERSISTENT, change_empty_weight );
SetupNG<float>			crew_weight( "CREW_WGT", 80, true, SYNC_BIDIR, PERSISTENT, change_crew_weight );
SetupNG<float>			gross_weight( "CREW_WGT", 350, true, SYNC_NONE, VOLATILE ); // derived from above
SetupNG<float>  		bugs( "BUGS", 0.0, true, SYNC_BIDIR, VOLATILE, modifyPolar  );

SetupNG<int>  			cruise_mode( "CRUISE", 0, RST_NONE, SYNC_BIDIR, VOLATILE );
SetupNG<float>  		OAT( "OAT", DEVICE_DISCONNECTED_C, true, SYNC_FROM_MASTER, VOLATILE );   // outside temperature
SetupNG<float>  		swind_dir( "SWDD", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetSWindAge );
SetupNG<float>  		swind_speed( "SWDS", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetSWindAge );
SetupNG<float>  		swind_sideslip_lim( "SWSL", 2.0, true, SYNC_FROM_MASTER );
SetupNG<float>  		cwind_dir( "CWDD", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetCWindAge );
SetupNG<float>  		cwind_speed( "CWDS", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetCWindAge );
SetupNG<float>  		mag_hdm( "HDM", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		mag_hdt( "HDT", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		average_climb( "AVCL", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		flap_pos( "FLPS", -100.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		altitude( "ALTI", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		ias( "IASV", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		te_vario( "TEVA", 0.0, true, SYNC_FROM_MASTER, VOLATILE );

SetupNG<float>  		s2f_speed( "S2F_SPEED", 100.0, true, SYNC_FROM_MASTER, PERSISTENT, 0, UNIT_SPEED );
SetupNG<float>  		s2f_hysteresis( "S2F_HYST", 5.0, true, SYNC_FROM_MASTER, PERSISTENT, 0, UNIT_SPEED  );
SetupNG<float>  		s2f_flap_pos( "S2F_FLAP", 1, true, SYNC_FROM_MASTER, PERSISTENT, 0 );
SetupNG<float>  		s2f_gyro_deg( "S2F_GYRO", 12, true, SYNC_FROM_MASTER, PERSISTENT, 0 );

SetupNG<float> 			audio_volume("AUD_VOL", 10, true, SYNC_BIDIR, VOLATILE, change_volume );
SetupNG<int>  			audio_split_vol( "AUD_SPLIT", 0 );
SetupNG<int>  			audio_variable_frequency( "AUD_VAFQ", 0);
SetupNG<int>  			s2f_switch_mode( "AUDIO_MODE" ,  3 );
SetupNG<int>  			chopping_mode( "CHOPPING_MODE",  VARIO_CHOP );
SetupNG<int>  			chopping_style( "CHOP_STYLE",  AUDIO_CHOP_SOFT );
SetupNG<int>  			amplifier_shutdown( "AMP_DIS", AMP_STAY_ON );
SetupNG<int>            audio_equalizer( "AUD_EQ", AUDIO_EQ_DISABLE, false );

SetupNG<int>  			wireless_type( "BT_ENABLE" ,  WL_BLUETOOTH );
SetupNG<float>  		wifi_max_power( "WIFI_MP" ,  50);
SetupNG<int>  			factory_reset( "FACTORY_RES" , 0 );
SetupNG<int>  			audio_range( "AUDIO_RANGE" , AUDIO_RANGE_5_MS );
SetupNG<int>  			alt_select( "ALT_SELECT" , AS_BARO_SENSOR );
SetupNG<int>  			fl_auto_transition( "FL_AUTO" , 0 );
SetupNG<int>  			alt_display_mode( "ALT_DISP_MODE" , MODE_QNH );
SetupNG<float>  		transition_alt( "TRANS_ALT", 50 );   // Transition Altitude
SetupNG<int>  			glider_type( "GLIDER_TYPE", 0, true, SYNC_FROM_MASTER, PERSISTENT,  polar_set );
SetupNG<int>  			glider_type_index( "GLIDER_TYPE_IDX", 0, true, SYNC_FROM_MASTER, PERSISTENT, polar_update_name );
SetupNG<int>  			ps_display( "PS_DISPLAY", 1 );

SetupNG<float>  		as_offset( "AS_OFFSET" , -1 );
SetupNG<float>  		bat_low_volt( "BAT_LOW_VOLT" , 11.5, true, SYNC_FROM_MASTER );
SetupNG<float>  		bat_red_volt( "BAT_RED_VOLT", 11.75, true, SYNC_FROM_MASTER  );
SetupNG<float>  		bat_yellow_volt( "BAT_YELLOW_VOLT" , 12.0, true, SYNC_FROM_MASTER  );
SetupNG<float>  		bat_full_volt( "BAT_FULL_VOLT", 12.8, true, SYNC_FROM_MASTER  );
SetupNG<float>  		core_climb_period( "CORE_CLIMB_P" , 60, true, SYNC_FROM_MASTER  );
SetupNG<float>  		core_climb_min( "CORE_CLIMB_MIN" , 0.5, true, SYNC_FROM_MASTER  );
SetupNG<float>  		core_climb_history( "CORE_CLIMB_HIST" , 45, true, SYNC_FROM_MASTER  );
SetupNG<float>  		mean_climb_major_change( "MEAN_CLMC", 0.5, true, SYNC_FROM_MASTER );
SetupNG<float>  		elevation( "ELEVATION", -1, true, SYNC_BIDIR, PERSISTENT, 0, UNIT_ALT );
SetupNG<float>  		default_volume( "DEFAULT_VOL", 25.0 );
SetupNG<float>  		max_volume( "MAXI_VOL", 60.0, true, SYNC_NONE, PERSISTENT, change_max_volume );
SetupNG<float>  		frequency_response( "FREQ_RES", 30.0 );
SetupNG<float>  		s2f_deadband( "DEADBAND_S2F", 10.0, true, SYNC_BIDIR, PERSISTENT, 0, UNIT_SPEED );
SetupNG<float>  		s2f_deadband_neg( "DB_S2F_NEG", -10.0, true, SYNC_BIDIR, PERSISTENT, 0, UNIT_SPEED );
SetupNG<float>  		s2f_delay( "S2F_DELAY", 5.0, true, SYNC_BIDIR );
SetupNG<float>  		factory_volt_adjust("FACT_VOLT_ADJ" , 0.00815, RST_NONE );

SetupNG<int>  			display_type( "DISPLAY_TYPE",  UNIVERSAL );
SetupNG<int>  			display_test( "DISPLAY_TEST", 0, RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int>  			display_orientation("DISPLAY_ORIENT" , DISPLAY_NORMAL, true, SYNC_NONE, PERSISTENT, chg_display_orientation );
SetupNG<int>  			flap_enable( "FLAP_ENABLE", 0, true, SYNC_FROM_MASTER, PERSISTENT, flap_act);
SetupNG<float>  		flap_minus_3( "FLAP_MINUS_3", 200,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act, UNIT_SPEED );
SetupNG<float>  		flap_minus_2( "FLAP_MINUS_2", 165,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act, UNIT_SPEED );
SetupNG<float>  		flap_minus_1( "FLAP_MINUS_1", 105,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act, UNIT_SPEED );
SetupNG<float>  		flap_0(       "FLAP_0", 88,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act, UNIT_SPEED );
SetupNG<float>  		flap_plus_1(  "FLAP_PLUS_1", 78,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act, UNIT_SPEED );
SetupNG<float>  		flap_plus_2(  "FLAP_PLUS_2", 70,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act, UNIT_SPEED );
SetupNG<int>  			alt_unit( "ALT_UNIT", ALT_UNIT_METER );
SetupNG<int>  			alt_quantization( "ALT_QUANT", ALT_QUANT_10 );
SetupNG<int>  			ias_unit( "IAS_UNIT", SPEED_UNIT_KMH );
SetupNG<int>  			vario_unit( "VARIO_UNIT", VARIO_UNIT_MS );
SetupNG<int>  			temperature_unit( "TEMP_UNIT", T_CELCIUS );
SetupNG<int>  			dst_unit( "DST_UNIT", DST_UNIT_M );
SetupNG<int>  			qnh_unit("QNH_UNIT", QNH_HPA );
SetupNG<int>  			rot_default( "ROTARY_DEFAULT", 0 );
SetupNG<int>  			serial1_speed( "SERIAL2_SPEED", 3 );   // tag will stay SERIAL2 from historical reason
SetupNG<int>            serial1_protocol("SERIAL2_PRT", DEV_FLARM );
SetupNG<int>  			serial1_pins_twisted( "SERIAL2_PINS", 0 );
SetupNG<int>  			serial1_rxloop( "SERIAL2_RXLOOP", 0 );
SetupNG<int>  			serial1_tx( "SERIAL2_TX", (1UL << RT_XCVARIO) | (1UL << RT_WIRELESS) );   //  Default Wireless and local XCVario for Flarm Warnings, bincom
SetupNG<int>  			rt_s1_xcv( "S2_TX_XCV", 1, RST_NONE, SYNC_NONE, VOLATILE  );
SetupNG<int>  			rt_s1_wl( "S2_TX_WL", 1, RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int>  			rt_s1_s2( "S2_TX_S2", 0, RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int>  			rt_s1_can( "S2_TX_CAN", 0, RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int>  			serial1_tx_inverted( "SERIAL2_TX_INV", RS232_INVERTED );
SetupNG<int>  			serial1_rx_inverted( "SERIAL2_RX_INV", RS232_INVERTED );
SetupNG<int>  			serial1_tx_enable( "SER1_TX_ENA", 1 );
SetupNG<int>  			serial2_speed( "SERIAL1_SPEED", 3 );
SetupNG<int>            serial2_protocol("SERIAL1_PRT", DEV_KRT2_RADIO );
SetupNG<int>  			serial2_pins_twisted( "SERIAL1_PINS", 0 );
SetupNG<int>  			serial2_tx( "SERIAL1_TX", (1UL << RT_XCVARIO) | (1UL << RT_WIRELESS) );     //  BT device and XCVario, Serial2 is foreseen for Protocols or Kobo
SetupNG<int>  			rt_s2_xcv( "S1_TX_XCV", 1, RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int>  			rt_s2_wl( "S1_TX_WL", 0, RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int>  			rt_s2_can( "S1_TX_CAN", 0,RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int>  			serial2_tx_inverted( "SERIAL1_TX_INV", RS232_INVERTED );
SetupNG<int>  			serial2_rx_inverted( "SERIAL1_RX_INV", RS232_INVERTED );
SetupNG<int>  			serial2_tx_enable( "SER2_TX_ENA", 1 );
SetupNG<int>  			software_update( "SOFTWARE_UPDATE", 0 );
SetupNG<int>  			battery_display( "BAT_DISPLAY", 0 );
SetupNG<int>  			airspeed_mode( "AIRSPEED_MODE", MODE_IAS );
SetupNG<int>  			nmea_protocol( "NMEA_PROTOCOL", XCVARIO );
SetupNG<int>		    log_level( "LOG_LEVEL", 3 );
SetupNG<float>		    audio_factor( "AUDIO_FACTOR", 1 );
SetupNG<float>		    te_comp_adjust ( "TECOMP_ADJ", 0);
SetupNG<int>		    te_comp_enable( "TECOMP_ENA", TE_TEK_PROBE );
SetupNG<int>		    rotary_dir( "ROTARY_DIR", 0 );
SetupNG<int>		    rotary_inc( "ROTARY_INC", 1 );
SetupNG<int>		    rotary_dir_21( "ROTARY_DIR_21", 1 );
SetupNG<int>		    student_mode( "STUD_MOD", 0 );
SetupNG<float>		    password( "PASSWORD", 0 );
SetupNG<int>		    autozero( "AUTOZERO", 0 );
SetupNG<int>		    attitude_indicator("AHRS", 1 );
SetupNG<int>		    ahrs_rpyl_dataset("RPYL", 0 );
SetupNG<int>		    ahrs_autozero("AHRSAZ", 0 );
SetupNG<float>		    ahrs_gyro_factor("AHRSMGYF", 100 );
SetupNG<float>		    ahrs_min_gyro_factor("AHRSLGYF", 20 );
SetupNG<float>		    ahrs_dynamic_factor("AHRSGDYN", 5 );
SetupNG<int>		    ahrs_roll_check("AHRSRCHECK", 0 );
SetupNG<float>       	gyro_gating("GYRO_GAT", 1.0 );
SetupNG<int>		    display_style("DISPLAY_STYLE", 1 );
SetupNG<int>		    s2f_switch_type("S2FHWSW", S2F_HW_SWITCH );
SetupNG<int>		    hardwareRevision("HWREV", HW_UNKNOWN );
SetupNG<int>		    ahrs_licence_dig1("AHRS_LIC_1_2", 0 );
SetupNG<int>		    ahrs_licence_dig2("AHRS_LIC_2", 0 );
SetupNG<int>		    ahrs_licence_dig3("AHRS_LIC_3", 0 );
SetupNG<int>		    ahrs_licence_dig4("AHRS_LIC_4", 0 );
SetupNG<int>		    dummy("DUMMY", 0 );
SetupNG<int>		    wk_sens_pos_plus_3("WKSP3", 1000);
SetupNG<int>		    wk_sens_pos_plus_2("WKSP2", 1500);
SetupNG<int>		    wk_sens_pos_plus_1("WKSP1", 2000);
SetupNG<int>		    wk_sens_pos_0("WKSP0", 2500);
SetupNG<int>		    wk_sens_pos_minus_1("WKSM1", 3000);
SetupNG<int>		    wk_sens_pos_minus_2("WKSM2", 3500);
SetupNG<int>		    wk_sens_pos_minus_3("WKSM3", 4000);
SetupNG<int>            stall_warning( "STALL_WARN", 0 );
SetupNG<float>			stall_speed( "STALL_SPEED", 70, true, SYNC_FROM_MASTER, PERSISTENT, 0, UNIT_SPEED  );
SetupNG<int>            flarm_warning( "FLARM_LEVEL", 1 );
SetupNG<float>          flarm_volume( "FLARM_VOL", 100 );
SetupNG<float>          flarm_alarm_time( "FLARM_ALM", 5 );
SetupNG<int>            flap_sensor( "FLAP_SENS", 0, RST_NONE, SYNC_FROM_MASTER, PERSISTENT, flap_act);
SetupNG<float>          flap_pos_max("FL_POS_M", +2, true, SYNC_FROM_MASTER, PERSISTENT, flap_act);
SetupNG<float>          flap_neg_max("FL_NEG_M", -2, true, SYNC_FROM_MASTER, PERSISTENT, flap_act);
SetupNG<int>            compass_enable( "CP_ENABLE", 0 );
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
SetupNG<float>          compass_declination( "CP_DECL", 0 );
SetupNG<int>            compass_declination_valid( "CP_DECL_VALID", 0 );
SetupNG<float>          compass_damping( "CPS_DAMP", 1.0 );
SetupNG<int>            compass_nmea_hdm( "CP_NMEA_HDM", 0 );
SetupNG<int>            compass_nmea_hdt( "CP_NMEA_HDT", 0 );
SetupNG<float>          compass_i2c_cl("CP_I2C_CL", 100 );
SetupNG<float>          wind_as_filter( "WINDASF", 0.02 );
SetupNG<float>          wind_gps_lowpass( "WINDGPSLP", 1.00 );
SetupNG<float>          wind_dev_filter( "WINDDEVF", 0.010 );
SetupNG<int> 			wind_enable( "WIND_ENA", WA_OFF );
SetupNG<int> 			wind_logging( "WIND_LOG", 0 );
SetupNG<float> 			wind_as_calibration("WIND_AS_CAL", 1.0 );
SetupNG<float> 			wind_filter_lowpass("SWINDAVER", 60 );
SetupNG<float> 			wind_straight_course_tolerance("WINDSTOL", 7.5 );
SetupNG<float> 			wind_straight_speed_tolerance("WINDSSTOL", 15 );
SetupNG<int> 			wind_display( "WIND_DIS", WD_NONE );
SetupNG<int> 			wind_reference( "WIND_REF", WR_HEADING );
SetupNG<float> 			wind_max_deviation("WIND_MDEV", 30.0 );
SetupNG<float> 			wind_as_min( "WIND_ASM", 25 );
SetupNG<int> 			s2f_blockspeed( "S2G_BLOCKSPEED", 0, true, SYNC_BIDIR );  // considering netto vario and g load for S2F or not
SetupNG<int> 			needle_color("NEEDLE_COLOR", VN_COLOR_ORANGE );
SetupNG<int>			s2f_arrow_color("S2F_ARRCOL", AC_WHITE_WHITE );
SetupNG<int> 			wk_label_plus_3( "WKLP3", 41,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act);  //  L
SetupNG<int> 			wk_label_plus_2( "WKLP2", 11,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act);  //  2
SetupNG<int> 			wk_label_plus_1( "WKLP1", 10,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act);  //  1
SetupNG<int> 			wk_label_null_0( "WKL0",   9,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act);  //  0
SetupNG<int> 			wk_label_minus_1( "WKLM1", 8,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act);  // -1
SetupNG<int> 			wk_label_minus_2( "WKLM2", 7,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act);  // -2
SetupNG<int> 			wk_label_minus_3( "WKLM3", 42,  true, SYNC_FROM_MASTER, PERSISTENT, flap_act); //  S
SetupNG<float>       	flap_takeoff("FLAPTO", 1,  true, SYNC_FROM_MASTER);
SetupNG<int> 			audio_mute_menu( "AUDIS", 0 );
SetupNG<int> 			audio_mute_sink( "AUDISS", 0 );
SetupNG<int> 			audio_mute_gen( "AUDISG", AUDIO_ON );
SetupNG<int>			vario_mode("VAMOD", CRUISE_NETTO );  // switch to netto mode when cruising
SetupNG<int>			airspeed_sensor_type("PTYPE", PS_NONE, RST_NONE);
SetupNG<int>			cruise_audio_mode("CAUDIO", 0 );
SetupNG<int>			netto_mode("NETMOD", NETTO_RELATIVE );  // regard polar sink
SetupNG<float>			v_max("VMAX", 270, true, SYNC_FROM_MASTER, PERSISTENT, 0, UNIT_SPEED  );
SetupNG<int>			gload_mode("GLMOD", GLOAD_OFF );
SetupNG<float>			gload_pos_thresh("GLOADPT", 4 );
SetupNG<float>			gload_neg_thresh("GLOADNT", -3 );
SetupNG<float>			gload_pos_limit_low("GLOADPLL", 3 );
SetupNG<float>			gload_neg_limit_low("GLOADNLL", -2 );
SetupNG<float>			gload_pos_limit("GLOADPL", 5 );
SetupNG<float>			gload_neg_limit("GLOADNL", -3 );
SetupNG<float>			gload_pos_max("GLOADPM", 1 );
SetupNG<float>			gload_neg_max("GLOADNM", 1 );
SetupNG<float>			airspeed_max("ASMAX", 0 );
SetupNG<float>		    gload_alarm_volume("GLOADAVOL", 100 );
SetupNG<int>        	display_variant("DISPLAY_VARIANT", 0 );
SetupNG<int>        	compass_dev_auto("COMPASS_DEV", 0 );
SetupNG<float>       	max_circle_wind_diff("CI_WINDDM", 60.0 );
SetupNG<float>       	max_circle_wind_delta_deg("CIMDELD", 20.0 );
SetupNG<float>       	max_circle_wind_delta_speed("CIMDELS", 5.0 );
SetupNG<float>       	circle_wind_lowpass("CI_WINDLOW", 5 );
SetupNG<int> 			can_speed( "CANSPEED", CAN_SPEED_OFF );
SetupNG<int> 			rt_can_xcv( "CANTX_XC", 0 );
SetupNG<int> 			rt_xcv_wl( "WLTX_XC", 1 );
SetupNG<int> 			rt_wl_can( "WLTX_CAN", 0 );
SetupNG<int> 			can_mode( "CANMOD", CAN_MODE_STANDALONE );
SetupNG<float> 			master_xcvario( "MSXCV", 0 );
SetupNG<int> 			master_xcvario_lock( "MSXCVL", 0 );
SetupNG<int> 			menu_long_press("MENU_LONG", 0 );
SetupNG<int> 			menu_screens("MENU_SCR", 0 );
SetupNG<int> 			screen_gmeter("SCR_GMET", 0, RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int> 			screen_horizon("SCR_HORIZ", 0, RST_NONE, SYNC_NONE, VOLATILE );
SetupNG<int> 			screen_centeraid("SCR_CA", 0, RST_NONE, SYNC_NONE  );
SetupNG<int> 			data_monitor_mode("DATAMONM", MON_MOD_ASCII, true, SYNC_NONE, VOLATILE  );
SetupNG<t_bitfield_compass>  calibration_bits("CALBIT", { 0,0,0,0,0,0 } );
SetupNG<int> 			gear_warning("GEARWA", 0 );
SetupNG<t_wireless_id>  custom_wireless_id("WLID", t_wireless_id("") );
SetupNG<int> 			drawing_prio("DRAWP", DP_NEEDLE );
SetupNG<int> 			logging("LOGGING", LOG_DISABLE );
SetupNG<float>      	display_clock_adj("DSCLADHJ", 0 );

static mpud::raw_axes_t zero_bias;
SetupNG<float>				glider_ground_aa("GLD_GND_AA", 12.0, true, SYNC_FROM_MASTER);
SetupNG<Quaternion>			imu_reference("IMU_REFERENCE", Quaternion(), false);
SetupNG<mpud::raw_axes_t>	gyro_bias("GYRO_BIAS", zero_bias );
SetupNG<mpud::raw_axes_t>	accl_bias("ACCL_BIAS", zero_bias );
SetupNG<float>              mpu_temperature("MPUTEMP", 45.0, true, SYNC_FROM_MASTER, PERSISTENT, chg_mpu_target );    // default for AHRS chip temperature (XCV 2023)


