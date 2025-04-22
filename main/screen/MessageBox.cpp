/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "MessageBox.h"

#include "protocol/Clock.h"
#include "sensor.h"

MessageBox *MBOX; // the global representation

struct Message {
    int alert_level;
    std::string text;
    Message(int a, const char *str) : alert_level(a), text(str) {}
};

void MessageBox::createMessageBox()
{
    if ( ! MBOX ) {
        MBOX = new MessageBox();
    }
}

MessageBox::MessageBox() :
    Clock_I(2),
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
    std::unique_ptr<Message> msg = std::make_unique<Message>(alert_level, str);
    if ( alert_level == 3 ) {
        _msg_list.push_front(std::move(msg));
    }
    else {
        _msg_list.push_back(std::move(msg));
    }
    if ( ! current ) { Clock::start(this); }
}

bool MessageBox::nextMsg()
{
    bool start_timer = (current == nullptr);

    // clear potential old  message
    MYUCG->undoClipRange();
    removeMsg();

    if (!_msg_list.empty()) {
        current = std::move(_msg_list.front());
        _msg_list.pop_front();
    }
    else {
        Display->redrawValues();
        return false;
    }

    // prepare screen 
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
    // MYUCG->setFont(ucg_font_fub35_hr);
    MYUCG->setFont(ucg_font_ncenR14_hr);
    MYUCG->setPrintPos(1, height-2);
    MYUCG->setColor(COLOR_WHITE);
    MYUCG->setFontPosBottom();
    MYUCG->print(current->text.c_str());

    // Exclude the message area for the rest of the system
    MYUCG->setClipRange(0, 0, width, height - 27);

    // set time counter
    _start_scroll = 0;
    _nr_scroll = std::max(MYUCG->getStrWidth(current->text.c_str()) - width +2, 0);
    _msg_to = current->alert_level * 10 * (1000/(2*Clock::TICK_ATOM)); // x 10 sec
    _print_pos = 0;

    // start the timer
    if ( start_timer ) {
        Clock::start(this);
    }
    return true;
}

void MessageBox::removeMsg()
{
    MYUCG->setColor(COLOR_BLACK);
    MYUCG->drawBox(0, height - 26, width, height);
}

bool MessageBox::tick()
{
    if ( _msg_to <= 0 ) {
        if ( ! nextMsg() ) {
            return true;
        }
    }
    _msg_to--;

    // move text to expose the full message
    _start_scroll++;
    if (_start_scroll > (4000/(2*Clock::TICK_ATOM)) && _nr_scroll > 0) {
        _nr_scroll--;
        _print_pos--;
        MYUCG->undoClipRange();
        MYUCG->setPrintPos(_print_pos, height-2);
        MYUCG->print(current->text.c_str());
        MYUCG->setClipRange(0, 0, width, height - 27);
    }
    return false;
}
