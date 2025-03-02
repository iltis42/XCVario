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
	virtual void release() = 0;
	virtual void longPress() = 0;
	virtual void escape() = 0;

	// bool readSwitch() { return Rotary ? Rotary->readSwitch() : false; }
	
	void attach(RotaryObserver *obs);
	void detach(RotaryObserver *obs);
};


class ESPRotary : public RotaryObserver
{
	friend void ObserverTask(void *arg);

public:
	ESPRotary(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw);
	virtual ~ESPRotary() = default;
	void begin();
	esp_err_t updateRotDir();

	// observer API
	void up(int diff) override;
	void down(int diff) override;
	void press() override;
	void release() override;
	void longPress() override;
	void escape() override;
	bool readSwitch(); // returns true if pressed
	gpio_num_t getSw() { return sw; };

private:
	gpio_num_t clk, dt, sw;
	pcnt_unit_handle_t pcnt_unit = nullptr;
	pcnt_channel_handle_t pcnt_chan = nullptr;
};

extern ESPRotary *Rotary;

