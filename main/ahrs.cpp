/*
 * ahrs.c
 *
 *  Created on: 22 mai 2021
 *      Author: GFM
 */

#include "ahrs.hpp"

#include "MadgwickAHRS.h"
#include "sensor.h"

#define DEBUG_INIT()
#define DEBUG_TS_PRINT(x)
#define DEBUG_TS_PRINTLN(x)

#define sqr(x) x *x
#define hypotenuse(x, y) sqrt(sqr(x) + sqr(y))

// MPU-6050
#define IMU_ADDR 0x68
#define IMU_ACCEL_XOUT_H 0x3B
#define IMU_REG 0x19
#define IMU_PWR_MGMT_1 0x6B

// Private Variables
uint32_t IMU::lastProcessed = 0;

double 	IMU::roll = 0;
double  IMU::pitch = 0;
double  IMU::yaw= 0;
double  IMU::offset_pitch = 0;
double  IMU::offset_roll = 0;
double  IMU::offset_yaw = 0;

uint64_t IMU::last_rts=0;
double IMU::accelX = 0.0;
double IMU::accelY = 0.0;
double IMU::accelZ = 0.0;
double IMU::accel_earthX = 0.0;
double IMU::accel_earthY = 0.0;
double IMU::accel_earthZ = 0.0;
double IMU::gyroX = 0.0;
double IMU::gyroY = 0.0;
double IMU::gyroZ = 0.0;
double IMU::offset_gyroX = 0.0;
double IMU::offset_gyroY = 0.0;
double IMU::offset_gyroZ = 0.0;
double IMU::offset_accelX = 0.0;
double IMU::offset_accelY = 0.0;
double IMU::offset_accelZ = 0.0;

// IMU Function Definition
	extern float u,v,w;
	extern float vx,vy,vz;
	double beta_quat;				// algorithm gain

void IMU::init(){
#define imax 10
	double gxsum=0;
	double gysum=0;
	double gzsum=0;
	double axsum=0;
	double aysum=0;
	double azsum=0;
	double rollsum=0;
	double pitchsum=0;
	double yawsum=0;
	for (int i=0;i<imax;i++){
		MPU6050Read();
		sleep( 0.1 );
		/* il faudrait faire une moyenne sur 10s*/
		gxsum+=gyroX;
		gysum+=gyroY;
		gzsum+=gyroZ;
		axsum+=accelX;
		aysum+=accelY;
		azsum+=accelZ;
		}
	offset_gyroX = gxsum/imax;
	offset_gyroY = gysum/imax;
	offset_gyroZ = gzsum/imax;
	offset_accelX= axsum/imax;
	offset_accelY= aysum/imax;
	//offset_accelZ= azsum/imax;
	offset_accelZ= (azsum-G)/imax;

	for (int i=0;i<imax;i++){
		MPU6050Read();
		sleep( 0.1 );
		IMU::RollPitchFromAccel(&roll, &pitch);
		rollsum+=roll;
		pitchsum+=pitch;
		yawsum+=yaw;
		}
	offset_roll=rollsum/imax;
	offset_pitch = pitchsum/imax;
	offset_yaw = yawsum/imax;

	lastProcessed = micros();
	ESP_LOGD(FNAME, "Finished IMU setup  gyroYAngle:%f ", pitch);
	/* Initialisation du quaternion*/
	q0=((cos((double)getRollRad()/2.0)*cos((double)getPitchRad()/2.0)*cos((double)yaw/2.0)+sin((double)getRollRad()/2.0)*sin((double)getPitchRad()/2.0)*sin((double)yaw/2.0)));
	q1=((sin((double)getRollRad()/2.0)*cos((double)getPitchRad()/2.0)*cos((double)yaw/2.0)-cos((double)getRollRad()/2.0)*sin((double)getPitchRad()/2.0)*sin((double)yaw/2.0)));
	q2=((cos((double)getRollRad()/2.0)*sin((double)getPitchRad()/2.0)*cos((double)yaw/2.0)+sin((double)getRollRad()/2.0)*cos((double)getPitchRad()/2.0)*sin((double)yaw/2.0)));
	q3=((cos((double)getRollRad()/2.0)*cos((double)getPitchRad()/2.0)*sin((double)yaw/2.0)-sin((double)getRollRad()/2.0)*sin((double)getPitchRad()/2.0)*cos((double)yaw/2.0)));

}
void IMU::read(){
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
		MadgwickAHRSupdateIMU(gyroX, gyroY, gyroZ, accelX, accelY, accelZ);
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
{// Les mesures sont transformées en valeur SI dès leur mise en forme
	// et les offsets enlevés
		accelX = -(accelG[2]*G-offset_accelX);
		accelY = (accelG[1]*G-offset_accelY);
		accelZ = (accelG[0]*G-offset_accelZ);
		gyroX = -(gyroDPS.z*DEG_TO_RAD-offset_gyroX);
		gyroY = (gyroDPS.y*DEG_TO_RAD-offset_gyroY);
		gyroZ = (gyroDPS.x*DEG_TO_RAD-offset_gyroZ);
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

	*roll = atan2((double)(accelY) , (double)(accelZ));
	*pitch = atan2((double)-(accelX), (double)(accelZ));

	ESP_LOGD( FNAME,"Accelerometer Roll: %f  Pitch: %f  (y:%f x:%f)", *roll * RAD_TO_DEG, *pitch * RAD_TO_DEG, (double)accelY, (double)accelX );

}

