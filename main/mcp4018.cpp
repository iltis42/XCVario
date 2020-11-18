#include "mcp4018.h"
#include "I2C.h"
#include <logdef.h>

//Create instance  MCP4018(gpio_num_t sda, gpio_num_t scl);
MCP4018::MCP4018()
{
	errorcount=0;
	_noDevice = false;
	wiper = 63;
}


bool MCP4018::begin()
{
	errorcount=0;
	if( readWiper( wiper ) ) {
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

bool MCP4018::incWiper(){
	if( wiper <127 )
		wiper++;
	return( writeWiper(wiper) );
}

bool MCP4018::decWiper(){
	if( wiper >1 )
		wiper--;
	return( writeWiper(wiper) );
}


bool MCP4018::readWiper( uint16_t &val ) {
	esp_err_t err = bus->read8bit(MPC4018_I2C_ADDR, &val );
	if( err == ESP_OK ){
		ESP_LOGI(FNAME,"MCP4018 read wiper val=%d  OK", val );
		return true;
	}
	else
	{
		ESP_LOGE(FNAME,"MCP4018 Error reading wiper, error count %d", errorcount);
		errorcount++;
	    return false;
	}
}

bool MCP4018::writeWiper( uint16_t val ) {
    uint8_t data = (uint8_t)val;
    ESP_LOGV(FNAME,"MCP4018 write wiper %d", data);
	esp_err_t err = bus->writeByte(MPC4018_I2C_ADDR, 0, data );
	if( err == ESP_OK ){
		ESP_LOGV(FNAME,"MCP4018 write wiper OK");
		return true;
	}
	else
	{
		ESP_LOGE(FNAME,"MCP4018 Error writing wiper, error count %d", errorcount);
		errorcount++;
	    return false;
	}
}




