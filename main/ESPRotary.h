/////////////////////////////////////////////////////////////////
/*
  Library for reading rotary encoder values using Observer Pattern, and GPIO Interrups
*/

#pragma once

#include <driver/pulse_cnt.h>
#include <esp_timer.h>
#include <driver/gpio.h>

enum _event
{
	NONE,
	PRESS,
	LONG_PRESS,
	RELEASE,
	UP,
	DOWN,
	ERROR,
	MAX_EVENT
};


class RotaryObserver
{
public:
	RotaryObserver() {};
	virtual ~RotaryObserver() {};
	virtual void up(int count) = 0;
	virtual void down(int count) = 0;
	virtual void press() = 0;
	virtual void longPress() = 0;
	virtual void release() = 0;
	virtual void escape() = 0;

	void attach();
	void detach();
};

static void button_isr_handler(void* arg);
static void longpress_timeout(void *arg);
static void ObserverTask(void *arg);

class ESPRotary
{
	friend void button_isr_handler(void* arg);
	friend void longpress_timeout(void *arg);
	friend void ObserverTask(void *arg);

public:
	static constexpr const int LONG_PRESS_THRESHOLD = 400;

	ESPRotary(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw);
	virtual ~ESPRotary() = default;
	void begin();
	esp_err_t updateRotDir();
	void setLongPressTimeout(int lptime_ms) { lp_duration = (uint64_t)1000 * lptime_ms; }

	// observer feed
	void sendUp(int diff) const;
	void sendDown(int diff) const;
	void sendPress() const;
	void sendRelease() const;
	void sendLongPress() const;
	void sendEscape() const;
	bool readSwitch() const { return state; } // returns true if pressed
	inline gpio_num_t getSw() { return sw; };

private:
	gpio_num_t clk, dt, sw;
	pcnt_unit_handle_t pcnt_unit = nullptr;
	pcnt_channel_handle_t pcnt_chan = nullptr;
	esp_timer_handle_t lp_timer = nullptr;
	uint64_t lp_duration = LONG_PRESS_THRESHOLD * 1000; // default 400msec
	bool state = false;
	bool hold = false; // timer timeout set the hold state
};

extern ESPRotary *Rotary;

