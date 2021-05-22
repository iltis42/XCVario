/*
 * ahrs.c
 *
 *  Created on: 22 mai 2021
 *      Author: GFM
 */

#include "ahrs.h"
#include "MadgwickAHRS.h"
#include "sensor.h"

// IMU Function Definition
	double roll=0;
	double pitch=0;
	double yaw=0;/* il faudrait initialiser Yaw sur le QFU de la piste lorsqu'on est parfaitement aligné*/
	extern float u,v,w;
	extern float vx,vy,vz;

void IMU::init(void){
	MPU6050Read();
	// sleep( 0.1 );
	roll=0;
	pitch=0;
	yaw=0;/* il faudrait initialiser Yaw sur le QFU de la piste lorsqu'on est parfaitement aligné*/
	/* il faudrait faire une moyenne sur 10s*/
	IMU::RollPitchFromAccel(&roll, &pitch);

	lastProcessed = micros();
	ESP_LOGD(FNAME, "Finished IMU setup  gyroYAngle:%f ", pitch);

	/* Initialisation du quaternion*/
	q0=((cos((double)roll/2.0)*cos((double)pitch/2.0)*cos((double)yaw/2.0)+sin((double)roll/2.0)*sin((double)pitch/2.0)*sin((double)yaw/2.0)));
	q1=((sin((double)roll/2.0)*cos((double)pitch/2.0)*cos((double)yaw/2.0)-cos((double)roll/2.0)*sin((double)pitch/2.0)*sin((double)yaw/2.0)));
	q2=((cos((double)roll/2.0)*sin((double)pitch/2.0)*cos((double)yaw/2.0)+sin((double)roll/2.0)*cos((double)pitch/2.0)*sin((double)yaw/2.0)));
	q3=((cos((double)roll/2.0)*cos((double)pitch/2.0)*sin((double)yaw/2.0)-sin((double)roll/2.0)*sin((double)pitch/2.0)*cos((double)yaw/2.0)));

}
void IMU::read(void){
	double dt=0;
	bool ret=false;

		uint64_t rts = esp_timer_get_time();
		if( last_rts == 0 )
			ret=true;
		dt = (double)(rts - last_rts)/1000000.0;
		last_rts = rts;
		if( ret )
			return;

		MPU6050Read();
		MadgwickAHRSupdateIMU(gyroX*DEG_TO_RAD, gyroY*DEG_TO_RAD, gyroZ*DEG_TO_RAD, accelX, accelY, accelZ);
	// Intégration des anges d'Euler
	  float temp1=q0;
	  float temp2=q1;
	  float temp3=q2;
	  float temp4=q3;
	  float temp5=2 * (temp1 * temp2 - temp3 * temp4);
	  float temp6=temp1*temp1-temp2*temp2-temp3*temp3+temp4*temp4;
	  roll = atan2(temp5, temp6); // phi
	  temp5=2 * ( temp1 * temp3-temp2 * temp4 );
	  if (abs(temp5)<1.0) pitch = asin(temp5); else pitch = 0.0; // theta
	  temp5=2 * (temp2 * temp3 + temp1 * temp4);
	  temp6=temp1*temp1+temp2*temp2-temp3*temp3-temp4*temp4;
	  yaw = atan2(temp5,temp6 ) ; // psi

	   // Variables intermédiares
			float q0q0 = temp1 * temp1;
			float q0q1 = temp1 * temp2;
			float q0q2 = temp1 * temp3;
			float q0q3 = temp1 * temp4;
			float q1q1 = temp2 * temp2;
			float q1q2 = temp2 * temp3;
			float q1q3 = temp2 * temp4;
			float q2q2 = temp3 * temp3;
			float q2q3 = temp3 * temp4;
			float q3q3 = temp4 * temp4;

// Calcul de la gravité en repère engin
	float gx=2.0*(-q0q2 + q1q3)*G;
	float gy=2.0*(q2q3+q0q1)*G;
	float gz=(q0q0-q1q1-q2q2+q3q3)*G;
	  // Intégration des vitesses en repère engin
	u=u+(accelX-gx)*dt;
	v=v+(accelY-gy)*dt;
	w=w+(accelZ-gz)*dt;

	// Passage des vitesses du repère engin en repère galiléen
	//  Psi:=arccos((q0q0+q1q1-q2q2-q3q3)/cos(Teta));If (q1q2+q0q3)<0.0 Then Psi:=-Psi;
	//  Phi:=arccos((q0q0-q1q1-q2q2+q3q3)/cos(Teta));If (q2q3+q0q1)<0.0 Then Phi:=-Phi;

	vx=(q0q0+q1q1-q2q2-q3q3)*u + 2.0*(q1q2-q0q3)*v + 2.0*(q0q2 + q1q3)*w;
	vy=2.0*(q1q2+q0q3)*u + (q0q0-q1q1+q2q2-q3q3)*v + 2.0*(q2q3-q0q1)*w;
	vz=2.0*(q1q3-q0q2)*u + 2.0*(q2q3+q0q1)*v + (q0q0-q1q1-q2q2+q3q3)*w;
}


void IMU::MPU6050Read()
{
		accelX = -accelG[2];
		accelY = accelG[1];
		accelZ = accelG[0];
		gyroX = -(gyroDPS.z);
		gyroY = gyroDPS.y;
		gyroZ = gyroDPS.x;
}

void IMU::PitchFromAccel(double *pitch)
{
	*pitch = atan2(-accelX, accelZ);
}
void IMU::RollPitchFromAccel(double *roll, double *pitch)
{
	// Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
	// atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
	// It is then converted from radians to degrees

	*roll = atan2((double)accelY , (double)accelZ);
	*pitch = atan2((double)-accelX, (double)accelZ);

	ESP_LOGD( FNAME,"Accelerometer Roll: %f  Pitch: %f  (y:%f x:%f)", *roll * RAD_TO_DEG, *pitch * RAD_TO_DEG, (double)accelY, (double)accelX );

}

