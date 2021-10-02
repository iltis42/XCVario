/*
 * ahrs.cpp
 *
 *  Created on: 22 mai 2021
 *      Author: GFM
 */

#include "ahrs.hpp"

#include "mpu/types.hpp"
#include "MadgwickAHRS.h"
#include "logdef.h"
#include "sensor.h"
#include "UBX_Parser.h"

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
bool IMU::initdone = false;
double  IMU::filterPitch = 0;
double  IMU::filterRoll = 0;
double  IMU::filterYaw = 0;
double  IMU::filterAccelX = 0;
double  IMU::filterAccelY = 0;
double  IMU::filterAccelZ = G;
double  IMU::filterGyroX = 0;
double  IMU::filterGyroY = 0;
double  IMU::filterGyroZ = 0;
double IMU::mX = 0.0;
double IMU::mY = 0.0;
double IMU::mZ = 0.0;

// IMU Function Definition
	extern float u,v,w;
	extern float vx,vy,vz;
	extern MPU_t MPU;         // create an object
	extern mpud::float_axes_t accelG;
	extern mpud::float_axes_t gyroDPS;
	mpud::raw_axes_t accelRaw;     // holds x, y, z axes as int16
	mpud::raw_axes_t gyroRaw;      // holds x, y, z axes as int16
	double q0 = 0.0;
	double q1 = 0.0;
	double q2 = 0.0;
	double q3 = 0.0;	// quaternion of sensor frame relative to auxiliary frame
	static double gravity_vector_plane[] = { 0, 0, G };
	static double previous_tas =0;
	bool ok=false;

