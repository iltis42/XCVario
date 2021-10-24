/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: DisplayDeviations.h

Class to display the compass deviations overview.

Author: Axel Pauli, February 2021

Last update: 2021-02-25

 ****************************************************************************/

#pragma once

#include "SetupMenuDisplay.h"

class DisplayDeviations: public SetupMenuDisplay
{
  public:

  DisplayDeviations( std::string title );

  virtual ~DisplayDeviations() {}

	virtual void display( int mode=0 );
};
