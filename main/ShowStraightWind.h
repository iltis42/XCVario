/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: ShowStraightWind.h

Class to display the circling wind status

Author: Eckhard Völlm, May 2021


 ****************************************************************************/

#pragma once

#include "SetupMenuDisplay.h"

class ShowStraightWind: public SetupMenuDisplay
{
public:

	ShowStraightWind( std::string title );

	virtual ~ShowStraightWind() {}

	virtual void display( int mode=0 );
	virtual void up( int count ) { display(5); };
	virtual void down( int count ) { display(5); };
};
