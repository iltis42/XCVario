#include "mcp4018.h"

#include "I2Cbus.hpp"
#include "logdefnone.h"

#include <esp_system.h>

bool MCP4018::readWiper( int &val ) {
	uint16_t i16val;
	esp_err_t err = bus->readBits(MPC4018_I2C_ADDR, 0, 0, 2, (uint8_t*)&i16val);// 
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

bool MCP4018::writeWiper( int val, bool validate ) {
    // ESP_LOGI(FNAME,"MCP4018 write wiper %d", val );
    uint8_t v = (uint8_t)val;
	esp_err_t err = bus->writeBytes(MPC4018_I2C_ADDR, 1, &v);
	if( err == ESP_OK ){
		// ESP_LOGI(FNAME,"MCP4018 write wiper OK val=%d", val );
		return true;
	}
	else
	{
		ESP_LOGE(FNAME,"MCP4018 Error writing wiper, error count %d", errorcount);
		errorcount++;
	    return false;
	}
}

