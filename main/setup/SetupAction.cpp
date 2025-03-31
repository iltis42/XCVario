/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "SetupAction.h"
#include "SetupMenu.h"
#include "sensor.h"

#include "logdef.h"

constexpr const char action_help[] = "Long Press to exit action control.";

SetupAction::SetupAction(const char *title, int (*action)(SetupAction *), int code, bool end_menu) :
    MenuEntry(),
    _action(action),
    _code(code),
    _end_menu(end_menu)
{
    ESP_LOGI(FNAME, "SetupAction( %s ) ", title);
    _title = title;
    helptext = action_help;
}

void SetupAction::enter()
{
    (*_action)(this);
    if ( !helptext ) {
        return; // do not enter at all, just one action
    }
    MenuEntry::enter();
}

void SetupAction::exit(int ups)
{
    MenuEntry::exit(ups+_extra_ups);
}

void SetupAction::display(int mode)
{
    ESP_LOGI(FNAME, "display() instance=%p mode=%d", this, mode);
    int line = _parent->getMenuPos()+1;
    int indent = MYUCG->getStrWidth(_title) + 12;
    MYUCG->setPrintPos(indent, (line + 1) * 25);
    MYUCG->printf( " %3d  ", mode );
    showhelp();
}

void SetupAction::press()
{
    ESP_LOGI(FNAME, "press() instance=%p", this);
    (*_action)(this);
}

void SetupAction::longPress()
{
    if ( _end_menu ) {
        _parent->menuSetTop();
    }
    exit();
}
