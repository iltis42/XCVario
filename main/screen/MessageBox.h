/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/ClockIntf.h"

#include <queue>
#include <string>
#include <memory>

// print text into the bottom line of the screen
// move the message, so that it is readable as a whole
// clip the message line from the current screen
//
// alarm levels are 1, 2, 3
// 1: stays for 10 seconds visible, blue line on top
// 2: stays for 20 seconds visible, orange line
// 3: stays for 30 seconds visible, red line
//
// precondition:
// - ucg adapter for the connected display
// - Clock timer
// - screens it interfers with

class Message;

class MessageBox final : public Clock_I
{
    using MessagePtr = std::unique_ptr<Message>;

public:
    static void createMessageBox();
    ~MessageBox();

    // API
    void newMessage(int alert_level, const char* msg);
    bool isVisible() const { return current != nullptr; }

    // Clock tick callback
    bool tick() override;

private:
    // helper
    MessageBox();
    bool nextMsg();
    void removeMsg();

private:
    MessagePtr current = nullptr;
    std::deque<MessagePtr> _msg_list;
    const int width;
    const int height;
    int _print_pos = 0;
    int _start_scroll = 0;
    int _nr_scroll = 0;
    int _msg_to = 0;
};

// exposed pointer to the message box
extern MessageBox *MBOX;