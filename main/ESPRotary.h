/////////////////////////////////////////////////////////////////
/*
  Library for reading rotary encoder values using Observer Pattern, and GPIO Interrups
*/

#ifndef ESPRotary_h
#define ESPRotary_h

#include "sdkconfig.h"
#include <inttypes.h>
#include "esp_system.h"
#include <string>
#include <vector>
#include <freertos/semphr.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"


class RotaryObserver{
public:
	RotaryObserver(){};
	virtual void up() = 0;
	virtual void down() = 0;
	virtual void press() = 0;
	virtual void release() = 0;
	virtual void longP() = 0;
	virtual ~RotaryObserver() {};
private:
};

struct ring_buffer
{
	ring_buffer( size_t cap ) : buffer(cap) {}
	bool empty() const { return sz == 0 ; }
	bool full() const { return sz == buffer.size() ; }

	void push( int i )
	{
		if( last >= buffer.size() ) last = 0 ;
		buffer[last] = i ;
		++last ;
		if( full() ) first = (first+1) %  buffer.size() ;
		else ++sz ;
	}
	int& operator[] ( size_t pos )
	{
		auto p = ( first + pos ) % buffer.size() ;
		return buffer[p] ;
	}

private:
	std::vector<int> buffer ;
	size_t first = 0 ;
	size_t last = 0 ;
	size_t sz = 0 ;
};

class ESPRotary {
public:
    ESPRotary();
    void begin(gpio_num_t clk, gpio_num_t dt, gpio_num_t sw );
    void attach( RotaryObserver *obs);
    static void readPos( void * args );
    static void readPosInt( void * args );
    static void readSwitch( void * args );

private:
    xSemaphoreHandle xMutex;
    static QueueHandle_t q1;
    static TickType_t xLastWakeTime;
	static std::vector<RotaryObserver *> observers;
    static gpio_num_t clk, dt, sw;
    static uint8_t state;
    static uint8_t _switch;
    static int dir;
    static int last_dir;
    static ring_buffer rb;
    static int last;
    static int _switch_state;
};

#endif
