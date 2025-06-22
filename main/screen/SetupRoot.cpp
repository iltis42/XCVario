/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "SetupRoot.h"

#include "DrawDisplay.h"
#include "UiEvents.h"
#include "setup/SubMenuDevices.h"
#include "IpsDisplay.h"

#include "sensor.h"
#include "logdef.h"

// bit field of all configured screens
// set to zero for boot-up
static uint32_t all_screens;

SetupRoot::SetupRoot(IpsDisplay *display) :
    SetupMenu("Setup Root", nullptr),
    _ui_mon_wd(this)
{
    _display = display;
    ESP_LOGI(FNAME,"Init root menu");
    attach();
}

SetupRoot::~SetupRoot()
{
    detach();
}

// time-out on ui interaction while airborne
void SetupRoot::barked()
{
    int exitMenu = ButtonEvent(ButtonEvent::ESCAPE).raw;
    xQueueSend(uiEventQueue, &exitMenu, 0);
}

void SetupRoot::initScreens()
{
    all_screens = 0;
    if ( screen_gmeter.get() ) {
        all_screens |= SCREEN_GMETER;
    }
    if ( screen_horizon.get() ) {
        all_screens |= SCREEN_HORIZON;
    }
    all_screens |= SCREEN_VARIO; // always
}

void SetupRoot::begin(MenuEntry *setup)
{
    ESP_LOGI(FNAME,"SetupMenu %s", _title.c_str());
    // root will always own only one child
    if ( !_childs.empty() ) {
        ESP_LOGW(FNAME,"Found root menu not empty.");
        delete _childs.front();
        _childs.clear();
    }

    // Given setup might be for QNH, or voltage adjustment
    if ( setup ) {
        addEntry(setup);
    } else {
        addEntry(SetupMenu::createTopSetup());
        if ( airborne.get() ) {
            // exit setup after timeout w/o user activity
            _ui_mon_wd.start(16000); // 16 seconds
            uiMonitor = &_ui_mon_wd;
        }
    }

    gflags.inSetup = true;

    _childs.front()->enter();

    SetupMenu::initGearWarning();
}

void SetupRoot::exit(int levels)
{
    ESP_LOGI(FNAME,"End Setup Menu");
    if ( uiMonitor ) {
        uiMonitor->stop();
        uiMonitor = nullptr;
    }
    free_connected_devices_menu();

    screens_init = INIT_DISPLAY_NULL;
    if (_restart) {
        reBoot();
    }
    delete _childs.front(); // hook to the entire setup tree
    _childs.clear();
    gflags.inSetup = false;
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
    ESP_LOGI(FNAME,"root press active_srceen %d (0x%x)", active_screen, (unsigned)all_screens);
    if ( active_screen == NO_SCREEN ) {
        active_screen = SCREEN_VARIO;
    }

    // cycle through screens, incl. setup
    if (!gflags.inSetup)
    {
        while (active_screen < SCREEN_THERMAL_ASSISTANT)
        {
            active_screen <<= 1;
            if (all_screens & active_screen)
            {
                ESP_LOGI(FNAME, "New active_screen: %x", active_screen);
                break;
            }
        }
        if ( active_screen >= SCREEN_THERMAL_ASSISTANT ) {
            ESP_LOGI(FNAME, "select vario screen");
            active_screen = SCREEN_VARIO;
        }
    }
    // only if menu long press is not set, jump into the setup befor going back to the vario screen
    if (!menu_long_press.get() && active_screen == SCREEN_VARIO && !gflags.inSetup)
    {
        begin();
    }
}

void SetupRoot::longPress()
{
    // enter setup fron any screen
    if (!gflags.inSetup) {
        begin();
    }
}
