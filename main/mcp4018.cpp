#include "mcp4018.h"
#include "I2C.h"
#include <logdef.h>

//Create instance  MCP4018(gpio_num_t sda, gpio_num_t scl);
MCP4018::MCP4018()
{
	errorcount=0;
	_noDevice = false;
	wiper = MCP4018RANGE/2;
	bus = 0;
	_scale = 100;
}

bool MCP4018::begin()
{
	errorcount=0;
	int local_wiper;
	if( readWiper( local_wiper ) ) {
		wiper = local_wiper;
		ESP_LOGI(FNAME,"MCP4018 wiper=%d", wiper );
		return(true);
	}
	else {
		ESP_LOGE(FNAME,"MCP4018 Error reading wiper!");
	    return( false );
	}
}

//destroy instance
MCP4018::~MCP4018()
{
}

bool MCP4018::haveDevice() {
	  ESP_LOGI(FNAME,"MCP4018 haveDevice");
	  esp_err_t err = bus->testConnection(MPC4018_I2C_ADDR);
	  if( err == ESP_OK ) {
		 ESP_LOGI(FNAME,"MCP4018 haveDevice: OK");
	     return true;
	  }
	  else{
		 ESP_LOGI(FNAME,"MCP4018 haveDevice: NONE");
		 return false;
	  }
}

bool MCP4018::readWiper( int &val ) {
	uint16_t i16val;
	esp_err_t err = bus->read8bit(MPC4018_I2C_ADDR, &i16val );
	if( err == ESP_OK ){
		//ESP_LOGI(FNAME,"MCP4018 read wiper val=%d  OK", i16val );
		val = i16val;
		return true;
	}
	else
	{
		ESP_LOGE(FNAME,"MCP4018 Error reading wiper, error count %d", errorcount);
		errorcount++;
	    return false;
	}
}

bool MCP4018::writeWiper( int val ) {
    // ESP_LOGI(FNAME,"MCP4018 write wiper %d", val );
	esp_err_t err = bus->write8bit(MPC4018_I2C_ADDR, (uint16_t)val );
	if( err == ESP_OK ){
		// ESP_LOGV(FNAME,"MCP4018 write wiper OK");
		return true;
	}
	else
	{
		ESP_LOGE(FNAME,"MCP4018 Error writing wiper, error count %d", errorcount);
		errorcount++;
	    return false;
	}
}

bool MCP4018::readVolume( float &val ) {
	int ival;
	if ( readWiper( ival ) ) {
		val = (float)(_scale * ival) * getInvRange();
		return true;
	}
	else
	{
	    return false;
	}
}

bool MCP4018::writeVolume( float val ) {
	int ival = (int)(val * getRange());
	ival /= _scale;
	return writeWiper( ival );
}
