// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _deadReckoning_HH
#define _deadReckoning_HH

#ifdef __cplusplus
extern "C" {
#endif

extern float air_speed_3DIMU;
extern float total_energy;
extern float locationErrorEarth[3];

extern float IMUlocationx, IMUlocationy, IMUlocationz;
extern float IMUvelocityx, IMUvelocityy, IMUvelocityz;
extern float IMUintegralAccelerationx;
extern float IMUintegralAccelerationy;
extern float IMUintegralAccelerationz;

extern float forward_ground_speed;

#define IMUheight IMUlocationz


void dead_reckon(void);

#define HEARTBEAT_HZ 10.0

#ifdef __cplusplus
}
#endif
#endif

