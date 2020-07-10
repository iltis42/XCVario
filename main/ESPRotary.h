/////////////////////////////////////////////////////////////////
/*
  Library for reading rotary encoder values using Observer Pattern, and GPIO Interrups
*/

#ifndef ESPRotary_h
#define ESPRotary_h

#include "driver/gpio.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include "esp_system.h"
#include <string>
#include <vector>
#include <freertos/semphr.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "driver/pcnt.h"


class RotaryObserver{
public:
	RotaryObserver(){};
	virtual void up( int count ) = 0;
	virtual void down( int count ) = 0;
	virtual void press() = 0;
	virtual void release() = 0;
	virtual void longPress() = 0;
	virtual ~RotaryObserver() {};
private:
};


enum _event { NONE, PRESS, LONG_PRESS, RELEASE, UP, DOWN, ERROR, MAX_EVENT };

class ESPRotary {
public:
    ESPRotary() {};
    void begin(gpio_num_t clk, gpio_num_t dt, gpio_num_t sw );
    void attach( RotaryObserver *obs);
    static void readPos( void * args );
    static void informObservers( void * args );
    static void readPosInt( void * args );
    bool readSwitch();  // returns true if pressed

private:
    xSemaphoreHandle swMutex;

	static std::vector<RotaryObserver *> observers;
    static gpio_num_t clk, dt, sw;
    static pcnt_config_t enc;
    static pcnt_config_t enc2;
    static int16_t r_enc_count;
    static int16_t r_enc2_count;
};

#endif
