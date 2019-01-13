/*
 * SetupCMD.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#include "SetupCMD.h"
#include <string>
#include <stdio.h>
#include "esp_system.h"
#include <esp_log.h>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ESP32NVS.h"
#include "rom/uart.h"
#include <iostream>
#include <map>
#include <math.h>
#include "ESPAudio.h"
#include "BMPVario.h"

char cb[128];

#include "esp_task_wdt.h"

void SetupCMD::factorySetting()
{
		_setup._speedcal = 0.0;
		_setup._deadband = 0.3;
		_setup._deadband_neg = -0.3;
		_setup._center_freq = 500.0;
		_setup._tone_var = 2.0;
		_setup._vario_delay = 3.0; // seconds
		_setup._analog_adj = 0.0;
		_setup._QNH = 1013.25;
		_setup._blue_enable = 1;
		_setup._range = 5.0;
		_setup._v1 = 100.0;
		_setup._w1 = -0.7;
		_setup._v2 = 150.0;
		_setup._w2 = -1.2;
		_setup._v3 = 200.0;
		_setup._w3 = -2.2;
		_setup._ballast = 0.0;
		_setup._MC = 0.0;
		_setup._voltmeter_adj = 0;
		_setup._factory_reset = 0;
		_setup._audio_range = 0;
		_setup._alt_select = 1;
		_setup._offset = 0;
		_setup._contrast_adj = 70.0;

		memset( _setup._bt_name, 0, sizeof( _setup._bt_name) );
		memcpy( _setup._bt_name, "iVario", 7 );
		commit();
}

void SetupCMD::begin( BTSender *btsender ) {
	printf( "Setup CMD begin()\n");
	_btsender = btsender;
	esp_err_t success;
	void * obj = NVS.getObject("SetupXC", &success);

	if ( success != ESP_OK ){
		printf( "Setup data not yet in NVS: Init\n");
	    factorySetting();
	}
	else{
		memcpy( &_setup, obj, sizeof( _setup) );
		if( checkSum() == false )
			factorySetting();
		else {
			printf( "SetupCMD: NVS data ok\n");
		}
		if( _setup._factory_reset == 1 ){
			printf( "SetupCMD: Factory Setting requested\n");
			factorySetting();
		}
	}
	_test_ms=0.0;
}

bool SetupCMD::checkSum( bool set ) {
	uint32_t cs=0;
	uint8_t * p=(uint8_t *)&_setup;
	for( int i=0; i< sizeof(_setup)-4; i++ )
		cs += *p++;
	if( set == true ) {
	   _setup._checksum = cs;
		return( true );
	}
	else {
		if (_setup._checksum == cs ) {
		   printf( "SetupCMD: NVS checksum ok %d\n", cs );
		   return( true );
		}
		else {
			printf( "SetupCMD: NVS checksum NOT ok !\n");
			return( false );
		}
	}
}

void SetupCMD::commit( ) {

	bool success;
	checkSum( true );
	success = NVS.setObject("SetupXC", &_setup, sizeof(_setup) );
	if ( success != true ){
		sprintf( cb, "Error storing data NVS, maybe HW defect");
	}
	else
		sprintf( cb,"Store data in NVS success\n");
}

void SetupCMD::tick() {
	if( _ticker > 600 && *_operationMode == false ){
		*_operationMode = true;
		_menucount = 0;
		sprintf( cb,"Setup mode timeout: Restart\n\n");
		esp_restart();
	}
	else {
		_ticker++;
	}
	uint8_t c;
	STATUS s = uart_rx_one_char(&c);
	// printf( "UART %d %c\n", s, (char)c );
	if (s == OK) {
		_ticker = 0;
		if( (c == 0x0d) || (_index > 30) || (c == 0x0a) ){
			_input[_index]=0;
			_index = 0;
			doSetup( (char *)_input );
		}
		else{
			printf( "%c", c );
			fflush(stdout);
			if (c != 0x7f)  // handle backspace correctly
			{
				_input[_index]=c;
				_index++;

			}
			else{
				_index--;
			}
		}
	}
}

void SetupCMD::help(){
	sprintf( cb,"SoarBox XC setup\n"); btprintf();
	sprintf( cb,"  name : %s\n",_setup._bt_name ); btprintf();
	sprintf( cb,"  speed: %0.2f\n",_setup._speedcal); btprintf();
	sprintf( cb,"  avs  : %0.2f\n",_setup._vario_delay ); btprintf();
	sprintf( cb,"  tfc  : %0.2f\n",_setup._center_freq ); btprintf();
	sprintf( cb,"  tva  : %0.2f\n",_setup._tone_var ); btprintf();
	sprintf( cb,"  dba  : %0.2f\n",_setup._deadband ); btprintf();
	sprintf( cb,"  dbs  : %0.2f\n",_setup._deadband_neg ); btprintf();
	sprintf( cb,"  adj  : %0.2f\n",_setup._analog_adj ); btprintf();
	sprintf( cb,"  QNH  : %0.2f\n",_setup._QNH ); btprintf();
	sprintf( cb,"Commands:\n"); btprintf();
	sprintf( cb,"  name  <val>: Bluetooth device name\n"); btprintf();
	sprintf( cb,"  speed <val>: Speed calibration in +-%%\n"); btprintf();
	sprintf( cb,"  avs   <val>: Average vario in seconds\n"); btprintf();
	sprintf( cb,"  tfc   <val>: Tone base frequency at 0 m/s\n"); btprintf();
	sprintf( cb,"  tva   <val>: Tone variation factor, 2: 2 octaves\n"); btprintf();
	sprintf( cb,"  dba   <val>: Tone deadband pos, mute + m/s\n"); btprintf();
	sprintf( cb,"  dbs   <val>: Tone deadband neg, mute - m/s\n"); btprintf();
	sprintf( cb,"  adj   <val>: Analog output adjust, in +-%%\n"); btprintf();
	sprintf( cb,"  test  <val>: Test audio/analog out with given rate\n"); btprintf();
	sprintf( cb,"Enter e.g. \"speed ?\" for details\n"); btprintf();
	sprintf( cb,"  exit       : Exit setup (timeout 1 min)\n"); btprintf();
	sprintf( cb,"  fact_reset : Factory reset: default values\n"); btprintf();
	sprintf( cb,"  help,?     : This screen\n"); btprintf();
}


void SetupCMD::btprintf()
{
	_btsender->send( cb );
}

bool SetupCMD::thisCmd( std::string cmd, const char* c_prefix )
{
	std::string prefix( c_prefix );
	if(cmd.substr(0, prefix.size()) == prefix)
		return true;
	else
		return false;

}

void SetupCMD::setFloat( std::string cmd, const char* c_prefix,
		float *val, float min, float max,
		const char *errtxt, const char * oktxt, const char * showtxt, bool store )
{
	std::string prefix( c_prefix );
	float tmp = 0;
	if (cmd.length() > prefix.length()){
		std::string num=cmd.substr(prefix.length()+1);
		sscanf( num.c_str(),"%f", &tmp );
		if( ((tmp < min) || (tmp > max)) || (num == "?")  ){
			sprintf( cb, errtxt ); btprintf();
		}
		else {

			sprintf( cb, oktxt, tmp ); btprintf();
			*val = tmp;
			if( store )
				commit();
			sleep(1);
		}
	}
	else{
		sprintf( cb,showtxt,*val); btprintf();
	}
}

void SetupCMD::doSetup( char *cmd_c ){
	// sprintf( cb,"doSetup(%s)",cmd);
	_ticker = 0;
	*_operationMode = false;
	std::string cmd( cmd_c );
	if (thisCmd( cmd, "end") )  {
		*_operationMode = true;
		sprintf( cb,"\nSetup mode ended, restart..\n"); btprintf();
		sleep( 5 );
		esp_restart();
	}
	else if ( thisCmd (cmd, "fact_reset") ) {
		sprintf( cb,"\nErase all setup data: set defauts..\n"); btprintf();
		sleep( 5 );
		NVS.eraseAll();
		esp_restart();
	}
	else if ( thisCmd( cmd, "name" ) ) {
		if (cmd.length() > strlen("name")){
			std::string tmp;
			tmp = cmd.substr(strlen("name")+1, cmd.length()-(strlen("name")+1));
			if( (tmp.length() < 5) || (tmp.length() > 30) ){
				sprintf( cb,"\n Invalid string entered, enter at least 5, max 30 characters\n"); btprintf();
			}
			else {
				sprintf( cb,"\nSave new name: %s\n", tmp.c_str() ); btprintf();
				memset( _setup._bt_name, 0, sizeof(_setup._bt_name ));
				memcpy( _setup._bt_name, tmp.c_str(), strlen(tmp.c_str()));
				commit();
				sleep( 5 );
			}
		}
		else{
			sprintf( cb," Device name: %s\n",_setup._bt_name ); btprintf();
		}
	}
	else if ( thisCmd( cmd, "help" ) || (_menucount == 0) ) {
		help();
		_menucount = 1;
	}
	else if ( cmd.length() == 0 ) {   // enter pressed, print prompt
		;
	}
	else if ( thisCmd( cmd, "avs") ){
		setFloat( cmd, "avs", &_setup._vario_delay, 0.2, 5.0,
				"\nInvalid float value entered\nExample: avs 1.5\nMin value 0.2, max value 5.0\nSeconds vario average\nFormat: avs d.d[d]",
				"\nNew vario integration value: %0.2f\n",
				"\nVario integration: %0.2f\n" );
	}
	else if ( thisCmd( cmd, "tfc") ){
			setFloat( cmd, "tfc", &_setup._center_freq, 200.0, 3000.0,
					"\nInvalid float value entered\nExample: tfc 600\nMin value 200, max value 3000\nAudio Vario Center Frequency\nFormat: tfc d.d[d]",
					"\nNew Center Frequency value: %0.2f\n",
					"\nCenter Frequency: %0.2f\n" );
		}
    else if ( thisCmd( cmd, "tva") ){
			setFloat( cmd, "tva", &_setup._tone_var, 1, 10,
					"\nInvalid float value entered\nExample: tva 2\nMin value 1, max value 10\nAudio Vario Tone Variation Factor\nFormat: tfc d.d",
					"\nNew Tone Variation Factor: %0.1f\n",
					"\nTone Variation Factor: %0.1\n" );
		}
    else if ( thisCmd( cmd, "dba") ){
   			setFloat( cmd, "dba", &_setup._deadband, 0, 5,
   					"\nInvalid float value entered\nExample: dba 0.2\nMin value 0, max value 5.0\nAudio Vario Deadband (Mute)\nFormat: dba d.d",
   					"\nNew pos. Audio Deadband value: %0.1f\n",
   					"\nPositive Audio Deadband: %0.1f\n" );
   		}
    else if ( thisCmd( cmd, "dbs") ){
      			setFloat( cmd, "dbs", &_setup._deadband_neg, -5, 0,
      					"\nInvalid float value entered\nExample: dbs -0.7\nMin value 0, max value -5.0\nAudio Vario Deadband (Mute)\nFormat: dbs d.d",
      					"\nNew neg. Audio Deadband value: %0.1f\n",
      					"\nNegative Audio Deadband: %0.1f\n" );
      		}
	else if ( thisCmd( cmd,"speed") ){
		setFloat( cmd, "speed", &_setup._speedcal, -50.0, 50.0,
				"\nInvalid float value entered\nExample: speed -2.5\nMin value -50.0, max value 50.0\nPercent speed adjust\nFormat: speed [-]d[d].d[d]",
				"\nNew speed calibration: %0.2f\n",
				"\nSpeed calibration: %0.2f\n" );
	}

	else if ( thisCmd( cmd,"adj") ){
		setFloat( cmd, "adj", &_setup._analog_adj, -100.0, 100.0,
	 			"\nInvalid float value entered\nExample: adj 7.0\nMin value -100.0, max value 100.0\nPercent analog adjust\nFormat: adj [-]d[d].d[d]",
				"\nNew analog output calibration: %0.2f\n",
				"\nAnalog output calibration: %0.2f\n" );
		bmpVario.setAdj( _setup._analog_adj );
	}
	else if ( thisCmd( cmd,"test") ){
		float old = _test_ms;
		setFloat( cmd, "test", &_test_ms, -5.0, 5.0,
			 			"\nInvalid float value entered\nExample: test 1.0\nMin value -5.0, max value 5.0\nMeter per second climb/sink\nFormat: test [-]d[d].d[d]",
						"\nNew vario test climb/sink rate: %0.2f\n",
						"\nTest sink rate: %0.2f\n", false );
		// printf("test_ms: %0.2f", _test_ms );
		bmpVario.setTE( _test_ms );
		Audio.test( _test_ms, old );
	}
	else {
		sprintf( cb,"   :command %s not understood\n", cmd.c_str()); btprintf();
		help();
	}
	sprintf( cb,"\nsetup>"); btprintf();
	fflush(stdout);

}
