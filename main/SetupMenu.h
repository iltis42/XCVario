/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _SetupMenu_H_
#define _SetupMenu_H_
#include "MenuEntry.h"
#include <string>
#include <driver/gpio.h>
#include "SetupMenuValFloat.h"

class IpsDisplay;
class ESPRotary;
class PressureSensor;
class AnalogInput;

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	SetupMenu( const char* title );
	virtual ~SetupMenu();
	void begin( IpsDisplay* display, PressureSensor * bmp, AnalogInput *adc );
	void setup();
	void display( int mode=0 );
	const char *value() { return 0; };
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	void escape();
	void showMenu();
	void create_subtree();
	void delete_subtree();


	static void catchFocus( bool activate );
	static bool focus;
	static gpio_num_t getGearWarningIO();

	static void setup_create_root(SetupMenu *top);

	static void wiper_menu_create(SetupMenu *top);
	static void bugs_item_create(SetupMenu *top);
	static void vario_menu_create(SetupMenu *top);
	static void vario_menu_create_damping(SetupMenu *top);
	static void vario_menu_create_meanclimb(SetupMenu *top);
	static void vario_menu_create_s2f(SetupMenu *top);
	static void vario_menu_create_ec(SetupMenu *top);

	static void audio_menu_create(SetupMenu *top);
	static void audio_menu_create_volume(SetupMenu *top);
	static void audio_menu_create_tonestyles(SetupMenu *top);
	static void audio_menu_create_deadbands(SetupMenu *top);
	static void audio_menu_create_equalizer(SetupMenu *top);
	static void audio_menu_create_mute(SetupMenu *top);

	static void glider_menu_create(SetupMenu *top);
	static void glider_menu_create_polarpoints(SetupMenu *top);

	static void options_menu_create(SetupMenu *top);
	static void options_menu_create_units(SetupMenu *top);
	static void options_menu_create_flarm(SetupMenu *top);
	static void options_menu_create_compasswind(SetupMenu *top);
	static void options_menu_create_compasswind_compass(SetupMenu *top);
	static void options_menu_create_compasswind_compass_dev(SetupMenu *top);
	static void options_menu_create_compasswind_compass_nmea(SetupMenu *top);
	static void options_menu_create_compasswind_straightwind(SetupMenu *top);
	static void options_menu_create_compasswind_straightwind_limits(SetupMenu *top);
	static void options_menu_create_compasswind_straightwind_filters(SetupMenu *top);
	static void options_menu_create_compasswind_circlingwind(SetupMenu *top);
	static void options_menu_create_wireless(SetupMenu *top);
	static void options_menu_create_wireless_custom_id(SetupMenu *top);
	static void options_menu_create_wireless_routing(SetupMenu *top);
	static void options_menu_create_gload(SetupMenu *top);

	static void system_menu_create(SetupMenu *top);
	static void system_menu_create_software(SetupMenu *top);
	static void system_menu_create_battery(SetupMenu *top);
	static void system_menu_create_hardware(SetupMenu *top);
	static void system_menu_create_altimeter_airspeed(SetupMenu *top);
	static void system_menu_create_altimeter_airspeed_stallwa(SetupMenu *top);
	static void system_menu_create_interfaceS1(SetupMenu *top);
	static void system_menu_create_interfaceS1_routing(SetupMenu *top);
	static void system_menu_create_interfaceS2(SetupMenu *top);
	static void system_menu_create_interfaceS2_routing(SetupMenu *top);
	static void system_menu_create_interfaceCAN(SetupMenu *top);
	static void system_menu_create_interfaceCAN_routing(SetupMenu *top);
	static void system_menu_create_hardware_type(SetupMenu *top);
	static void system_menu_create_hardware_rotary(SetupMenu *top);
	static void system_menu_create_hardware_rotary_screens(SetupMenu *top);
	static void system_menu_create_hardware_ahrs(SetupMenu *top);
	static void system_menu_create_ahrs_calib(SetupMenu *top);
	static void system_menu_create_hardware_ahrs_lc(SetupMenu *top);
	static void system_menu_create_hardware_ahrs_parameter(SetupMenu *top);
	static SetupMenuValFloat * createQNHMenu();
};

#endif
