/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: SetupMenuDisplay.h

Generic class to display text in a menu item.

Author: Axel Pauli, February 2021

Last update: 2021-02-25

 ****************************************************************************/

#pragma once

#include "MenuEntry.h"


class SetupMenuDisplay: public MenuEntry
{
public:
  SetupMenuDisplay( const char* title, int (*action)(SetupMenuDisplay *p) = nullptr );

  virtual ~SetupMenuDisplay();

  /**
   * Make a class derive and overload this method with your own display method
   * or handle all display stuff in your callback function action.
   */
	virtual void display( int mode=0 );

	// No value support
	virtual const char *value() { return nullptr; }

	// Ignore up calls
	virtual void up( int count ) {}

	// Ignore down calls
	virtual void down( int count ) {}

	// Handle press calls
	virtual void press();

	// Ignore release calls
	virtual void release() {}

	// Ignore escape calls
	void escape() {};

private:
	// User's callback function
	int (*_action)( SetupMenuDisplay *p );
};