/* Functions omegaSOG() and adj_accel() are included here with William J (Bill) Premerlani permission*/
	static double omegaSOG(double omega, double speed)
	{
	#define CENTRIFSAT 60 //1 rd/s * 60 m/s
		// multiplies omega times speed, and scales appropriately
		// omega in radians per second, speed in m per second
		double working;
		working = omega * speed;
		if (working > CENTRIFSAT)
		{
			return (CENTRIFSAT);
		}
		else if (working < -CENTRIFSAT)
		{
			return (- CENTRIFSAT);
		}
		else
		{
			return (working);
		}
	}

	static void adj_accel(double angleOfAttack)
	{
		// Performs centrifugal compensation without a GPS.
		// Based on the fact that the magnitude of the
		// compensated gplane vector should be GRAVITY*GRAVITY.
		// This produces another equation from which the
		// product of airspeed time rotation rate can be reasonably estimated.
		double omega_times_velocity ; // it should be positive, but noise
		                               // in the computations could produce neg
		double radical;
		double accum;
		double accelY;
		double vertical_cross_rotation_axis;
		double force_cross_rotation_axis;
		double rotation_axis[2];
		double recipNorm;
		// Compute the X-Z rotation axis
		// by normalizing the X-Z gyro vector
		rotation_axis[0] = IMU::getFilterGyroX();
		rotation_axis[1] = IMU::getFilterGyroZ();
		//vector2_normalize(rotation_axis, rotation_axis);
		if ( (rotation_axis[0] != 0) && (rotation_axis[0] != 0)) {
			recipNorm = 1.0f / sqrt(rotation_axis[0] * rotation_axis[0] + rotation_axis[1] * rotation_axis[1]);
			rotation_axis[0] *= recipNorm;
			rotation_axis[1] *= recipNorm;
			}

		// compute force cross rotation axis:
		force_cross_rotation_axis = (IMU::getFilterAccelX() * rotation_axis[1]) - ( IMU::getFilterAccelZ() * rotation_axis[0]  ) ;

		// compute vertical cross rotation axis:
		vertical_cross_rotation_axis = (2.0*(-q0*q2+q1*q3)* rotation_axis[1]) - ((q0*q0-q1*q1-q2*q2+q3*q3)* rotation_axis[0] );

		// compute the square root of the sum of the square of the
		// force cross rotation, minus the square of the magnitude of the accelerometer vector,
		// plus the square of GRAVITY

		// Start by using rmat for accelY instead of the measured value.
		// It is less sensitive to forward acceleration, which cannot be compensated without GPS.
		accelY = ( 2.0*(q2*q3+q0*q1) * G );

		// form the sum
		accum = (force_cross_rotation_axis * force_cross_rotation_axis)
		         + G * G
		         - IMU::getFilterAccelX() * IMU::getFilterAccelX()
		         - IMU::getFilterAccelZ() * IMU::getFilterAccelZ()
		         - accelY* accelY;
		if (accum < 0)
		{
			accum = 0;
		}
		radical = sqrt(accum);

		// Now we are using the solution to quadratic equation in the theory,
		// and there is some logic for selecting the positive or negative square root
		if (force_cross_rotation_axis < 0)
		{
			omega_times_velocity = force_cross_rotation_axis + radical;
		}
		else
		{
			if (vertical_cross_rotation_axis < 0)
			{
				omega_times_velocity = force_cross_rotation_axis + radical;
			}
			else
			{
				omega_times_velocity = force_cross_rotation_axis - radical;
			}
		}
		if (omega_times_velocity < 0)
		{
			omega_times_velocity = 0;
		}
		// now compute omega vector cross velocity vector and adjust
		gravity_vector_plane[0] = IMU::getFilterAccelX();
		gravity_vector_plane[1] = IMU::getFilterAccelY();
		gravity_vector_plane[2] = IMU::getFilterAccelZ();
		accum = (omega_times_velocity * rotation_axis[1]  );
		gravity_vector_plane[0] -= accum;
		gravity_vector_plane[1] = IMU::getFilterAccelY();
		accum = (omega_times_velocity * rotation_axis[0] );
		gravity_vector_plane[2] += accum;

		// account for angle of attack and forward acceleration
		double air_speed_z;
		// total (3D) airspeed in cm/sec is used to adjust for acceleration
		// compute Z component of airspeed due to angle of attack
		accum = angleOfAttack * getTAS();
		air_speed_z = angleOfAttack * Ground_Speed_gps;
		// compute centrifugal and forward acceleration compensation (must use corrected gyro) and suppose that this routine is called at 10 Hz
		gravity_vector_plane[0] = gravity_vector_plane[0] + omegaSOG(IMU::getFilterGyroY(), air_speed_z);
		gravity_vector_plane[1] = gravity_vector_plane[1] - omegaSOG(IMU::getFilterGyroX(), air_speed_z) +  (Ground_Speed_gps-previous_tas)*10.0;
		previous_tas = Ground_Speed_gps;

	}

