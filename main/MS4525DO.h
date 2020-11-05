#ifndef MS4525DO_H
#define MS4525DO_H

#include <inttypes.h>
#include <stdio.h>
#include "I2C.h"
#include <string.h>
#include <logdef.h>
#include "Setup.h"
#include <math.h>

#define I2C_ADDRESS_MS4525DO    0x28    /**< 7-bit address =0x28. 8-bit is 0x50. Depends on the order code (this is for code "I") */
 
/* Register address */
#define ADDR_READ_MR            0x00    /* write to this address to start conversion */
 
// MS4525D sensor full scale range and units
const int16_t MS4525FullScaleRange = 1;  // 1 psi
 
// MS4525D Sensor type (A or B) comment out the wrong type assignments
// Type A (10% to 90%)
const int16_t MS4525MinScaleCounts = 1638;
const int16_t MS4525FullScaleCounts = 14746;
 
const int16_t MS4525Span=MS4525FullScaleCounts-MS4525MinScaleCounts;
 
//MS4525D sensor pressure style, differential or otherwise. Comment out the wrong one.
//Differential
const int16_t MS4525ZeroCounts=(MS4525MinScaleCounts+MS4525FullScaleCounts)/2;
 
#define MAX_AUTO_CORRECTED_OFFSET 50

const float multiplier ( 2 * 6894.76 / MS4525Span );

class MS4525DO : public I2C
{
    public:
    // instance methods
        MS4525DO();
        ~MS4525DO();
    
    // public functions
        bool  begin( gpio_num_t sda, gpio_num_t sck, char slave_adr = I2C_ADDRESS_MS4525DO );
        void  initialize(void);
        int   measure(void);            // returns status of measurement
        float getPSI(void);             // returns the PSI of last measurement
        float getTemperature(void);     // returns temperature of last measurement
        float getAirSpeed(void);        // calculates and returns the airspeed
        bool  doOffset( bool force=false );
        float readPascal( float minimum=10 );
    	inline float pascal2km( float pascal ) {  float v = sqrt( 2*pascal / 1.225 ); return v*3.6; };
        char  fetch_pressure(uint16_t &P_dat, uint16_t &T_dat);
        bool  offsetPlausible( uint16_t offset );
        bool  selfTest( int& adval );

    private:
        char        address;
        char        _status;
        float       psi;
        float       temperature;
        float       airspeed;
        uint16_t    P_dat;  // 14 bit pressure data
        uint16_t    T_dat;  // 11 bit temperature data
        esp_err_t   error;
    	float       _offset;
    // private functions
        int collect(void);
          
    
};  // end of the class
 
#endif 
 
