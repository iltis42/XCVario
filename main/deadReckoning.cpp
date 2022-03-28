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


#include "deadReckoning.h"
#include "UBX_Parser.h"

#include "ahrs.hpp"
//#include "estWind.h"
#include "estAltitude.h"


// seconds
#define DR_TIMESTEP (1.0/HEARTBEAT_HZ)		// Inverse of HeartBeat
// dimensionless
#define DR_TAU 1
#define DR_FILTER_GAIN DR_TIMESTEP/DR_TAU
#define DR_FILTER_GAIN_Z DR_TIMESTEP/DR_TAU*2.0
// seconds
#define DR_I_GAIN DR_FILTER_GAIN/DR_TAU
#define DR_I_GAIN_Z DR_FILTER_GAIN_Z/DR_TAU*2.0
// Gravity
#define G 9.807

//float forward_ground_speed = 0 ;
//float IMU_climb = 0 ;

//float air_speed_3DIMU = 0;
//float total_energy = 0;

// GPSvelocity - IMUvelocity
float velocityErrorEarth[] = { 0, 0, 0 };
float locationErrorEarth[] = { 0, 0, 0 };
extern float estimated_altitude;        // previous above ground height (millimeters)
extern float vze_fusion;                // Altitude speed fusion output (in mm)

//extern int16_t dead_reckon_clock ;
extern int dead_reckon_clock ;
extern int gps_nav_valid;
static uint64_t last_rts_deadrecko;

void dead_reckon(void)
{
//	float air_speed_x, air_speed_y, air_speed_z;
//	float accum;
//	float energy;
	double dt=0;
	bool ret=false;
	uint64_t rts = esp_timer_get_time();
	if( last_rts_deadrecko == 0 )
		ret=true;
	dt = (double)(rts - last_rts_deadrecko)/1000000.0;
	last_rts_deadrecko = rts;
	if( ret )
		return;

	// use fusion for altitude whatever the GPS is
	locationErrorEarth[2] = estimated_altitude - IMUlocationz;
	velocityErrorEarth[2] = vze_fusion - IMUvelocityz;

	// compensate for velocity error ;
	IMUintegralAccelerationz += DR_I_GAIN_Z * velocityErrorEarth[2];

	// integrate the raw acceleration m/s
	IMUvelocityz += IMU::getEarthAccelZ()*dt;

	// apply the proportional term for the acceleration bias compensation
	IMUvelocityz += 2*DR_FILTER_GAIN_Z * velocityErrorEarth[2];

	// apply the integral term for the acceleration bias compensation
	IMUvelocityz += IMUintegralAccelerationz*dt;
	// integrate IMU velocity to update the IMU location
	IMUlocationz += IMUvelocityz * dt;

	// apply the location bias compensation
	IMUlocationz += DR_FILTER_GAIN_Z * locationErrorEarth[2];

	if(dead_reckon_clock>0)
    {
		dead_reckon_clock--;
		if (gps_nav_valid == 1)
		{
			velocityErrorEarth[0] = Vsx_gps - IMUvelocityx;
			velocityErrorEarth[1] = Vsy_gps - IMUvelocityy;
			// compute the integral term for the acceleration bias compensation
			IMUintegralAccelerationx += DR_I_GAIN * velocityErrorEarth[0];
			IMUintegralAccelerationy += DR_I_GAIN * velocityErrorEarth[1];

			// integrate the raw acceleration
			IMUvelocityx += IMU::getEarthAccelX()*dt;
			IMUvelocityy += IMU::getEarthAccelY()*dt;
		
			// apply the proportional term for the acceleration bias compensation
			IMUvelocityx += 2*DR_FILTER_GAIN * velocityErrorEarth[0];
			IMUvelocityy += 2*DR_FILTER_GAIN * velocityErrorEarth[1];
		
			// apply the integral term for the acceleration bias compensation
			IMUvelocityx += IMUintegralAccelerationx*dt;
			IMUvelocityy += IMUintegralAccelerationy*dt;
		
		}
	}
	else  // GPS has gotten disconnected
	{
		//gps_nav_valid = 0 ;//Not needed if gps_nav_valid were set to -1 in the GPS parser
		// integrate the raw acceleration
		IMUvelocityx += IMU::getEarthAccelX()*dt;
		IMUvelocityy += IMU::getEarthAccelY()*dt;
		IMUvelocityz += IMU::getEarthAccelZ()*dt;
		IMUlocationz += IMUvelocityz * dt;
	}


/*	air_speed_x = IMUvelocityx._.W1 - estimatedWind[0];
	air_speed_y = IMUvelocityy._.W1 - estimatedWind[1];
	air_speed_z = IMUvelocityz._.W1 - estimatedWind[2];

	accum.WW = ((__builtin_mulss(-IMUintegralAccelerationx._.W1, rmat[1])
	                          + __builtin_mulss( IMUintegralAccelerationy._.W1, rmat[4])) << 2);
	forward_ground_speed = accum._.W1 ;

	air_speed_3DIMU = vector3_mag(air_speed_x, air_speed_y, air_speed_z);

	accum.WW   = __builtin_mulsu(air_speed_x, 37877);
	energy.WW  = __builtin_mulss(accum._.W1, accum._.W1);

	accum.WW   = __builtin_mulsu(air_speed_y, 37877);
	energy.WW += __builtin_mulss(accum._.W1, accum._.W1);

	accum.WW   = __builtin_mulsu(air_speed_z, 37877);
	energy.WW += __builtin_mulss(accum._.W1, accum._.W1);

	energy.WW += IMUlocationz.WW;
	total_energy = energy._.W1;
 */
}