void IMU::init(bool f){
	#define imax 10
	double rollsum=0;
	double pitchsum=0;
	double yawsum=0;
	initdone = false;
	  double  roll=0.0;
	  double  pitch=0.0;
	  double  yaw=0.0;

	double gxsum=0;
	double gysum=0;
	double gzsum=0;
	double axsum=0;
	double aysum=0;
	double azsum=0;
	for (int i=0;i<imax;i++){
		sleep( 1 );
		esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
		if( err != ESP_OK )
			ESP_LOGE(FNAME, "accel I2C error, X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
		err |= MPU.rotation(&gyroRaw);       // fetch raw data from the registers
		if( err != ESP_OK )
			ESP_LOGE(FNAME, "gyro I2C error, X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
		accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // raw data to gravity
		gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);  // raw data to º/s
		gxsum-=(gyroDPS.z);
		gysum+=(gyroDPS.y);
		gzsum+=(gyroDPS.x);
		axsum-=(accelG[2]);
		aysum+=(accelG[1]);
		azsum+=(accelG[0]);
		ESP_LOGI(FNAME, "IMU setup  offsets:%d,%f,%f,%f,%f,%f,%f ",i, -(gyroDPS.z), (gyroDPS.y), (gyroDPS.x), -(accelG[2]), (accelG[1]), (accelG[0]));
		}
	offset_gyroX = (double)(gxsum/imax*DEG_TO_RAD);
	offset_gyroY = (double)(gysum/imax*DEG_TO_RAD);
	offset_gyroZ = (double)(gzsum/imax*DEG_TO_RAD);
	/* Suppression de l'hypothèse que l'assiette initiale est horizontale
	offset_accelX= (double)(axsum/imax)*G;
	offset_accelY= (double)(aysum/imax)*G;
	offset_accelZ= (double)(azsum/imax)*G-G;
	offset_roll=rollsum/imax;
	offset_pitch = pitchsum/imax;
	offset_yaw = yawsum/imax;
	*/
	for (int i=0;i<imax;i++){
		esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
		if( err != ESP_OK )
			ESP_LOGE(FNAME, "accel I2C error, X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
		err |= MPU.rotation(&gyroRaw);       // fetch raw data from the registers
		if( err != ESP_OK )
			ESP_LOGE(FNAME, "gyro I2C error, X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
		accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // raw data to gravity
		gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);  // raw data to º/s
		MPU6050Read();
		IMU::RollPitchFromAccel(&roll, &pitch);
		double my=QMC5883L::mag_vector[1]*cos(roll)- QMC5883L::mag_vector[2]*sin(roll);
		double m2=(QMC5883L::mag_vector[0]*QMC5883L::mag_vector[0]+QMC5883L::mag_vector[1]*QMC5883L::mag_vector[1]+QMC5883L::mag_vector[2]*QMC5883L::mag_vector[2]);
		double mz=-sin(pitch)*QMC5883L::mag_vector[0]+sin(roll)*cos(pitch)*QMC5883L::mag_vector[1]+cos(roll)*cos(pitch)*QMC5883L::mag_vector[2];
		double mh2=m2-mz*mz;
		double mx=sqrt(mh2-my*my);
		yaw = -atan2( my,mx );
		if (yaw>PI2) yaw -= PI2;
		if (yaw<0.0) yaw += PI2;
		rollsum+=roll;
		pitchsum+=pitch;
		yawsum+=yaw;
		sleep( 1 );
	}
	roll=rollsum/imax;
	pitch = pitchsum/imax;
	yaw = yawsum/imax;
	if (yaw>PI2) yaw -= PI2;
	if (yaw<0.0) yaw += PI2;
	lastProcessed = micros();
	ESP_LOGI(FNAME, "Finished IMU setup  offsets:%f,%f,%f,%f,%f,%f ", offset_gyroX, offset_gyroY, offset_gyroZ, offset_accelX, offset_accelY, offset_accelZ);
	ESP_LOGI(FNAME, "Attitude:%f,%f,%f ", roll,pitch,yaw);
	/* Initialisation du quaternion*/
	q0=((cos((double)roll/2.0)*cos((double)pitch/2.0)*cos((double)yaw/2.0)+sin((double)roll/2.0)*sin((double)pitch/2.0)*sin((double)yaw/2.0)));
	q1=((sin((double)roll/2.0)*cos((double)pitch/2.0)*cos((double)yaw/2.0)-cos((double)roll/2.0)*sin((double)pitch/2.0)*sin((double)yaw/2.0)));
	q2=((cos((double)roll/2.0)*sin((double)pitch/2.0)*cos((double)yaw/2.0)+sin((double)roll/2.0)*cos((double)pitch/2.0)*sin((double)yaw/2.0)));
	q3=((cos((double)roll/2.0)*cos((double)pitch/2.0)*sin((double)yaw/2.0)-sin((double)roll/2.0)*sin((double)pitch/2.0)*cos((double)yaw/2.0)));
	ESP_LOGI(FNAME, "Quaternion:%f,%f,%f,%f ", q0,q1,q2,q3);

	initdone=true;
}
void IMU::read(){
	double dt=0;
	bool ret=false;
	  double  roll=0.0;
	  double  pitch=0.0;
	  double  yaw=0.0;

		uint64_t rts = esp_timer_get_time();
		if( last_rts == 0 )
			ret=true;
		dt = (double)(rts - last_rts)/1000000.0;
		last_rts = rts;
		if( ret )
			return;
//		if(dt>0.11) {/* si la période de temps n'est pas celle attendue, on n'intègre pas les gyros*/
		if(false) {/* si la période de temps n'est pas celle attendue, on n'intègre pas les gyros*/
			initdone = false;
			IMU::init(false);
		}
		else {//La période de temps est cohérente de celle qu'on attend, cette précaution a été mise en place à cause d'initialisation longue de XCVario
			esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
			if( err != ESP_OK )
				ESP_LOGE(FNAME, "accel I2C error, X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
			err |= MPU.rotation(&gyroRaw);       // fetch raw data from the registers
			if( err != ESP_OK )
				ESP_LOGE(FNAME, "gyro I2C error, X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
			accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // raw data to gravity
			gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);  // raw data to º/s
		MPU6050Read();
		//adj_accel(angleOfAttack);
		adj_accel(0.1);// angleOfAttack is supposed to be 6°
		// Acquisition du vecteur magnetomètre
//		esp_err_t err = MPU.heading(&magRaw);  // fetch raw data from the registers. Pas nécessaire car CalculateHeading
		// qui est appellé régulièrement dans readBMP() rafraichit les mesures magnétomètre.
		//Pour l'instant, les mesures magnéto ne sont ni filtrées, ni recalées dans le temps par rapport à l'IMU
		mX = QMC5883L::mag_vector[0];
		mY = QMC5883L::mag_vector[1];
		mZ = QMC5883L::mag_vector[2];
		//MadgwickAHRSupdateIMU(dt,gyroX, gyroY, gyroZ, gravity_vector_plane[0], gravity_vector_plane[1], gravity_vector_plane[2],&q0,&q1,&q2,&q3);
		//MadgwickAHRSupdateIMU(dt,gyroX, gyroY, gyroZ, accelX, accelY, accelZ,&q0,&q1,&q2,&q3);
		MadgwickAHRSupdate(dt,gyroX, gyroY, gyroZ, gravity_vector_plane[0], gravity_vector_plane[1], gravity_vector_plane[2], mX, mY, mZ,&q0,&q1,&q2,&q3);
		//MadgwickAHRSupdateIMU(dt,gyroX, gyroY, gyroZ, accelX, accelY, accelZ, mX, mY, mZ,&q0,&q1,&q2,&q3);
	// Calcul des anges d'Euler
	  roll = atan2(2 * (q0 * q1 + q2 * q3), (q0*q0-q1*q1-q2*q2+q3*q3)); // phi
	  if (abs(2*( q0 * q2-q1 * q3 ))<1.0) pitch = asin(2 * ( q0 * q2-q1 * q3 )); else pitch = 0.0; // theta
	  yaw = atan2(2 * (q1 * q2 + q0 * q3),(q0*q0+q1*q1-q2*q2-q3*q3) ) ; // psi
	  /*filterPitch += (pitch - filterPitch) * 0.2;
	  filterRoll += (roll - filterRoll) * 0.2;
	  filterYaw += (yaw - filterYaw) * 0.2;
	   */
	  filterPitch = pitch ;
	  filterRoll = roll ;
	  filterYaw = yaw ;
	  filterAccelX += (accelX - filterAccelX);
	  filterAccelY += (accelY - filterAccelY);
	  filterAccelZ += (accelZ - filterAccelZ);
	  filterGyroX += (gyroX - filterGyroX);
	  filterGyroY += (gyroY - filterGyroY);
	  filterGyroZ += (gyroZ - filterGyroZ);
	   // Variables intermédiares
			float q0q0 = q0 * q0;
			float q0q1 = q0 * q1;
			float q0q2 = q0 * q2;
			float q0q3 = q0 * q3;
			float q1q1 = q1 * q1;
			float q1q2 = q1 * q2;
			float q1q3 = q1 * q3;
			float q2q2 = q2 * q2;
			float q2q3 = q2 * q3;
			float q3q3 = q3 * q3;

// Calcul de la gravité en repère engin
	float gx=2.0*(-q0q2 + q1q3)*G;
	float gy=2.0*(q2q3+q0q1)*G;
	float gz=(q0q0-q1q1-q2q2+q3q3)*G;
	  // Intégration des vitesses en repère engin avec prise en compte de l'accélération d'entrainement
	u=u+(accelX-gx+v*gyroZ-w*gyroY)*dt;
	v=v+(accelY-gy+w*gyroX-u*gyroZ)*dt;
	w=w+(accelZ-gz+u*gyroY-v*gyroX)*dt;

	// Passage des vitesses du repère engin en repère galiléen
	//  Psi:=arccos((q0q0+q1q1-q2q2-q3q3)/cos(Teta));If (q1q2+q0q3)<0.0 Then Psi:=-Psi;
	//  Phi:=arccos((q0q0-q1q1-q2q2+q3q3)/cos(Teta));If (q2q3+q0q1)<0.0 Then Phi:=-Phi;

	vx=(q0q0+q1q1-q2q2-q3q3)*u + 2.0*(q1q2-q0q3)*v + 2.0*(q0q2 + q1q3)*w;
	vy=2.0*(q1q2+q0q3)*u + (q0q0-q1q1+q2q2-q3q3)*v + 2.0*(q2q3-q0q1)*w;
	vz=2.0*(q1q3-q0q2)*u + 2.0*(q2q3+q0q1)*v + (q0q0-q1q1-q2q2+q3q3)*w;

	accel_earthX = (accelX*(q0q0+q1q1-q2q2-q3q3)+accelY*2.0*(q1q2-q0q3)+accelZ*2.0*(q1q3+q0q2));
	accel_earthY = (accelX*2.0*(q1q2+q0q3)+accelY*(q0q0-q1q1+q2q2-q3q3)+accelZ*2.0*(q2q3-q0q1));
	accel_earthZ = (accelX*2.0*(-q0q2+q1q3)+accelY*2.0*(q2q3+q0q1)+accelZ*(q0q0-q1q1-q2q2+q3q3)) - G;//Substract pesanteur
	}
	ESP_LOGI(FNAME, "attitude,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f ",
				dt,filterGyroX, filterGyroY, filterGyroZ, filterAccelX, filterAccelY, filterAccelZ,roll,pitch,yaw,q0,q1,q2,q3,
				gravity_vector_plane[0], gravity_vector_plane[1], gravity_vector_plane[2], accel_earthX, accel_earthY, accel_earthZ,
				u,v,w,vx,vy,vz);
}


void IMU::MPU6050Read()
{// Les mesures sont transformées en valeur SI dès leur mise en forme
	// et les offsets enlevés
		accelX = (accelG[2]*G-offset_accelX);
		accelY = (accelG[1]*G-offset_accelY);
		accelZ = (accelG[0]*G-offset_accelZ);
		gyroX = -(gyroDPS.z*DEG_TO_RAD-offset_gyroX);
		gyroY = -(gyroDPS.y*DEG_TO_RAD-offset_gyroY);
		gyroZ = -(gyroDPS.x*DEG_TO_RAD-offset_gyroZ);
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
	*pitch = atan2((double)(-accelX), (double)(accelZ));

	ESP_LOGD( FNAME,"Accelerometer Roll: %f  Pitch: %f  (y:%f x:%f)", *roll * RAD_TO_DEG, *pitch * RAD_TO_DEG, (double)accelY, (double)accelX );

}

