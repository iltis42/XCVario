/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "MessageBox.h"

#include "Colors.h"
#include "DrawDisplay.h"
#include "UiEvents.h"
#include "protocol/Clock.h"
#include "logdefnone.h"
#include "sensor.h"

#include <mutex>

MessageBox *MBOX; // the global representation

const int CLOCK_DIVIDER = 8;

// A message is represented throught
// - alert level (1,2,3,4)
// - and a text message

void MessageBox::createMessageBox()
{
    if ( ! MBOX ) {
        MBOX = new MessageBox();
    }
}

MessageBox::MessageBox() :
    Clock_I(CLOCK_DIVIDER),
    width(MYUCG->getDisplayWidth()),
    height(MYUCG->getDisplayHeight())
{
    // _list_mutex = SemaphoreMutex();
}

MessageBox::~MessageBox()
{
    Clock::stop(this);
    _msg_list.clear();
}

void MessageBox::newMessage(int alert_level, const char *str, int to)
{
    std::unique_ptr<ScreenMsg> msg = std::make_unique<ScreenMsg>(alert_level, str, to);
    {
        std::lock_guard<SemaphoreMutex> lock(_list_mutex);
        if ( alert_level >= 3 ) {
            _msg_list.push_front(std::move(msg));
        }
        else {
            _msg_list.push_back(std::move(msg));
        }
    }
    
    if ( alert_level == 4 ) {
        _msg_to = 0; // trigger immediate display
    }
    if ( ! current ) {
        Clock::start(this);
    }
}

void MessageBox::recallAlarm()
{
    if ( current ) {
        _msg_to = 0; // trigger immediate display of next message
    }
}

// returns true in case a message is now on the display
bool MessageBox::nextMsg()
{
    // clear message area
    MYUCG->undoClipRange();
    removeMsg();

    {
        std::lock_guard<SemaphoreMutex> lock(_list_mutex);
        if (!_msg_list.empty()) {
            current = std::move(_msg_list.front());
            _msg_list.pop_front();
        }
        else {
            // All done
            current = nullptr;
            return false;
        }
    }

    // prepare screen with a colored line on top
    if ( current->alert_level == 1 ) {
        MYUCG->setColor(COLOR_BLUE);
    }
    else if ( current->alert_level == 2 ) {
        MYUCG->setColor(COLOR_ORANGE);
    }
    else if ( current->alert_level >= 3 ) {
        MYUCG->setColor(COLOR_RED);
    }
    MYUCG->drawHLine(0, height - 26, width);
    MYUCG->setFont(ucg_font_ncenR14_hr);
    _print_pos = 1;
    if  ( current->alert_level == 4 ) {
        // center text
        int w = MYUCG->getStrWidth(current->text.c_str());
        if ( w < width ) {
            _print_pos = (width - w) / 2;
        }
    }
    MYUCG->setPrintPos(_print_pos, height-2);
    MYUCG->setColor(COLOR_WHITE);
    MYUCG->setFontPosBottom();

    MYUCG->print(current->text.c_str());

    // Exclude the message area for the rest of the system
    MYUCG->setClipRange(0, 0, width, height - 27);

    // set time counter
    _start_scroll = 0;
    _nr_scroll = std::max(MYUCG->getStrWidth(current->text.c_str()) - width +2, 0);
    _msg_to = (current->_to > 0) ? current->_to :  current->alert_level * 10; // x 10 sec
    _msg_to *= (1000/(CLOCK_DIVIDER*Clock::TICK_ATOM)); // convert to ticks

    return true;
}

void MessageBox::removeMsg()
{
    MYUCG->setColor(COLOR_BLACK);
    MYUCG->drawBox(0, height - 26, width, 26);
}

// drive and draw the message box
// only call when there are messages queued
// returns true when the message box has finished
bool MessageBox::draw()
{
    ESP_LOGI(FNAME, "draw message");
    _msg_queued = false; // a 1:1 request quing
    if ( _msg_to <= 0 ) {
        if ( ! nextMsg() ) {
            Clock::stop(this);
            ESP_LOGI(FNAME, "message stop");
            return true;
        }
    }
    _msg_to--;

    _start_scroll++;
    if ( current->alert_level != 4 ) {
        // move text to expose the full message
        if (_start_scroll > (4000/(CLOCK_DIVIDER*Clock::TICK_ATOM)) && _nr_scroll > 0) {
            _nr_scroll--;
            _print_pos--;
            MYUCG->undoClipRange();
            MYUCG->setFont(ucg_font_ncenR14_hr);
            MYUCG->setColor(COLOR_WHITE);
            MYUCG->setFontPosBottom();
            MYUCG->setPrintPos(_print_pos, height-2);
            MYUCG->print(current->text.c_str());
            MYUCG->setClipRange(0, 0, width, height - 27);
        }
    }
    else {
        // flash the message
        if ( (_start_scroll % 2) == 0 ) {
            MYUCG->undoClipRange();
            if ( _start_scroll & 0x2) {
                MYUCG->setColor(COLOR_RED);
            }
            else {
                MYUCG->setColor(COLOR_WHITE);
            }
            MYUCG->setFont(ucg_font_ncenR14_hr);
            MYUCG->setFontPosBottom();
            MYUCG->setPrintPos(_print_pos, height-2);
            MYUCG->print(current->text.c_str());
            MYUCG->setClipRange(0, 0, width, height - 27);
        }
    }
    return false;
}

bool MessageBox::tick()
{
    if ( !_msg_queued ) {
        int evt = ScreenEvent(ScreenEvent::MSG_BOX).raw;
        xQueueSend(uiEventQueue, &evt, 0);
        _msg_queued = true;
    }
    return false;
}
