/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: ShowCompassSettings.h

Class to display the compass settings overview.

Author: Axel Pauli, March 2021

Last update: 2021-03-08

 ****************************************************************************/

#pragma once

#include "SetupMenuDisplay.h"

class ShowCompassSettings: public SetupMenuDisplay
{
  public:

  ShowCompassSettings( String title );

  virtual ~ShowCompassSettings() {}

	virtual void display( int mode=0 );
};
