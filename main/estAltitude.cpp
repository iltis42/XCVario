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


#include "estAltitude.h"
#include "deadReckoning.h"
#include <math.h>
#include <stdlib.h>
#include "BMPVario.h"
//#include "PressureSensor.h"
#include "UBX_Parser.h"
#include "deadReckoning.h"

#define ALPHA_CONSTANTE 2
#define BETA_CONSTANTE 6
//Qualities definition : the greater the quality is, the more the data is filtered
#define QUAL_IMU_MAX 4
#define QUAL_IMU_MIN 2
#define QUAL_GPS_MAX 8
#define QUAL_GPS_MIN 2
#define QUAL_BARO_MAX 60
#define QUAL_BARO_MIN 20
// seconds
#define DR_TIMESTEP (1.0/HEARTBEAT_HZ)
/*
// The origin is recorded as the altitude of the plane during power up.
				float alt = baroSensor->readAltitude( qnh, ok);
				baroP = baroSensor->readPressure(ok);   // 5x per second
				// ESP_LOGI(FNAME,"Baro Pressure: %4.3f", baroP );
				float altSTD = baroSensor->calcAVGAltitudeSTD( baroP );
*/
//extern PressureSensor *baroSensor;
//extern PressureSensor *teSensor;
static float barometer_agl_altitude_pred;// previous above ground level altitude - AGL
static long barometer_altitude;          // above sea level altitude - ASL (meters)
float barometer_agl_altitude;

extern float gps_altitude;              // previous above airfield height (meters)
static float gps0=0;                    //  altitude recorded during power up for height (meters)
static float gps_altitude_pred;         // predicted above ground height (meters)
static float IMU_altitude;              // previous above ground height (meters)
static float IMU_altitude_pred;         // predicted above ground height (meters)
static float vze_baro,vze_gps,vze_IMU;  // Altitude speeds in m/s
static int qual_baro,qual_gps,qual_IMU; // altitudes qualities
float        alpha,beta;                // filter coefficients
static int val_baro,val_gps,val_IMU;    // altitude validities
static int baro_confiance=2;            //Altitudes confidences
static int gps_confiance=10;
static int IMU_confiance=0;
float vze;
extern float vze_fusion;                   // Vertical speed fusion output (in m/s)
extern float estimated_altitude;               // Altitude fusion output (in m)
extern int gps_nav_valid;
extern float Vsz_gps;
void altimeter_calibrate(void)
{
	barometer_altitude = bmpVario.readAVGalt();
    qual_IMU = 2;
    IMU_altitude = barometer_altitude;
    vze_IMU = 0;
    IMU_altitude_pred = barometer_altitude;
    vze_baro = 0;
    qual_baro = 10;
    barometer_agl_altitude_pred = barometer_altitude;
    vze_gps = 0;
    vze_fusion = 0;
    qual_gps = 2;
    gps_altitude_pred = gps_altitude;
    qual_baro = 0;
    val_baro = 0;
    val_gps = 0;
    val_IMU = 0;
    if (gps_nav_valid) gps0 = gps_altitude; else gps0=barometer_altitude;
    estimated_altitude = barometer_altitude;
    //if (gps0 ==0) gps0 = gps_altitude;
    //  else gps0 = (15*gps0 + gps_altitude)/16;// If altimeter_calibrate is called many times at power up
}

//Estimate the best altitude from all three sensors IMU, Baro and GPS

