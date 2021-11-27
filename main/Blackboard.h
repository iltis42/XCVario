/*
Copyright_License {

  XCVario Sensor - http://www.xcvario.de/
  Copyright (C) 2017-2021 The XCVario Project

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License 2
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "SetupNG.h"
#include "Units.h"

class Vario : public SetupNG<float>
{
public:
	Vario( const char* akey, float adefault, e_sync_t sync, void (* action)() )
		: SetupNG(akey, adefault, true, sync, VOLATILE, action) {}

	virtual float getGui() const override { return Units::Vario(get()); }
	virtual const char* unit() const override { return Units::VarioUnit(); }
};

extern Vario MC;
