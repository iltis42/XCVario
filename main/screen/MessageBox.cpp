/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "MessageBox.h"

#include "DrawDisplay.h"
#include "UiEvents.h"
#include "protocol/Clock.h"
#include "sensor.h"

MessageBox *MBOX; // the global representation

const int CLOCK_DIVIDER = 4;

// A message is represented throught 
// - alert level (1,2,3)
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
}

MessageBox::~MessageBox()
{
    Clock::stop(this);
    _msg_list.clear();
}

void MessageBox::newMessage(int alert_level, const char *str)
{
    std::unique_ptr<ScreenMsg> msg = std::make_unique<ScreenMsg>(alert_level, str);
    if ( alert_level == 3 ) {
        _msg_list.push_front(std::move(msg));
    }
    else {
        _msg_list.push_back(std::move(msg));
    }
    if ( ! current ) {
        nextMsg();
        Clock::start(this);
    }
}

// returns true in case a message is now on the display
bool MessageBox::nextMsg()
{
    // clear message area
    xSemaphoreTake(display_mutex,portMAX_DELAY);
    MYUCG->undoClipRange();
    removeMsg();

    if (!_msg_list.empty()) {
        current = std::move(_msg_list.front());
        _msg_list.pop_front();
    }
    else {
        // All done
        xSemaphoreGive(display_mutex);
        current = nullptr;
        Display->redrawValues(); // fixme
        return false;
    }

    // prepare screen with a colored line on top
    if ( current->alert_level == 1 ) {
        MYUCG->setColor(COLOR_BLUE);
    }
    else if ( current->alert_level == 2 ) {
        MYUCG->setColor(COLOR_ORANGE);
    }
    else if ( current->alert_level == 3 ) {
        MYUCG->setColor(COLOR_RED);
    }
    MYUCG->drawHLine(0, height - 26, width);
    MYUCG->setFont(ucg_font_ncenR14_hr);
    MYUCG->setPrintPos(1, height-2);
    MYUCG->setColor(COLOR_WHITE);
    MYUCG->setFontPosBottom();
    MYUCG->print(current->text.c_str());

    // Exclude the message area for the rest of the system
    MYUCG->setClipRange(0, 0, width, height - 27);
    xSemaphoreGive(display_mutex);

    // set time counter
    _start_scroll = 0;
    _nr_scroll = std::max(MYUCG->getStrWidth(current->text.c_str()) - width +2, 0);
    _msg_to = current->alert_level * 10 * (1000/(CLOCK_DIVIDER*Clock::TICK_ATOM)); // x 10 sec
    _print_pos = 0;

    return true;
}

void MessageBox::removeMsg()
{
    MYUCG->setColor(COLOR_BLACK);
    MYUCG->drawBox(0, height - 26, width, 26);
}

// drive and draw the message box
// only call when there are messages queued
// returns true when the message box is finished
bool MessageBox::draw()
{
    if ( _msg_to <= 0 ) {
        if ( ! nextMsg() ) {
            Clock::stop(this);
            return true;
        }
    }
    _msg_to--;

    // move text to expose the full message
    _start_scroll++;
    if (_start_scroll > (4000/(CLOCK_DIVIDER*Clock::TICK_ATOM)) && _nr_scroll > 0) {
        _nr_scroll--;
        _print_pos--;
        xSemaphoreTake(display_mutex,portMAX_DELAY);
        MYUCG->undoClipRange();
        MYUCG->setFont(ucg_font_ncenR14_hr);
        MYUCG->setColor(COLOR_WHITE);
        MYUCG->setFontPosBottom();
        MYUCG->setPrintPos(_print_pos, height-2);
        MYUCG->print(current->text.c_str());
        MYUCG->setClipRange(0, 0, width, height - 27);
        xSemaphoreGive(display_mutex);
    }
    return false;
}

bool MessageBox::tick()
{
    int evt = ScreenEvent(ScreenEvent::MSG_BOX).raw;
    xQueueSend(uiEventQueue, &evt, 0);
    return false;
}

