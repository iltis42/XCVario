/////////////////////////////////////////////////////////////////
/*
  Library for reading rotary encoder values using Observer Pattern, and GPIO Interrups
*/

#pragma once

#include "protocol/ClockIntf.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include <driver/pulse_cnt.h>
#include <driver/gpio.h>



class RotaryObserver
{
public:
	RotaryObserver() {};
	virtual ~RotaryObserver() {};
	virtual void rot(int count) = 0;
	virtual void press() = 0;
	virtual void longPress() = 0;
	virtual void release() = 0;
	virtual void escape() = 0;
	void setRotDynamic(float d) { _rot_dynamic = d; }
	float getRotDynamic() const { return _rot_dynamic; }

	void attach();
	void detach();

private:
	float _rot_dynamic = 2.f; // optional rotary accelerator.
};


class ESPRotary final : public Clock_I
{

public:
	static constexpr const int DEFAULT_LONG_PRESS_THRESHOLD = 400;

	ESPRotary(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw);
	~ESPRotary();
	void begin();
	void stop();
	esp_err_t updateRotDir();
	void updateIncrement(int inc);
	void setLongPressTimeout(int lptime_ms) { lp_duration = lptime_ms/10; }

	// observer feed
	void sendRot(int diff) const;
	void sendPress() const;
	void sendRelease() const;
	void sendLongPress() const;
	void sendEscape() const;
	// gpio's
	bool readSwitch() const; // safe to call in rotary callback
	bool readBootupStatus() const { return gpio_get_level(_sw) == 0; }
	gpio_num_t getSw() const { return _sw; };
	gpio_num_t getClk() const { return clk; };
	gpio_num_t getDt() const { return dt; };

	// polling state machine
	bool tick() override;

private:
	gpio_num_t clk, dt, _sw; // actually used pins
	pcnt_unit_handle_t pcnt_unit = nullptr;
	pcnt_channel_handle_t pcnt_chan = nullptr;
	int lp_duration = DEFAULT_LONG_PRESS_THRESHOLD/10; // default 400msec
	bool state = false;
	int holdCount = 0; // timer timeout set the hold state
	bool lastButtonRead = false;
	int debounceCount = 0;
	int increment = 1;
};

extern ESPRotary *Rotary;

