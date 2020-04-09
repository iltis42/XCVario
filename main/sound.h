#ifndef SOUND_H
#define SOUND_H

#include "mcp4018.h"

typedef enum _sound { DING, HI } e_sound;

class Sound
{
public:
	Sound();
	static void setPoti( MCP4018 *poti ) { _poti = poti; };
	static void playSound( e_sound a_sound, bool end=false );

private:
	static void timer_isr(void* arg);
	static void timerInitialise (int timer_period_us);
	static int pos;
	static bool ready;
	static MCP4018 *_poti;
	static e_sound sound;
};

#endif
