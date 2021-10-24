/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: ShowCirclingWind.h

Class to display the circling wind status

Author: Eckhard Völlm, May 2021


 ****************************************************************************/

#pragma once

#include "SetupMenuDisplay.h"

class ShowCirclingWind: public SetupMenuDisplay
{
public:

	ShowCirclingWind( std::string title );

	virtual ~ShowCirclingWind() {}

	virtual void display( int mode=0 );
	virtual void up( int count ) { display(5); };
	virtual void down( int count ) { display(5); };
};
