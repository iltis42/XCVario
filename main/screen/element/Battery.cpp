/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "Battery.h"

#include "Colors.h"
#include "setup/SetupNG.h"
#include "Units.h"
#include "AdaptUGC.h"

#include <cstdio>

extern AdaptUGC *MYUCG;

Battery::Battery(int16_t cx, int16_t cy) :
    ScreenElement(cx, cy)
{
    setThresholds();
}

void Battery::setThresholds()
{
    _yellow =  (int)(( bat_yellow_volt.get() - bat_low_volt.get() )*100)/( bat_full_volt.get() - bat_low_volt.get() );
    _red = (int)(( bat_red_volt.get() - bat_low_volt.get() )*100)/( bat_full_volt.get() - bat_low_volt.get() );
}

void Battery::blank()
{
    if( battery_display.get() != BAT_VOLTAGE_BIG ){
        MYUCG->setColor( COLOR_BLACK );
        MYUCG->drawBox( _ref_x-40, _ref_y-2, 40, 12  );
    } else {
        MYUCG->setColor( COLOR_BLACK );
        MYUCG->drawBox( _ref_x-55, _ref_y-12, 65, 22  );
    }
    _dirty = true;
}


void Battery::draw(float volt)
{
	if ( volt < bat_red_volt.get() && ! _dirty ) {
		blank();
        return;
	}
	int chargev = (int)(volt*10);
	if ( _charge == chargev && ! _dirty  ) {
        return;
	}
    _charge = chargev;
    _dirty = false;

    int chargep = (int)((volt - bat_low_volt.get())*100)/( bat_full_volt.get() - bat_low_volt.get());
    if(chargep < 0)
        chargep = 0;
    if( chargep > 100 )
        chargep = 100;
    if ( battery_display.get() != BAT_VOLTAGE_BIG ){
        MYUCG->setColor( COLOR_HEADER );
        MYUCG->drawBox( _ref_x-40,_ref_y-2, 36, 12  );  // Bat body square
        MYUCG->drawBox( _ref_x-4, _ref_y+1, 3, 6  );      // Bat pluspole pimple
        if ( chargep > _yellow )  // >25% grün
            MYUCG->setColor( COLOR_GREEN ); // green
        else if ( chargep < _yellow && chargep > _red )
            MYUCG->setColor( COLOR_YELLOW ); //  yellow
        else if ( chargep < _red )
            MYUCG->setColor( COLOR_RED ); // red
        else
            MYUCG->setColor( COLOR_RED ); // red
        int chgpos=(chargep*32)/100;
        if(chgpos <= 4)
            chgpos = 4;
        MYUCG->drawBox( _ref_x-40+2, _ref_y, chgpos, 8  );  // Bat charge state
        MYUCG->setColor( DARK_GREY );
        MYUCG->drawBox( _ref_x-40+2+chgpos, _ref_y, 32-chgpos, 8 );  // Empty bat bar
        MYUCG->setFont(ucg_font_fub11_hr, true);
        MYUCG->setPrintPos(_ref_x-42, _ref_y-6);
    }
    MYUCG->setColor( COLOR_WHITE );
    if( battery_display.get() == BAT_PERCENTAGE ) {
        MYUCG->printf("%3d", chargep);
        MYUCG->setColor( COLOR_HEADER );
        MYUCG->print("% ");
    }
    else if ( battery_display.get() == BAT_VOLTAGE ) {
        // MYUCG->setPrintPos(x-40,y-8);
        MYUCG->printf("%2.1f", volt);
        MYUCG->setColor( COLOR_HEADER );
        MYUCG->print("V ");
    }
    else if ( battery_display.get() == BAT_VOLTAGE_BIG ) {
        MYUCG->setPrintPos(_ref_x-50, _ref_y+11);
        MYUCG->setFont(ucg_font_fub14_hr, true);
        MYUCG->printf("%2.1f", volt);
        MYUCG->setColor( COLOR_HEADER );
        MYUCG->print("V ");
    }
}