/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: SetupMenuDisplay.cpp

Generic class to display text in a menu item.

Author: Axel Pauli, February 2021

Last update: 2021-02-25

 ****************************************************************************/
#include "esp_log.h"
#include "esp_system.h"

#include "IpsDisplay.h"
#include "SetupMenuDisplay.h"
#include "sensor.h"

SetupMenuDisplay::SetupMenuDisplay( String title,
                                    int (*action)(SetupMenuDisplay *p) ) :
  MenuEntry()
{
	_rotary->attach( this );
	_title = title;
	_action = action;
}
SetupMenuDisplay::~SetupMenuDisplay()
{
    _rotary->detach(this);
}

void SetupMenuDisplay::display( int mode )
{
  if( (selected != this) || !inSetup )
    return;

  if( _action != nullptr ) {
    // Call user's callback
    (*_action)( this );
  }
}

void SetupMenuDisplay::press()
{
	if( selected != this )
		return;

  ESP_LOGI(FNAME,"press(): pressed=%d", pressed);

	if( pressed )
    {
	    // called during menu exit press
      if( _parent != 0 )
        selected = _parent;

      selected->highlight = -1;  // to topmost selection when back
      selected->pressed = true;
      pressed = false;
    }
  else
    {
      // called during menu entry press
      pressed = true;
      display();
    }
}
