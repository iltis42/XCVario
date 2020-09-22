#include "mcp3221.h"
#include "I2C.h"
#include <logdef.h>

//Create instance  MCP3221(gpio_num_t sda, gpio_num_t scl);
MCP3221::MCP3221(): I2C()
{
	errorcount=0;
	exponential_average=0;
	_noDevice = false;
}

bool MCP3221::begin(gpio_num_t sda, gpio_num_t scl)
{
	errorcount=0;
	init( sda, scl );
	exponential_average = 0;
	return true;
}

//destroy instance
MCP3221::~MCP3221()
{
}

float MCP3221::readAVG( float alpha ) {

	uint16_t newval;
	esp_err_t ret = readRaw(newval);
	// ESP_LOGI(FNAME,"Airspeed AD1: %d", newval );
	if( ret == ESP_OK ){
		// ESP_LOGI(FNAME, "%d", newval );
		if ( exponential_average == 0 ){
			exponential_average = newval;
		}
		exponential_average = exponential_average + alpha*(newval - exponential_average);
		return exponential_average;
	}
	else
		return 0.0;
}
//You cannot write to an MCP3221, it has no writable registers.
//MCP3221 also requires an ACKnowledge between each byte sent, before it will send the next byte. So we need to be a bit manual with how we talk to it.
//It also needs an (NOT) ACKnowledge after the second byte or it will keep sending bytes (continuous sampling)
//
//From the datasheet.
//
//I2C.START
//Send 8 bit device/ part address to open conversation.   (See .h file for part explanation)
//read a byte (with ACK)
//read a byte (with NAK)
//I2C.STOP

int  MCP3221::readVal(){
	int retval = 0;
	for( int i=0; i<32; i++ ){
		uint16_t as;
		readRaw( as );
		retval += as;
	}
	retval = retval / 32;
	// ESP_LOGI(FNAME,"Airspeed AD1 readVal: %d", retval );
	return retval;
}


esp_err_t MCP3221::readRaw(uint16_t &val)
{
	esp_err_t ret=read16bit( MCP3221_CONVERSE, &val );
	if( ret != ESP_OK ){
		val = 0;
	}
	return( ret );
}

MCP3221 MCP;
