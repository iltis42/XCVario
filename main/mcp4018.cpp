#include "mcp4018.h"
#include "I2C.h"

//Create instance  MCP4018(gpio_num_t sda, gpio_num_t scl);
MCP4018::MCP4018(): I2C()
{
	errorcount=0;
	_noDevice = false;
	wiper = 63;
}


bool MCP4018::begin(gpio_num_t sda, gpio_num_t scl)
{
	errorcount=0;
	// init( sda, scl );
	// return true;
	if( readWiper( wiper ) ) {
		printf("MCP4018 wiper=%d\n", wiper );
		return(true);
	}
	else {
		printf("MCP4018 Error reading wiper!\n");
	    return( false );
	}

}

//destroy instance
MCP4018::~MCP4018()
{
}

bool MCP4018::haveDevice() {
	  uint16_t val;
	  if( read16bit( MPC4018_I2C_ADDR, &val ) == ESP_OK )
	     return true;
	  else
		 return false;
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


bool MCP4018::readWiper( uint16_t & val ) {
	esp_err_t ret = read16bit( MPC4018_I2C_ADDR, &val );
	val = val&0x7F;
	if( ret == ESP_OK ){
		printf("MCP4018 read wiper %d\n", val);
		return true;
	}
	else
	{
		printf("MCP4018 Error reading wiper, error count %d\n", errorcount);
		errorcount++;
	    return false;
	}
}

bool MCP4018::writeWiper( uint16_t val ) {
	esp_err_t ret = write8bit( MPC4018_I2C_ADDR, val );
	if( ret == ESP_OK ){
		printf("MCP4018 write wiper %d\n", val);
		return true;
	}
	else
	{
		printf("MCP4018 Error writing wiper, error count %d\n", errorcount);
		errorcount++;
	    return false;
	}
}




