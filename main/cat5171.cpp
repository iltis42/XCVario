#include "cat5171.h"
#include "I2C.h"
#include <logdef.h>

//Create instance  CAT5171(gpio_num_t sda, gpio_num_t scl);
CAT5171::CAT5171()
{
	errorcount=0;
	_noDevice = false;
	wiper = 63;
	bus = 0;
}

bool CAT5171::begin()
{
	errorcount=0;
	if( readWiper( wiper ) ) {
		ESP_LOGI(FNAME,"CAT5171 wiper=%d", wiper );
		return(true);
	}
	else {
		ESP_LOGE(FNAME,"CAT5171 Error reading wiper!");
	    return( false );
	}
}

//destroy instance
CAT5171::~CAT5171()
{
}

bool CAT5171::haveDevice() {
	  ESP_LOGI(FNAME,"CAT5171 haveDevice");
	  esp_err_t err = bus->testConnection(CAT5171_I2C_ADDR);
	  if( err == ESP_OK ) {
		 ESP_LOGI(FNAME,"CAT5171 haveDevice: OK");
	     return true;
	  }
	  else{
		 ESP_LOGI(FNAME,"CAT5171 haveDevice: NONE");
		 return false;
	  }
}

bool CAT5171::incWiper(){
	if( wiper <getRange() )
		wiper++;
	return( writeWiper(wiper) );
}

bool CAT5171::decWiper(){
	if( wiper >1 )
		wiper--;
	return( writeWiper(wiper) );
}


bool CAT5171::readWiper( uint16_t &val ) {
	esp_err_t err = bus->read8bit(CAT5171_I2C_ADDR, &val );
	if( err == ESP_OK ){
		// ESP_LOGI(FNAME,"CAT5171 read wiper val=%d  OK", val );
		return true;
	}
	else
	{
		ESP_LOGE(FNAME,"CAT5171 Error reading wiper, error count %d", errorcount);
		errorcount++;
	    return false;
	}
}

bool CAT5171::writeWiper( uint16_t val ) {
    // ESP_LOGI(FNAME,"CAT5171 write wiper %d", val );
	esp_err_t err = bus->write2bytes( CAT5171_I2C_ADDR, 0, (uint8_t)val );  // 0x40 = RS = midscale
	if( err != ESP_OK ){
		// ESP_LOGV(FNAME,"CAT5171 write wiper OK");
		ESP_LOGE(FNAME,"CAT5171 Error writing wiper, error count %d", errorcount);
		errorcount++;
		return false;
	}
	uint16_t b;
	bool ret=readWiper( b );

	if( b != (uint8_t)val ){
		ESP_LOGE(FNAME,"CAT5171 Error writing wiper, error count %d, write %d != read %d", errorcount, val, b );
	}

	// ESP_LOGI(FNAME,"CAT5171 write wiper OK");
	return true;
}


bool CAT5171::reset() {
    ESP_LOGI(FNAME,"CAT5171 reset");
	esp_err_t err = bus->write2bytes( CAT5171_I2C_ADDR, 0x60, 128 );  // 0x40 = RS = midscale
	if( err != ESP_OK ){
		// ESP_LOGV(FNAME,"CAT5171 write wiper OK");
		ESP_LOGE(FNAME,"CAT5171 Error writing wiper, error count %d", errorcount);
		errorcount++;
		return false;
	}
	ESP_LOGI(FNAME,"CAT5171 reset OK");
	return true;
}

