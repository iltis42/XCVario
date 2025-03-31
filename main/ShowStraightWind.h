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

	ShowStraightWind( const char* title );

	virtual ~ShowStraightWind() {}

	void display(int mode=0) override;
	void up( int count ) override { display(5); };
	void down( int count ) override { display(5); };
};
