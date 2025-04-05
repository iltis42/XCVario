/////////////////////////////////////////////////////////////////
/*
  Library for reading rotary encoder values using Observer Pattern, and GPIO Interrups
*/

#pragma once

#include <driver/pulse_cnt.h>
#include <esp_timer.h>
#include <driver/gpio.h>


// use this to build an event infrastructure
constexpr const int SHORT_PRESS     = 1;
constexpr const int LONG_PRESS      = 2;
constexpr const int BUTTON_RELEASED = 3;
constexpr const int ROTARY_EVTMASK  = 0xf0;

union KnobEvent {
	struct {
		int ButtonEvent : 4; // 1,2,3
		int RotaryEvent : 4; // -3,-2,-1, 1,2,3
	};
	int raw;
	KnobEvent() = default;
	constexpr KnobEvent(const int v) : raw(v) {}
};


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


class ESPRotary
{
private:
	friend void button_isr_handler(void* arg);
	friend void longpress_timeout(void *arg);
	friend void ObserverTask(void *arg);

public:
	static constexpr const int DEFAULT_LONG_PRESS_THRESHOLD = 400;

	ESPRotary(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw);
	virtual ~ESPRotary() = default;
	void begin();
	esp_err_t updateRotDir();
	void setLongPressTimeout(int lptime_ms) { lp_duration = (uint64_t)1000 * lptime_ms; }

	// observer feed
	void sendRot(int diff) const;
	void sendPress() const;
	void sendRelease() const;
	void sendLongPress() const;
	void sendEscape() const;
	bool readSwitch() const { return state; } // fixme, polling does create a pile on the event queue
	gpio_num_t getSw() { return sw; };

private:
	gpio_num_t clk, dt, sw; // actually used pins
	pcnt_unit_handle_t pcnt_unit = nullptr;
	pcnt_channel_handle_t pcnt_chan = nullptr;
	esp_timer_handle_t lp_timer = nullptr;
	uint64_t lp_duration = DEFAULT_LONG_PRESS_THRESHOLD * 1000; // default 400msec
	bool state = false;
	bool hold = false; // timer timeout set the hold state
};

extern ESPRotary *Rotary;

