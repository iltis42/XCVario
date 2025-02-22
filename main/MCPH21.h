#ifndef MCPH21_H
#define MCPH21_H

#include <inttypes.h>
#include <stdio.h>
// #include "I2C.h"
#include <string.h>
#include <logdef.h>
#include "Setup.h"
#include <math.h>
#include "I2Cbus.hpp"
#include "AirspeedSensor.h"


#define I2C_ADDRESS_MCPH21    0x7F  //  Datasheet testcode: IC_Send(0xFE,..) what is 7F shifted right 1 bit

// MCPH21D sensor full scale range and units
const int16_t MCPH21FullScaleRange = 0.725;  //  psi
 
// MCPH21D Sensor type (10% to 90%)
// Output (% of 2^14 counts) = P max. 80% x (Pressure applied – P min. ) + 10%

const int16_t MCPH21MinScaleCounts = 0;


// Long term stability of Sensor as from datasheet FS* 0.15 + 0.3 (dT) % per year -> 16777216 * 0.00015 = 2516
#define MAX_AUTO_CORRECTED_OFFSET 2516

// const float MCPH21multiplier =  2 * 6894.76 / MCPH21Span;

class MCPH21 : public AirspeedSensor
{
    public:
    // instance methods
        MCPH21();
        ~MCPH21();
    
    // public functions
        bool  doOffset( bool force=false );
        float readPascal( float minimum, bool &ok );
        bool  offsetPlausible( uint32_t offset );
        bool  selfTest( int& adval );
        void  setBus( I2C_t *theBus ) {  bus = theBus; };
        void  changeConfig();

    private:
        bool  measure(void);            // returns status of measurement
        float getTemperature(void);     // returns temperature of last measurement
        float getAirSpeed(void);        // calculates and returns the airspeed
        bool  fetch_pressure(uint32_t &P_dat, uint16_t &T_dat);


        I2C_t *bus;
        const char   address = I2C_ADDRESS_MCPH21;
        char        _status;
        float       psi;
        float       temperature;
        float       airspeed;
        uint32_t    P_dat;  // 24 bit pressure data raw
        uint16_t    T_dat;  // 16 bit temperature data raw
        esp_err_t   error;
    	float       _offset;
    	float       multiplier;
    // private functions
        int collect(void);
    
};  // end of the class
 
#endif 
 
