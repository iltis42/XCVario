/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "ShowBootMsg.h"

#include "setup/SetupNG.h"
#include "sensor.h"
#include "logdef.h"

#include <sstream>

ShowBootMsg::ShowBootMsg( const char* title ) :
    SetupMenuDisplay( title, nullptr )
{
}

void ShowBootMsg::display(int mode)
{
    clear();

    const int line_height = 20;
    int ln = line_height;
    MYUCG->setFont(ucg_font_fub11_tr);

    std::istringstream stream(logged_tests);
    std::string line;
    while (std::getline(stream, line)) {
        MYUCG->setPrintPos(0, ln);
        MYUCG->print(line.c_str());
        ln += line_height;
    }
    MYUCG->setPrintPos(20, ln+line_height);
    MYUCG->print("Press button to exit");
}
