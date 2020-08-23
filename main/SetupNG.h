/*
 * Setup.h
 *
 *  Created on: Dec 23, 2017
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
#include "esp_system.h"
#include "stdio.h"
#include <esp_log.h>
#include <string>
#include "string.h"
#include "BTSender.h"
#include "Polars.h"
#include <iostream>
#include <vector>



/*
typedef enum display_type { UNIVERSAL, RAYSTAR_RFJ240L_40P, ST7789_2INCH_12P, ILI9341_TFT_18P } display_t;
typedef enum chopping_mode { NO_CHOP, VARIO_CHOP, S2F_CHOP, BOTH_CHOP } chopping_mode_t;
typedef enum rs232linemode { RS232_NORMAL, RS232_INVERTED } rs232lm_t;
typedef enum nmea_protocol  { OPENVARIO, BORGELT } nmea_proto_t;
typedef enum airspeed_mode  { MODE_IAS, MODE_TAS } airspeed_mode_t;

 */

/*
typedef struct {
	uint32_t _config_version;
	float _speedcal;
	float _vario_delay;
	float _vario_av_delay;
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
	float    _s2f_deadband;
	float    _s2f_delay;
	float    _factory_volt_adjust;
	uint8_t  _battery_display;
	uint8_t  _nmea_protocol;
	float    _bugs;
	uint8_t  _airspeed_mode;
	uint32_t _checksum;
} setup_t;



 */

/*
 *
 * NEW API:
 * SetupNG<uint_8>  airspeed_mode( "AIRSPEED_MODE", MODE_IAS );
 *
 *
 * uint8_t as = airspeed_mode.get();
   airspeed_mode.set( MODE_TAS );
 *
 *
 *
 *
 *
 */


template<typename T>


class SetupNG {
public:
	T* getPtr() {
			return &_value;
	};
	T& getRef() {
			return _value;
	};
	T get() {
		return _value;
	};
	bool set( T aval ) {
		printf("set: ");
		std::cout << aval << "\n";
		_value = aval;
		if( !open() ) {
			printf("Error open nvs handle !\n");
			return( false );
		}
		bool ret = commit();
		return ret;
	};
	bool commit() {
		printf("NVS commit(): ");
		if( !open() ) {
			printf("NVS error\n");
			return false;
		}
		printf("set(key:%s , addr:%08x, len:%d, nvs_handle: %04x)\n", _key, (unsigned int)(&_value), sizeof( _value ), _nvs_handle);
		esp_err_t _err = nvs_set_blob(_nvs_handle, _key, (void *)(&_value), sizeof( _value ));
		if(_err != ESP_OK) {
			printf("set blob error %d\n", _err );
			close();
			return( false );
		}
		_err = nvs_commit(_nvs_handle);
		if(_err != ESP_OK)  {
			printf("NVS error\n");
			close();
			return false;
		}
		printf("success\n");
		close();
		return true;
	};
	SetupNG( const char * akey, T adefault ) {
		_nvs_handle = 0;
		printf("SetupNG(%s)\n", akey );
		esp_err_t _err = nvs_flash_init();
		if (_err == ESP_ERR_NVS_NO_FREE_PAGES) {
			printf("Error no more space in NVS: erase partition\n");
						   const esp_partition_t* nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
			_err = (esp_partition_erase_range(nvs_partition, 0, nvs_partition->size));
			if ( _err != ESP_OK ){
				printf( "partition erase returned error ret=%d\n", _err );
			}
		}
		if( !open() ) {
			printf("Error open nvs handle !\n");
			return;
		}
		_key = akey;
		_default = adefault;
		size_t required_size;
		printf("get_blob(key:%s, nvs_handle: %04x)\n", _key, _nvs_handle );
		_err = nvs_get_blob(_nvs_handle, _key, NULL, &required_size);
		if ( _err != ESP_OK ){
			printf( "nvs_get_blob returned error ret=%d\n", _err );
			set( _default );  // try to init
		}
		else {
			printf("Object size: %d\n", required_size );
			if( required_size > sizeof( T ) ) {
				printf("NVS size too big: %d > %d\n", required_size , sizeof( T ) );
				erase();
				set( _default );  // try to init
			}
			else {
				printf("NVS size okay: %d\n", required_size );
				_err = nvs_get_blob(_nvs_handle, _key, &_value, &required_size);
				if ( _err != ESP_OK ){
					printf( "nvs_get_blob returned error ret=%d\n", _err );
					erase();
					set( _default );  // try to init
				}
				else {
					printf("key %s already in Storage, value: ", _key );
					std::cout << _value << "\n";
				}
			}
		}
		close();
	};

    bool erase_all() {
		open();
		esp_err_t _err = nvs_erase_all(_nvs_handle);
		if(_err != ESP_OK)
			return false;
		else
			printf("erased all by handle %d\n", _nvs_handle );
		if( commit() )
			return true;
		else
			return false;

    };

    bool erase() {
    	open();
    	esp_err_t _err = nvs_erase_key(_nvs_handle, _key);
    	if(_err != ESP_OK)
    		return false;
    	else {
    		printf("erased %s by handle %d\n", _key, _nvs_handle );
    		if( set( _default ) )
    			return true;
    		else
    			return false;
    	}
    };


private:
	T _value;
	T _default;
	const char * _key;
	nvs_handle_t  _nvs_handle;

	bool open() {
		if( _nvs_handle == 0) {
			esp_err_t _err = nvs_open("SetupNG", NVS_READWRITE, &_nvs_handle);
			if(_err != ESP_OK){
				printf("ESP32NVS open storage error %d\n", _err );
				_nvs_handle = 0;
				return( false );
			}
			else {
				printf("ESP32NVS handle: %04X  OK\n", _nvs_handle );
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





#endif /* MAIN_SETUP_NG_H_ */
