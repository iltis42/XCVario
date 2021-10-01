/*
 * Setup.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#include "Setup.h"
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

std::vector<SetupCommon *> SetupCommon::entries;
char SetupCommon::_ID[14];

void change_mc_bal() {  // or bugs
	Speed2Fly.change_mc_bal();
}

void resetSWindAge() {
	if( swind_dir.get() != 0 && swind_speed.get() != 0 )  // do not reset age at initial sync
		StraightWind::resetAge();
}
void resetCWindAge() {
	if( cwind_dir.get() != 0 && cwind_speed.get() != 0 )
		CircleWind::resetAge();
}


static int last_volume=0;

void change_volume() {
	int delta = (int)audio_volume.get() - last_volume;
	if( delta != 0 ){
		if( delta > 0 ){
			Audio::incVolume(delta);
		}
		if( delta < 0 ){
			Audio::decVolume(abs(delta));
		}
		last_volume = (int)audio_volume.get();
	}
}


SetupNG<float>  		QNH( "QNH", 1013.25, true, SYNC_BIDIR );
SetupNG<float> 			polar_wingload( "POLAR_WINGLOAD", 34.40, true, SYNC_FROM_MASTER );
SetupNG<float> 			polar_speed1( "POLAR_SPEED1",   80, true, SYNC_FROM_MASTER );
SetupNG<float> 			polar_sink1( "POLAR_SINK1",    -0.66, true, SYNC_FROM_MASTER );
SetupNG<float> 			polar_speed2( "POLAR_SPEED2",   125, true, SYNC_FROM_MASTER );
SetupNG<float> 			polar_sink2( "POLAR_SINK2",    -0.97, true, SYNC_FROM_MASTER );
SetupNG<float> 			polar_speed3( "POLAR_SPEED3",   175, true, SYNC_FROM_MASTER );
SetupNG<float> 			polar_sink3( "POLAR_SINK3",    -2.24, true, SYNC_FROM_MASTER );
SetupNG<float> 			polar_max_ballast( "POLAR_MAX_BAL",  160, true, SYNC_FROM_MASTER );
SetupNG<float> 			polar_wingarea( "POLAR_WINGAREA", 10.5, true, SYNC_FROM_MASTER );

SetupNG<float>  		speedcal( "SPEEDCAL", 0.0 );
SetupNG<float>  		vario_delay( "VARIO_DELAY", 3.0 );
SetupNG<float>  		vario_av_delay( "VARIO_AV_DELAY", 20.0 );  // changed to 20 seconds (quasi standard) what equals to a half circle
SetupNG<float>  		center_freq( "AUDIO_CENTER_F", 500.0 );
SetupNG<float>  		tone_var( "OCTAVES", 2.0);
SetupNG<int>  			dual_tone( "DUAL_TONE", 0 );
SetupNG<float>  		high_tone_var( "HIGH_TONE_VAR", 12.0 );
SetupNG<float>  		deadband( "DEADBAND", 0.3 );
SetupNG<float>  		deadband_neg("DEADBAND_NEG" , -0.3 );
SetupNG<float>  		range( "VARIO_RANGE", 5.0 );
SetupNG<float>  		ballast( "BALLAST" , 0.0, true, SYNC_BIDIR, PERSISTENT, change_mc_bal );
SetupNG<float>  		bugs( "BUGS", 0.0, true, SYNC_BIDIR, PERSISTENT, change_mc_bal  );
SetupNG<float>  		MC( "MacCready", 0.5, true, SYNC_BIDIR, PERSISTENT, change_mc_bal );
SetupNG<int>  			cruise_mode( "CRUISE", 0, true, SYNC_BIDIR, VOLATILE );
SetupNG<float>  		OAT( "OAT", 0.0, true, SYNC_FROM_MASTER, VOLATILE );   // outside temperature
SetupNG<float>  		swind_dir( "SWDD", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetSWindAge );
SetupNG<float>  		swind_speed( "SWDS", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetSWindAge );
SetupNG<float>  		cwind_dir( "CWDD", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetCWindAge );
SetupNG<float>  		cwind_speed( "CWDS", 0.0, true, SYNC_FROM_MASTER, VOLATILE, resetCWindAge );
SetupNG<float>  		mag_hdm( "HDM", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		mag_hdt( "HDT", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		average_climb( "AVCL", 0.0, true, SYNC_FROM_MASTER, VOLATILE );
SetupNG<float>  		flap_pos( "FLPS", 0.0, true, SYNC_FROM_MASTER, VOLATILE );

SetupNG<float>  		s2f_speed( "S2F_SPEED", 100.0 );
SetupNG<float>  		s2f_hysteresis( "S2F_HYST", 5.0 );

SetupNG<float>  		audio_volume( "AUD_VOL", 10, true, SYNC_BIDIR, VOLATILE, change_volume );
SetupNG<int>  			audio_variable_frequency( "AUD_VAFQ", 0);
SetupNG<int>  			audio_mode( "AUDIO_MODE" ,  3 );
SetupNG<int>  			chopping_mode( "CHOPPING_MODE",  VARIO_CHOP );
SetupNG<int>  			chopping_style( "CHOP_STYLE",  AUDIO_CHOP_SOFT );
SetupNG<int>  			amplifier_shutdown( "AMP_DIS", 0 );

SetupNG<int>  			wireless_type( "BT_ENABLE" ,  WL_BLUETOOTH );
SetupNG<float>  		wifi_max_power( "WIFI_MP" ,  50);
SetupNG<int>  			factory_reset( "FACTORY_RES" , 0 );
SetupNG<int>  			audio_range( "AUDIO_RANGE" , AUDIO_RANGE_5_MS );
SetupNG<int>  			alt_select( "ALT_SELECT" , AS_BARO_SENSOR );
SetupNG<int>  			fl_auto_transition( "FL_AUTO" , 0 );
SetupNG<int>  			alt_display_mode( "ALT_DISP_MODE" , MODE_QNH );
SetupNG<float>  		transition_alt( "TRANS_ALT", 50 );   // Transition Altitude
SetupNG<int>  			glider_type( "GLIDER_TYPE", 0, true, SYNC_FROM_MASTER );
SetupNG<int>  			glider_type_index( "GLIDER_TYPE_IDX", 0, true, SYNC_FROM_MASTER );
SetupNG<int>  			ps_display( "PS_DISPLAY", 1 );

SetupNG<float>  		as_offset( "AS_OFFSET" , -1 );
SetupNG<float>  		bat_low_volt( "BAT_LOW_VOLT" , 11.5 );
SetupNG<float>  		bat_red_volt( "BAT_RED_VOLT", 11.75 );
SetupNG<float>  		bat_yellow_volt( "BAT_YELLOW_VOLT" , 12.0 );
SetupNG<float>  		bat_full_volt( "BAT_FULL_VOLT", 12.8 );
SetupNG<float>  		core_climb_period( "CORE_CLIMB_P" , 60 );
SetupNG<float>  		core_climb_min( "CORE_CLIMB_MIN" , 0.5 );
SetupNG<float>  		core_climb_history( "CORE_CLIMB_HIST" , 45 );
SetupNG<float>  		elevation( "ELEVATION", -1 );
SetupNG<float>  		default_volume( "DEFAULT_VOL", 10.0 );
SetupNG<float>  		max_volume( "MAXI_VOL", 60.0 );
SetupNG<float>  		frequency_response( "FREQ_RES", 30.0 );
SetupNG<float>  		s2f_deadband( "DEADBAND_S2F", 10.0 );
SetupNG<float>  		s2f_deadband_neg( "DB_S2F_NEG", -10.0 );
SetupNG<float>  		s2f_delay( "S2F_DELAY", 5.0 );
SetupNG<float>  		factory_volt_adjust("FACT_VOLT_ADJ" , 0.00815, false );

SetupNG<int>  			display_type( "DISPLAY_TYPE",  UNIVERSAL );
SetupNG<int>  			display_orientation("DISPLAY_ORIENT" , 0 );
SetupNG<int>  			flap_enable( "FLAP_ENABLE", 0, true, SYNC_FROM_MASTER );
SetupNG<float>  		flap_minus_3( "FLAP_MINUS_3", 200,  true, SYNC_FROM_MASTER );
SetupNG<float>  		flap_minus_2( "FLAP_MINUS_2", 165,  true, SYNC_FROM_MASTER );
SetupNG<float>  		flap_minus_1( "FLAP_MINUS_1", 105,  true, SYNC_FROM_MASTER );
SetupNG<float>  		flap_0( "FLAP_0", 88,  true, SYNC_FROM_MASTER );
SetupNG<float>  		flap_plus_1( "FLAP_PLUS_1", 78,  true, SYNC_FROM_MASTER);
SetupNG<float>  		flap_plus_2( "FLAP_PLUS_2", 70,  true, SYNC_FROM_MASTER );
SetupNG<int>  			alt_unit( "ALT_UNIT", 0 );
SetupNG<int>  			ias_unit( "IAS_UNIT", 0 );
SetupNG<int>  			vario_unit( "VARIO_UNIT", 0 );
SetupNG<int>  			temperature_unit( "TEMP_UNIT", T_CELCIUS );
SetupNG<int>  			rot_default( "ROTARY_DEFAULT", 0 );
SetupNG<int>  			serial1_speed( "SERIAL2_SPEED", 3 );   // tag will stay SERIAL2 from historical reason
SetupNG<int>  			serial1_pins_twisted( "SERIAL2_PINS", 0 );
SetupNG<int>  			serial1_rxloop( "SERIAL2_RXLOOP", 0 );
SetupNG<int>  			serial1_tx( "SERIAL2_TX", 2 );      // Just BT device = XCSoar to control FLARM
SetupNG<int>  			serial1_tx_inverted( "SERIAL2_TX_INV", RS232_INVERTED );
SetupNG<int>  			serial1_rx_inverted( "SERIAL2_RX_INV", RS232_INVERTED );
SetupNG<int>  			serial1_tx_enable( "SER1_TX_ENA", RT_XCVARIO );
SetupNG<int>  			serial2_speed( "SERIAL1_SPEED", 3 );
SetupNG<int>  			serial2_pins_twisted( "SERIAL1_PINS", 0 );
SetupNG<int>  			serial2_tx( "SERIAL1_TX", 1 );     //  BT device and XCVario, Serial2 is foreseen for Protocols or Kobo
SetupNG<int>  			serial2_tx_inverted( "SERIAL1_TX_INV", RS232_INVERTED );
SetupNG<int>  			serial2_rx_inverted( "SERIAL1_RX_INV", RS232_INVERTED );
SetupNG<int>  			serial2_tx_enable( "SER2_TX_ENA", RT_XCVARIO );
SetupNG<int>  			software_update( "SOFTWARE_UPDATE", 0 );
SetupNG<int>  			battery_display( "BAT_DISPLAY", 0 );
SetupNG<int>  			airspeed_mode( "AIRSPEED_MODE", MODE_IAS );
SetupNG<int>  			nmea_protocol( "NMEA_PROTOCOL", XCVARIO );
SetupNG<int>		    log_level( "LOG_LEVEL", 3 );
SetupNG<float>		    audio_factor( "AUDIO_FACTOR", 1 );
SetupNG<float>		    te_comp_adjust ( "TECOMP_ADJ", 0);
SetupNG<int>		    te_comp_enable( "TECOMP_ENA", 0 );
SetupNG<int>		    rotary_dir( "ROTARY_DIR", 0 );
SetupNG<int>		    rotary_inc( "ROTARY_INC", 1 );
SetupNG<int>		    rotary_dir_21( "ROTARY_DIR_21", 1 );
SetupNG<int>		    student_mode( "STUD_MOD", 0 );
SetupNG<float>		    password( "PASSWORD", 0 );
SetupNG<int>		    autozero( "AUTOZERO", 0 );
SetupNG<int>		    attitude_indicator("AHRS", 1 );
SetupNG<int>		    ahrs_autozero("AHRSAZ", 0 );
SetupNG<float>		    ahrs_gyro_factor("AHRSGF", 90 );
SetupNG<int>		    display_style("DISPLAY_STYLE", 1 );
SetupNG<int>		    s2f_switch_type("S2FHWSW", S2F_HW_SWITCH );
SetupNG<int>		    hardwareRevision("HWREV", HW_UNKNOWN );
SetupNG<int>		    ahrs_licence_dig1("AHRS_LIC_1_2", 0, false );
SetupNG<int>		    ahrs_licence_dig2("AHRS_LIC_2", 0, false );
SetupNG<int>		    ahrs_licence_dig3("AHRS_LIC_3", 0, false );
SetupNG<int>		    ahrs_licence_dig4("AHRS_LIC_4", 0, false );
SetupNG<int>		    dummy("DUMMY", 0, false );
SetupNG<int>		    wk_sens_pos_plus_3("WKSP3", 1000);
SetupNG<int>		    wk_sens_pos_plus_2("WKSP2", 1500);
SetupNG<int>		    wk_sens_pos_plus_1("WKSP1", 2000);
SetupNG<int>		    wk_sens_pos_0("WKSP0", 2500);
SetupNG<int>		    wk_sens_pos_minus_1("WKSM1", 3000);
SetupNG<int>		    wk_sens_pos_minus_2("WKSM2", 3500);
SetupNG<int>		    wk_sens_pos_minus_3("WKSM3", 4000);
SetupNG<int>            stall_warning( "STALL_WARN", 0 );
SetupNG<float>			stall_speed( "STALL_SPEED", 70 );
SetupNG<int>            flarm_warning( "FLARM_LEVEL", 1 );
SetupNG<int>            flarm_sim( "FLARM_SIM", 0 );
SetupNG<float>          flarm_volume( "FLARM_VOL", 100 );
SetupNG<int>            flap_sensor( "FLAP_SENS", 0, false, SYNC_FROM_MASTER);
SetupNG<float>          flap_pos_max("FL_POS_M", +2,  true, SYNC_FROM_MASTER);
SetupNG<float>          flap_neg_max("FL_NEG_M", -2,  true, SYNC_FROM_MASTER);
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
SetupNG<float>          compass_x_scale( "CP_X_SCALE", 0 );
SetupNG<float>          compass_y_scale( "CP_Y_SCALE", 0 );
SetupNG<float>          compass_z_scale( "CP_Z_SCALE", 0 );
SetupNG<int>            compass_calibrated( "CP_CALIBRATED", 0 );
SetupNG<float>          compass_declination( "CP_DECL", 0 );
SetupNG<int>            compass_declination_valid( "CP_DECL_VALID", 0 );
SetupNG<float>          compass_damping( "CPS_DAMP", 5.0 );
SetupNG<int>            compass_nmea_hdm( "CP_NMEA_HDM", 0 );
SetupNG<int>            compass_nmea_hdt( "CP_NMEA_HDT", 0 );
SetupNG<float>          compass_i2c_cl("CP_I2C_CL", 100 );
SetupNG<float>          wind_as_filter( "WINDASF", 0.02 );
SetupNG<float>          wind_gps_lowpass( "WINDGPSLP", 1.00 );
SetupNG<float>          wind_dev_filter( "WINDDEVF", 0.02 );
SetupNG<int> 			wind_enable( "WIND_ENA", WA_OFF );
SetupNG<int> 			wind_logging( "WIND_LOG", 0 );
SetupNG<float> 			wind_as_calibration("WIND_AS_CAL", 1.0 );
SetupNG<float> 			wind_filter_lowpass("SWINDAVER", 20 );
SetupNG<int> 			wind_display( "WIND_DIS", WD_NONE );
SetupNG<int> 			wind_reference( "WIND_REF", WR_HEADING );
SetupNG<float> 			wind_max_deviation("WIND_MDEV", 30.0 );
SetupNG<float> 			wind_as_min( "WIND_ASM", 25 );
SetupNG<int> 			s2f_with_gload( "S2G_GLOAD", 1 );       // considering g load in S2F
SetupNG<int> 			s2f_blockspeed( "S2G_BLOCKSPEED", 0 );  // considering netto vario and g load for S2F or not
SetupNG<int> 			wk_label_plus_3( "WKLP3", 32,  true, SYNC_FROM_MASTER );  //  L
SetupNG<int> 			wk_label_plus_2( "WKLP2", 11,  true, SYNC_FROM_MASTER );  //  2
SetupNG<int> 			wk_label_plus_1( "WKLP1", 10,  true, SYNC_FROM_MASTER );  //  1
SetupNG<int> 			wk_label_null_0( "WKL0",   9,  true, SYNC_FROM_MASTER );  //  0
SetupNG<int> 			wk_label_minus_1( "WKLM1", 8,  true, SYNC_FROM_MASTER );  // -1
SetupNG<int> 			wk_label_minus_2( "WKLM2", 7,  true, SYNC_FROM_MASTER );  // -2
SetupNG<int> 			wk_label_minus_3( "WKLM3", 33,  true, SYNC_FROM_MASTER ); //  S
SetupNG<float>       	flap_takeoff("FLAPTO", 1,  true, SYNC_FROM_MASTER );
SetupNG<int> 			audio_disable( "AUDIS", 0 );
SetupNG<int>			vario_mode("VAMOD", CRUISE_NETTO );  // switch to netto mode when cruising
SetupNG<int>			airspeed_sensor_type("PTYPE", PS_NONE, false);
SetupNG<int>			cruise_audio_mode("CAUDIO", 0 );
SetupNG<int>			netto_mode("NETMOD", NETTO_RELATIVE );  // regard polar sink
SetupNG<float>			v_max("VMAX", 270 );
SetupNG<int>			gload_mode("GLMOD", GLOAD_OFF );
SetupNG<float>			gload_pos_thresh("GLOADPT", 4 );
SetupNG<float>			gload_neg_thresh("GLOADNT", -2 );
SetupNG<float>			gload_pos_limit("GLOADPL", 5 );
SetupNG<float>			gload_neg_limit("GLOADNL", -3 );
SetupNG<float>			gload_pos_max("GLOADPM", 1 );
SetupNG<float>			gload_neg_max("GLOADNM", 1 );
SetupNG<int>        	display_variant("DISPLAY_VARIANT", 0 );
SetupNG<int>        	compass_dev_auto("COMPASS_DEV", 0 );
SetupNG<float>       	max_circle_wind_diff("CI_WINDDM", 60.0 );
SetupNG<float>       	circle_wind_lowpass("CI_WINDLOW", 5 );
SetupNG<int> 			can_speed( "CANSPEED", CAN_SPEED_OFF );
SetupNG<int> 			can_tx( "CANTX", RT_XCVARIO );
SetupNG<int> 			can_mode( "CANMOD", CAN_MODE_STANDALONE );


mpud::raw_axes_t zero_bias;
SetupNG<mpud::raw_axes_t>	gyro_bias("GYRO_BIAS", zero_bias );
SetupNG<mpud::raw_axes_t>	accl_bias("ACCL_BIAS", zero_bias );


void SetupCommon::sendSetup( e_sync_t sync, const char *key, char type, void *value, int len ){
	// ESP_LOGI(FNAME,"sendSetup(): key=%s, type=%c, len=%d", key, type, len );
	char str[40];
	char sender;
	if( isMaster()  )
		sender='M';
	else if( isClient() )
		sender='C';
	else
		sender='U';
	if( sender != 'U' ) {
		int l = sprintf( str,"!xs%c,%s,%c,%d,", sender, key, type, len );
		if( type == 'F' )
			sprintf( str+l,"%.3f", *(float*)(value) );
		else if( type == 'I' )
			sprintf( str+l,"%d", *(int*)(value) );
		// ESP_LOGI(FNAME,"Setup data: %s", str );
		OV.sendNMEAString(str);
	}
}

SetupCommon * SetupCommon::getMember( const char * key ){
	for(int i = 0; i < entries.size(); i++ ) {
		if( std::string( key ) == std::string( entries[i]->key() )){
			// ESP_LOGI(FNAME,"found key %s", entries[i]->key() );
			return entries[i];
		}
	}
	return 0;
}

// at time of connection establishment
void SetupCommon::syncEntry( int entry ){
	// ESP_LOGI(FNAME,"SetupCommon::syncEntry( %d )", entry );
	if( (isMaster() && entries[entry]->getSync() == SYNC_FROM_MASTER) ||
			(isClient() && entries[entry]->getSync() == SYNC_FROM_CLIENT) ||
			(isMaster() && entries[entry]->getSync() == SYNC_BIDIR)
	)
	{
		// ESP_LOGI(FNAME,"We are wireless type=%d", wireless );
		if( entry < entries.size() ) {
			entries[entry]->sync();
		}
	}
}

bool SetupCommon::factoryReset(){
	ESP_LOGI(FNAME,"\n\n******  FACTORY RESET ******");
	bool retsum = true;
	for(int i = 0; i < entries.size(); i++ ) {
		ESP_LOGI(FNAME,"i=%d %s erase", i, entries[i]->key() );
		if( entries[i]->mustReset() ){
			bool ret = entries[i]->erase();
			if( ret != true ) {
				ESP_LOGE(FNAME,"Error erasing %s", entries[i]->key() );
				retsum = false;
			}
			ret = entries[i]->init();
			if( ret != true ) {
				ESP_LOGE(FNAME,"Error init with default %s", entries[i]->key() );
				retsum = false;
			}
			else
				ESP_LOGI(FNAME,"%s successfully initialized with default", entries[i]->key() );
		}
	}
	if( retsum )
		ESP_LOGI(FNAME,"Factory reset SUCCESS");
	else
		ESP_LOGI(FNAME,"Factory reset FAILED!");
	return retsum;
}

bool SetupCommon::initSetup( bool& present ) {
	bool ret=true;
	ESP_LOGI(FNAME,"SetupCommon::initSetup()");
	esp_err_t _err = nvs_flash_init();
	if (_err == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_LOGE(FNAME,"Error no more space in NVS: erase partition");
		const esp_partition_t* nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
		_err = (esp_partition_erase_range(nvs_partition, 0, nvs_partition->size));
		if ( _err != ESP_OK ){
			ESP_LOGE(FNAME, "partition erase returned error ret=%d", _err );
			ret = false;
		}
	}
	if( ahrs_licence_dig1.exists() )
		present = true;
	else
		present = false;

	for(int i = 0; i < entries.size(); i++ ) {
			bool ret = entries[i]->init();
			if( ret != true )
				ESP_LOGE(FNAME,"Error init with default NVS: %s", entries[i]->key() );
	}

	if( factory_reset.get() ) {
		ESP_LOGI(FNAME,"\n\n******  FACTORY RESET ******");
		for(int i = 0; i < entries.size(); i++ ) {
			ESP_LOGI(FNAME,"i=%d %s erase", i, entries[i]->key() );
			if( entries[i]->mustReset() ){
				bool ret = entries[i]->erase();
				if( ret != true ) {
					ESP_LOGE(FNAME,"Error erasing %s", entries[i]->key() );
					ret = false;
				}
				ret = entries[i]->init();
				if( ret != true ) {
					ESP_LOGE(FNAME,"Error init with default %s", entries[i]->key() );
					ret = false;
				}
				else
					ESP_LOGI(FNAME,"%s successfully initialized with default", entries[i]->key() );
			}
		}
	}
	return ret;
};

char * SetupCommon::getID() {
	if( strlen( _ID ) == 0 ) {
		uint8_t mac[6];
		unsigned long  crc = 0;
		if ( esp_efuse_mac_get_default(mac) == ESP_OK ){
			crc = mz_crc32(0L, mac, 6);
		}
		if( hardwareRevision.get() >= 3 ){
			sprintf( _ID, "XCVario-%04d", int(crc % 10000) );
		}
		else{
			sprintf( _ID, "iVario-%03d", int(crc % 1000));
		}
	}
	return _ID;
}

bool SetupCommon::isMaster(){
	return((wireless_type.get() == WL_WLAN) || (can_speed.get() != CAN_SPEED_OFF && can_mode.get() == CAN_MODE_MASTER));
}

bool SetupCommon::isClient(){
	return((wireless_type.get() == WL_WLAN_CLIENT) || (can_speed.get() != CAN_SPEED_OFF && can_mode.get() == CAN_MODE_CLIENT));
}

