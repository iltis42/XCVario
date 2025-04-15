/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "SetupRoot.h"
#include "Blackboard.h"
#include "IpsDisplay.h"

#include "sensor.h"
#include "logdef.h"


static int screen_index = 0;
static uint8_t screen_mask_len = 1;

static void init_screens()
{
    uint32_t scr = menu_screens.get();
    screen_gmeter.set((scr >> SCREEN_GMETER) & 1);
    // 	screen_centeraid.set( (scr >> SCREEN_THERMAL_ASSISTANT) & 1);
    screen_horizon.set((scr >> SCREEN_HORIZON) & 1);
    screen_mask_len = 1; // default vario
    while (scr)
    {
        scr = scr >> 1;
        screen_mask_len++;
    };
    ESP_LOGI(FNAME, "screens mask len: %d, screens: %d", screen_mask_len, menu_screens.get());
}

// static void initGearWarning()
// {
// 	gpio_num_t io = SetupMenu::getGearWarningIO();
// 	if (io != GPIO_NUM_0) {
// 		gpio_reset_pin(io);
// 		gpio_set_direction(io, GPIO_MODE_INPUT);
// 		gpio_set_pull_mode(io, GPIO_PULLUP_ONLY);
// 		gpio_pullup_en(io);
// 	}ESP_LOGI(FNAME,"initGearWarning: IO: %d", io );
// }

SetupRoot::SetupRoot(IpsDisplay *display) : SetupMenu("Setup Root", nullptr)
{
    _display = display;
    ESP_LOGI(FNAME,"Init root menu");
    // selected = this;
    attach();
}

SetupRoot::~SetupRoot()
{
    detach();
}

void SetupRoot::begin(MenuEntry *setup)
{
    ESP_LOGI(FNAME,"SetupMenu %s", _title);
    // root will always own only one child
    if ( !_childs.empty() ) {
        ESP_LOGW(FNAME,"Abort, found root menu not empty.");
        delete _childs.front();
        _childs.clear();
    }

    if ( setup ) {
        addEntry(setup);
    } else {
        addEntry(SetupMenu::createTopSetup());
    }

    gflags.inSetup = true;
    _display->doMenu(true);
    delay(200); // fixme give display task time to finish drawing

    audio_volume.set(default_volume.get());
    init_screens();

    _childs.front()->enter();

    // initGearWarning();
}

void SetupRoot::exit(int levels)
{
    ESP_LOGI(FNAME,"End Setup Menu");

    screens_init = INIT_DISPLAY_NULL;
    _display->doMenu(false);
    if (_restart) {
        reBoot();
    }
    delete _childs.front();
    _childs.clear();
    gflags.inSetup = false;
    _display->doMenu(false);
}

void SetupRoot::rot(int count)
{
    // ESP_LOGI(FNAME,"root: rot");
    if (rot_default.get() == 1) {
        // MC Value
        float mc = MC.get();
        float step = Units::Vario2ms(0.1);
        mc += step * count;
        if (mc > 9.9) {
            mc = 9.9;
        }
        else if (mc < 0.0) {
            mc = 0.0;
        }
        MC.set(mc);
    }
    else {
        // Volume
        float vol = audio_volume.get();
        if (vol < 1.5) {
            vol = 1.5;
        }
        vol = vol * std::powf(1.33f, count);
        if (vol > max_volume.get()) {
            vol = max_volume.get();
        }
        else if (vol < 1.5) {
            vol = 1.5;
        }
        audio_volume.set(vol);
    }
}

void SetupRoot::press()
{
    ESP_LOGI(FNAME,"root press active_srceen %d (%x)", active_screen, menu_screens.get());

	if (!gflags.inSetup) {
		active_screen = 0;
		while (active_screen == 0 && (screen_index <= screen_mask_len)) {
			if (menu_screens.get() & (1 << screen_index)) {
				active_screen = screen_index;
				ESP_LOGI(FNAME,"New active_screen: %d", active_screen );
			}
			screen_index++;
		}
		if (screen_index > screen_mask_len) {
			ESP_LOGI(FNAME,"select vario screen");
			screen_index = 0;
			active_screen = 0; // fall back into default vario screen after optional screens
		}
	}
    if (!menu_long_press.get() && active_screen == 0 && !gflags.inSetup) {
        begin();
    }
}

void SetupRoot::longPress()
{
    if (menu_long_press.get() && !gflags.inSetup) {
        begin();
    }
}

void SetupRoot::escape()
{
    if (gflags.inSetup) {
		ESP_LOGI(FNAME,"escape now Setup Menu - really, need to unroll");
		// _display->clear();
		// _display->doMenu(false);
		// gflags.inSetup = false;
	}
}
