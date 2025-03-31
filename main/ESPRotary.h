/////////////////////////////////////////////////////////////////
/*
  Library for reading rotary encoder values using Observer Pattern, and GPIO Interrups
*/

#pragma once

#include <driver/pulse_cnt.h>
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


class ESPRotary
{
	friend void ObserverTask(void *arg);

public:
	ESPRotary(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw);
	virtual ~ESPRotary() = default;
	void begin();
	esp_err_t updateRotDir();

	// observer feed
	void sendUp(int diff) const;
	void sendDown(int diff) const;
	void sendPress() const;
	void sendRelease() const;
	void sendLongPress() const;
	void sendEscape() const;
	bool readSwitch(); // returns true if pressed
	inline gpio_num_t getSw() { return sw; };

private:
	gpio_num_t clk, dt, sw;
	pcnt_unit_handle_t pcnt_unit = nullptr;
	pcnt_channel_handle_t pcnt_chan = nullptr;
};

extern ESPRotary *Rotary;

