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
#include <logdef.h>
#include "PressureSensor.h"
#include "UBX_Parser.h"
#include "deadReckoning.h"

#define ALPHA_CONSTANTE 2.0
#define BETA_CONSTANTE 6.0
//Qualities definition : the greater the quality is, the more the data is filtered
#define QUAL_IMU_MAX 4.0
#define QUAL_IMU_MIN 2.0
#define QUAL_GPS_MAX 8.0
#define QUAL_GPS_MIN 4.0
#define QUAL_BARO_MAX 10.0
#define QUAL_BARO_MIN 5.0
// seconds
#define DR_TIMESTEP (1.0/HEARTBEAT_HZ)
/*
// The origin is recorded as the altitude of the plane during power up.
				float alt = baroSensor->readAltitude( qnh, ok);
				baroP = baroSensor->readPressure(ok);   // 5x per second
				// ESP_LOGI(FNAME,"Baro Pressure: %4.3f", baroP );
				float altSTD = baroSensor->calcAVGAltitudeSTD( baroP );
*/
extern PressureSensor *baroSensor;
//extern PressureSensor *teSensor;
static float barometer_agl_altitude_pred;// previous above ground level altitude - AGL
static float barometer_altitude;          // above sea level altitude - ASL (meters)
float barometer_agl_altitude;

extern float gps_altitude;              // previous above airfield height (meters)
static float gps0=0.0;                    //  altitude recorded during power up for height (meters)
static float gps_altitude_pred;         // predicted above ground height (meters)
static float IMU_altitude;              // previous above ground height (meters)
static float IMU_altitude_pred;         // predicted above ground height (meters)
static float vze_baro,vze_gps,vze_IMU;  // Altitude speeds in m/s
static int qual_baro,qual_gps,qual_IMU; // altitudes qualities
float        alpha,beta;                // filter coefficients
static float val_baro,val_gps,val_IMU;    // altitude validities
static float baro_confiance=100.0;            //Altitudes confidences
static float gps_confiance=100.0;
static float IMU_confiance=10.0;
float vze;
extern float vze_fusion;                   // Vertical speed fusion output (in m/s)
extern float estimated_altitude;               // Altitude fusion output (in m)
extern int gps_nav_valid;
extern float Vsz_gps;
void altimeter_calibrate(void)
{
	float baroP=0;
	bool ok=false;
	baroP = baroSensor->readPressure(ok);   // 5x per second
	// ESP_LOGI(FNAME,"Baro Pressure: %4.3f", baroP );
	barometer_altitude = -1.0f*baroSensor->calcAVGAltitude( QNH.get(), baroP );
    qual_IMU = QUAL_IMU_MIN;
    IMU_altitude = barometer_altitude;
    vze_IMU = 0.0;
    IMU_altitude_pred = barometer_altitude;
    vze_baro = 0.0;
    qual_baro = QUAL_BARO_MIN;
    barometer_agl_altitude_pred = barometer_altitude;
    vze_gps = 0.0;
    vze_fusion = 0.0;
    qual_gps = QUAL_GPS_MIN;
    val_baro = 0.0;
    val_gps = 0.0;
    val_IMU = 0.0;
    if (gps_nav_valid==1) gps0 = gps_altitude-barometer_altitude; else gps0=0.0;
    estimated_altitude = barometer_altitude;
    gps_altitude_pred = gps0;
    IMUlocationz=barometer_altitude;
    //if (gps0 ==0) gps0 = gps_altitude;
    //  else gps0 = (15*gps0 + gps_altitude)/16;// If altimeter_calibrate is called many times at power up
}

//Estimate the best altitude from all three sensors IMU, Baro and GPS