void estAltitude(void)
{
	if( alt_select.get() == 0 ) {// TE is the only baro sensor for the moment
		barometer_altitude = bmpVario.readAVGalt();
/*	else { // Baro
		if(  alt_unit.get() == 2 ) { // FL, always standard
			barometer_altitude = altSTD;
			// ESP_LOGI(FNAME,"au: %d", alt_unit.get() );
		}else if( (fl_auto_transition.get() == 1) && ((int)altSTD*0.0328084 + (int)(standard_setting) > transition_alt.get() ) ) {
			barometer_altitude = altSTD;
			// ESP_LOGI(FNAME,"auto:%d alts:%f ss:%d ta:%f", fl_auto_transition.get(), altSTD, standard_setting, transition_alt.get() );
		}
		else {
			barometer_altitude = baroSensor->calcAVGAltitude( QNH.get(), baroP );
			// ESP_LOGI(FNAME,"QNH %f baro: %f alt: %f SS:%d", QNH.get(), baroP, alt, standard_setting  );
*/
	}

// Beginning alti_fusion gfm
      /*======================================================================*/
      /*          Altimeter fusion   (all altitudes in m)                    */
      /*======================================================================*/
      // IMU altitude filter and offset compensation are calculated in deadReckoning
      IMU_altitude = IMUlocationz;

      // Update qualities
      if (abs(IMU_altitude - estimated_altitude)<10)
      {         
          if (qual_IMU <QUAL_IMU_MAX ) qual_IMU=qual_IMU+1;
      }
      else
      {        
          if (qual_IMU>QUAL_IMU_MIN)  qual_IMU=qual_IMU-1;
      }
       
      if (abs(barometer_altitude - estimated_altitude)<100)
      {          
          if (qual_baro <QUAL_BARO_MAX ) qual_baro=qual_baro+1;
      }
      else
      {          
          if (qual_baro>QUAL_BARO_MIN) qual_baro=qual_baro-1;
      }
      if (abs(gps_altitude - estimated_altitude)<15)
      {          
          if (qual_gps <QUAL_GPS_MAX ) qual_gps=qual_gps+1;
      }
      else
      {         
          if (qual_gps>QUAL_GPS_MIN) qual_gps=qual_gps-1;
      }
//         gps_confiance=10;
//         IMU_confiance=0;
      
      // Compute filter alpha beta parameters for IMU
      alpha = ALPHA_CONSTANTE * (2*qual_IMU-1)/ qual_IMU/(qual_IMU+1);
      beta = BETA_CONSTANTE / qual_IMU / (qual_IMU+1);

      vze_IMU += beta * (IMU_altitude-IMU_altitude_pred) ;
      IMU_altitude_pred +=  alpha * (IMU_altitude-IMU_altitude_pred) + vze_IMU * DR_TIMESTEP;

      // Compute filter alpha beta parameters for Baro
      alpha = ALPHA_CONSTANTE * (2*qual_baro-1) / qual_baro / (qual_baro+1);
      beta = BETA_CONSTANTE / qual_baro /(qual_baro+1);

      vze_baro +=  beta * (barometer_altitude-barometer_agl_altitude_pred);
      barometer_agl_altitude_pred += alpha * (barometer_altitude-barometer_agl_altitude_pred) +vze_baro*DR_TIMESTEP;

      if (gps_nav_valid == 1){
      // Compute filter alpha beta parameters for GNSS
      alpha = ALPHA_CONSTANTE *(2*qual_gps-1)/qual_gps/(qual_gps+1);
      beta = BETA_CONSTANTE / qual_gps / (qual_gps+1);

      vze_gps = Vsz_gps ;
      gps_altitude_pred += alpha * (gps_altitude-gps_altitude_pred-gps0)+vze_gps*DR_TIMESTEP ;
      // update confidences according to some heuristic rules
      val_gps = qual_gps * gps_confiance;      // val_gps max = 8*10=80
      if ((abs(gps_altitude_pred - estimated_altitude)>60)) val_gps = 0;
      }
      else {
           val_gps = 0;
           gps_altitude_pred = gps_altitude;
      }
      // update confidences according to some heuristic rules
      val_IMU = qual_IMU * IMU_confiance  ;    //val_IMU max = 4*5=20
      if (( IMU_altitude_pred < -100) || (abs(IMU_altitude_pred - estimated_altitude)>40)|| (val_gps>10))  val_IMU = 0;
      val_baro = qual_baro * baro_confiance;   //val_baro max = 60
      if (( barometer_agl_altitude_pred < -4000) || (abs(barometer_agl_altitude_pred - estimated_altitude)>100)) val_baro = 0;


      // compute fusion
      if ((val_baro>0.0) || (val_gps>0.0) || (val_IMU>0.0))//If at least one sensor is validated
        {
    	  estimated_altitude = val_baro*barometer_agl_altitude_pred + val_gps*gps_altitude_pred +val_IMU*IMU_altitude_pred  ;
    	  estimated_altitude /= (val_baro+val_gps+val_IMU);
 
        vze = val_baro*vze_baro + val_gps*vze_gps + val_IMU*vze_IMU  ;
        vze /= (val_baro+val_gps+val_IMU);
        vze_fusion = vze;// room for some filtrage
      }
      else// if no sensor validated, use IMU only
      {
    	  estimated_altitude = IMU_altitude;
    	  vze_fusion = IMUvelocityz;
      }
    // fin alti_fusion gfm
}
