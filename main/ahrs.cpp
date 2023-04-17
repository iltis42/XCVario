/*
 * ahrs.cpp
 *
 *  Created on: 22 mai 2021
 *      Author: GFM mainly reuse of Bill Premerlani studies
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
// Alpha/Beta filter coefficient corresponding to N=6
#define alphaf 0.52
#define betaf 0.14
#define  Masse  500.0 // masse  de reférence kg
#define   Surface  16.4 // Surface de ref DuoDiscus   m²
#define   CLa  5.807 // Gradient de portance
#define   CL0  0.621
// Private Variables
uint32_t IMU::lastProcessed = 0;

float  IMU::offset_pitch = 0;
float  IMU::offset_roll = 0;
float  IMU::offset_yaw = 0;

uint64_t IMU::last_rts=0;
float IMU::accelX = 0.0;
float IMU::accelY = 0.0;
float IMU::accelZ = 0.0;
float IMU::accel_earthX = 0.0;
float IMU::accel_earthY = 0.0;
float IMU::accel_earthZ = 0.0;
float IMU::gyroX = 0.0;
float IMU::gyroY = 0.0;
float IMU::gyroZ = 0.0;
float IMU::offset_gyroX = 0.0;
float IMU::offset_gyroY = 0.0;
float IMU::offset_gyroZ = 0.0;
float IMU::offset_accelX = 0.0;
float IMU::offset_accelY = 0.0;
float IMU::offset_accelZ = 0.0;
bool IMU::initdone = false;
float  IMU::filterPitch = 0;
float  IMU::filterRoll = 0;
float  IMU::filterYaw = 0;
float  IMU::filterAccelX = 0;
float  IMU::filterAccelY = 0;
float  IMU::filterAccelZ = G;
float  IMU::filterGyroX = 0;
float  IMU::filterGyroY = 0;
float  IMU::filterGyroZ = 0;
float IMU::mX = 0.0;
float IMU::mY = 0.0;
float IMU::mZ = 0.0;
float Af=0.0;
float Vf=0.0;

// IMU Function Definition
	extern MPU_t MPU;         // create an object
	extern mpud::float_axes_t accelG;
	extern mpud::float_axes_t gyroDPS;
	mpud::raw_axes_t accelRaw;     // holds x, y, z axes as int16
	mpud::raw_axes_t gyroRaw;      // holds x, y, z axes as int16
	float q0 = 0.0;
	float q1 = 0.0;
	float q2 = 0.0;
	float q3 = 0.0;	// quaternion of sensor frame relative to auxiliary frame
	static float gravity_vector_plane[] = { 0, 0, G };
	static float previous_tas =0;
	bool ok=false;
	float angleOfAttack=0.0;

	static void adj_accel(float angleOfAttack)
	{
		gravity_vector_plane[0] = IMU::getFilterAccelX();
		gravity_vector_plane[1] = IMU::getFilterAccelY();
		gravity_vector_plane[2] = IMU::getFilterAccelZ();

		// account for angle of attack and forward acceleration
		float air_speed_x;
		float air_speed_y = 0.0;//Pas de dérapage
		float air_speed_z;
		// total (3D) airspeed in cm/sec is used to adjust for acceleration
		// compute Z component of airspeed due to angle of attack
		air_speed_x = getTAS(); //cos(angleOfAttack)=1
		air_speed_x = Ground_Speed_gps;
		air_speed_z = angleOfAttack * getTAS();//sin(angleOfAttack)=angleOfAttack
		air_speed_z = angleOfAttack * Ground_Speed_gps;
		// compute centrifugal and forward acceleration compensation (must use corrected gyro) and suppose that this routine is called at 10 Hz
		gravity_vector_plane[0] += IMU::getFilterGyroY()* air_speed_z-IMU::getFilterGyroZ()* air_speed_y+  Af;
		gravity_vector_plane[1] += IMU::getFilterGyroZ()* air_speed_x - IMU::getFilterGyroX()* air_speed_z ;
		gravity_vector_plane[2] += IMU::getFilterGyroX()* air_speed_y - IMU::getFilterGyroY()* air_speed_x ;
		previous_tas = Ground_Speed_gps;

	}

void IMU::init(bool f){
	#define imax 10
	float rollsum=0;
	float pitchsum=0;
	float yawsum=0;
	initdone = false;
	  float  roll=0.0;
	  float  pitch=0.0;
	  float  yaw=0.0;

	float gxsum=0;
	float gysum=0;
	float gzsum=0;
	float axsum=0;
	float aysum=0;
	float azsum=0;
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
		gxsum+=(gyroDPS.z);
		gysum+=(gyroDPS.y);
		gzsum+=(gyroDPS.x);
		axsum+=(accelG[2]);
		aysum+=(accelG[1]);
		azsum+=(accelG[0]);
		ESP_LOGI(FNAME, "IMU setup  offsets:%d,%f,%f,%f,%f,%f,%f ",i, -(gyroDPS.z), (gyroDPS.y), (gyroDPS.x), -(accelG[2]), (accelG[1]), (accelG[0]));
		}
	offset_gyroX = (float)(gxsum/imax*DEG_TO_RAD);
	offset_gyroY = (float)(gysum/imax*DEG_TO_RAD);
	offset_gyroZ = (float)(gzsum/imax*DEG_TO_RAD);
	/* Suppression de l'hypothèse que l'assiette initiale est horizontale
	offset_accelX= (float)(axsum/imax)*G;
	offset_accelY= (float)(aysum/imax)*G;
	offset_accelZ= (float)(azsum/imax)*G-G;
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
		float my=QMC5883L::mag_vector[1]*cos(roll)- QMC5883L::mag_vector[2]*sin(roll);
		float m2=(QMC5883L::mag_vector[0]*QMC5883L::mag_vector[0]+QMC5883L::mag_vector[1]*QMC5883L::mag_vector[1]+QMC5883L::mag_vector[2]*QMC5883L::mag_vector[2]);
		float mz=-sin(pitch)*QMC5883L::mag_vector[0]+sin(roll)*cos(pitch)*QMC5883L::mag_vector[1]+cos(roll)*cos(pitch)*QMC5883L::mag_vector[2];
		float mh2=m2-mz*mz;
		float mx=sqrt(mh2-my*my);
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
	q0=((cos((float)roll/2.0)*cos((float)pitch/2.0)*cos((float)yaw/2.0)+sin((float)roll/2.0)*sin((float)pitch/2.0)*sin((float)yaw/2.0)));
	q1=((sin((float)roll/2.0)*cos((float)pitch/2.0)*cos((float)yaw/2.0)-cos((float)roll/2.0)*sin((float)pitch/2.0)*sin((float)yaw/2.0)));
	q2=((cos((float)roll/2.0)*sin((float)pitch/2.0)*cos((float)yaw/2.0)+sin((float)roll/2.0)*cos((float)pitch/2.0)*sin((float)yaw/2.0)));
	q3=((cos((float)roll/2.0)*cos((float)pitch/2.0)*sin((float)yaw/2.0)-sin((float)roll/2.0)*sin((float)pitch/2.0)*cos((float)yaw/2.0)));
	ESP_LOGI(FNAME, "Quaternion:%f,%f,%f,%f ", q0,q1,q2,q3);

	initdone=true;
}
void IMU::read(){
	float TAS=0.0;
	float dt=0;
	bool ret=false;
	  float  roll=0.0;
	  float  pitch=0.0;
	  float  yaw=0.0;
	  float n=1.0;
		uint64_t rts = esp_timer_get_time();
		if( last_rts == 0 )
			ret=true;
		dt = (float)(rts - last_rts)/1000000.0;
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
		n=filterAccelZ;
		TAS = getTAS()/3.6;//TAS en m/s???
		if (TAS>10.0) {angleOfAttack = (Masse / Surface * n * G / (0.5 * 1.227 * TAS * TAS)) / CLa + CL0;} else {angleOfAttack=CL0;}

        Af = Af + betaf * (Ground_Speed_gps - Vf) / dt;
        Vf = Vf + alphaf * (Ground_Speed_gps - Vf) + Af * dt;
		adj_accel(angleOfAttack);
		// Acquisition du vecteur magnetomètre
		//Pour l'instant, les mesures magnéto ne sont ni filtrées, ni recalées dans le temps et en géométrie par rapport à l'IMU
		mX = QMC5883L::mag_vector[0];
		mY = QMC5883L::mag_vector[1];
		mZ = QMC5883L::mag_vector[2];
		MadgwickAHRSupdate(dt,gyroX, gyroY, gyroZ, gravity_vector_plane[0], gravity_vector_plane[1], gravity_vector_plane[2], mX, mY, mZ,&q0,&q1,&q2,&q3);
	// Calcul des anges d'Euler
	  roll = atan2(2 * (q0 * q1 + q2 * q3), (q0*q0-q1*q1-q2*q2+q3*q3)); // phi
	  if (abs(2*( q0 * q2-q1 * q3 ))<1.0) pitch = -asin(2 * ( -q0 * q2+q1 * q3 )); else pitch = 0.0; // theta
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

// Calcul de l'accélération absolue
	accel_earthX = -(accelX*(q0q0+q1q1-q2q2-q3q3)+accelY*2.0*(q1q2-q0q3)+accelZ*2.0*(q1q3+q0q2));
	accel_earthY = -(accelX*2.0*(q1q2+q0q3)+accelY*(q0q0-q1q1+q2q2-q3q3)+accelZ*2.0*(q2q3-q0q1));
	accel_earthZ = -(accelX*2.0*(q1q3-q0q2)+accelY*2.0*(q2q3+q0q1)+accelZ*(q0q0-q1q1-q2q2+q3q3) - G);//Substract pesanteur
	}
	//ESP_LOGI(FNAME, "attitude,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f ",
				//dt,filterGyroX, filterGyroY, filterGyroZ, filterAccelX, filterAccelY, filterAccelZ,roll,pitch,yaw,q0,q1,q2,q3,
				//gravity_vector_plane[0], gravity_vector_plane[1], gravity_vector_plane[2], accel_earthX, accel_earthY, accel_earthZ);
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

void IMU::PitchFromAccel(float *pitch)
{
	*pitch = atan2(-accelX, accelZ);
}
void IMU::RollPitchFromAccel(float *roll, float *pitch)
{
	// Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
	// atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
	// It is then converted from radians to degrees

	*roll = atan2((float)(accelY) , (float)(accelZ));
	*pitch = atan2((float)(-accelX), (float)(accelZ));

	ESP_LOGD( FNAME,"Accelerometer Roll: %f  Pitch: %f  (y:%f x:%f)", *roll * RAD_TO_DEG, *pitch * RAD_TO_DEG, (float)accelY, (float)accelX );

}