void estAltitude(void)
{
	bool ok=false;
	float baroRatio = baroSensor->readPressure(ok)/QNH.get();   // 5x per second
	// ESP_LOGI(FNAME,"Baro Pressure: %4.3f", baroP );
	if( alt_select.get() == 0 ) // TE
		barometer_altitude = -1.0f*bmpVario.readAVGalt();
	else { // Baro
		/*if(  alt_unit.get() == 2 ) { // FL, always standard
			barometer_altitude = altSTD;
			ESP_LOGI(FNAME,"au: %d", alt_unit.get() );
		}else if( (fl_auto_transition.get() == 1) && ((int)altSTD*0.0328084 + (int)(standard_setting) > transition_alt.get() ) ) {
			barometer_altitude = altSTD;
			ESP_LOGI(FNAME,"auto:%d alts:%f ss:%d ta:%f", fl_auto_transition.get(), altSTD, standard_setting, transition_alt.get() );
		}
		else {*/
		//barometer_altitude = -1.0f*baroSensor->readAltitude( QNH.get(), ok );
		barometer_altitude = -(4184*baroRatio*baroRatio*baroRatio+15330*baroRatio*baroRatio-26596*baroRatio+15450);
			// ESP_LOGI(FNAME,"QNH %f baro: %f barometer_altitude: %f ", QNH.get(), baroP, barometer_altitude  );
		}



// Beginning alti_fusion gfm
      /*======================================================================*/
      /*          Altimeter fusion   (all altitudes in m)                    */
      /*======================================================================*/
      // IMU altitude filter and offset compensation are calculated in deadReckoning
      IMU_altitude = IMUlocationz;

      // Update qualities
      if (abs(IMU_altitude - estimated_altitude)>10.0)
      {         
          if (qual_IMU <QUAL_IMU_MAX ) qual_IMU=qual_IMU+1;
      }
      else
      {        
          if (qual_IMU>QUAL_IMU_MIN)  qual_IMU=qual_IMU-1;
      }
       
      if (abs(barometer_altitude - estimated_altitude)>100.0)
      {          
          if (qual_baro <QUAL_BARO_MAX ) qual_baro=qual_baro+1;
      }
      else
      {          
          if (qual_baro>QUAL_BARO_MIN) qual_baro=qual_baro-1;
      }
      if (abs(gps_altitude - estimated_altitude)>15.0)
      {          
          if (qual_gps <QUAL_GPS_MAX ) qual_gps=qual_gps+1;
      }
      else
      {         
          if (qual_gps>QUAL_GPS_MIN) qual_gps=qual_gps-1;
      }
      
      // Compute filter alpha beta parameters for IMU
      alpha = (float)(ALPHA_CONSTANTE * (2.0*qual_IMU-1)/ qual_IMU/(qual_IMU+1));
      beta =  (float)(BETA_CONSTANTE / qual_IMU / (qual_IMU+1));

      vze_IMU += beta * (IMU_altitude-IMU_altitude_pred) ;
      IMU_altitude_pred +=  alpha * (IMU_altitude-IMU_altitude_pred) + vze_IMU;

      // Compute filter alpha beta parameters for Baro
      alpha =  (float)(ALPHA_CONSTANTE * (2*qual_baro-1) / qual_baro / (qual_baro+1));
      beta =  (float)(BETA_CONSTANTE / qual_baro /(qual_baro+1));

      vze_baro +=  beta * (barometer_altitude-barometer_agl_altitude_pred);
      barometer_agl_altitude_pred += alpha * (barometer_altitude-barometer_agl_altitude_pred) +vze_baro;

      if (gps_nav_valid == 1){
      // Compute filter alpha beta parameters for GNSS
      alpha =  (float)(ALPHA_CONSTANTE *(2*qual_gps-1)/qual_gps/(qual_gps+1));
      beta =  (float)(BETA_CONSTANTE / qual_gps / (qual_gps+1));

      vze_gps += beta * (gps_altitude-gps_altitude_pred-gps0) ;
      gps_altitude_pred += alpha * (gps_altitude-gps_altitude_pred-gps0)+vze_gps ;
      // update confidences according to some heuristic rules
      val_gps =  gps_confiance/qual_gps;      // val_gps max = 100/4=25
      if ((abs(gps_altitude_pred - estimated_altitude)>60.0)) val_gps = 0.0;
      }
      else {
           val_gps = 0.0;
           //vze_gps = 0.0 ;
           //gps_altitude_pred = gps_altitude;
      }
      // update confidences according to some heuristic rules
      val_IMU = (float)( IMU_confiance/qual_IMU)  ;    //val_IMU max = 100/2=50
      if (( IMU_altitude_pred > 100.0) || (abs(IMU_altitude_pred - estimated_altitude)>100.0)|| (val_gps>10.0))  val_IMU = 0.0;
      val_baro = (float)( baro_confiance/qual_baro);   //val_baro max = 100/5=20
      if (( barometer_agl_altitude_pred > 4000.0) || (abs(barometer_agl_altitude_pred - estimated_altitude)>100.0)) val_baro = 0.0;


      // compute fusion
      if ((val_baro>0.0) || (val_gps>0.0) || (val_IMU>0.0))//If at least one sensor is validated
        {
    	  estimated_altitude = val_baro*barometer_agl_altitude_pred + val_gps*gps_altitude_pred +val_IMU*IMU_altitude_pred  ;
    	  estimated_altitude /= (val_baro+val_gps+val_IMU);
 
        vze = val_baro*vze_baro + val_gps*vze_gps + val_IMU*vze_IMU  ;
        vze /= (val_baro+val_gps+val_IMU);
        vze_fusion = vze / DR_TIMESTEP;// room for some filtrage
      }
      else// if no sensor validated, use IMU only
      {
    	  estimated_altitude = IMU_altitude;
    	  vze_fusion = IMUvelocityz;
      }
    // fin alti_fusion gfm
}
