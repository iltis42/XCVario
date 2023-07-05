#include "KalmanMPU6050.h"
#include "logdef.h"
#include "sensor.h"
#include "quaternion.h"
#include "vector_3d.h"
#include "sensor_processing_lib.h"
#include "vector.h"

#define sqr(x) x *x
#define hypotenuse(x, y) sqrt(sqr(x) + sqr(y))

// Kalman Variables
Kalman IMU::kalmanX; // Create the Kalman instances
Kalman IMU::kalmanY;
Kalman IMU::kalmanZ;

double  IMU::filterPitch = 0;
double  IMU::filterRoll = 0;
double  IMU::filterYaw = 0;

uint64_t IMU::last_rts=0;
double IMU::accelX = 0.0;
double IMU::accelY = 0.0;
double IMU::accelZ = 0.0;
double IMU::gyroX = 0.0;
double IMU::gyroY = 0.0;
double IMU::gyroZ = 0.0;
double IMU::kalXAngle = 0.0;
double IMU::kalYAngle = 0.0;
float  IMU::fused_yaw = 0;
float IMU::ax1,IMU::ay1,IMU::az1 = 0.0;
float IMU::positiveG = 1.0;


Quaternion IMU::att_quat(0,0,0,0);
vector_ijk IMU::att_vector;
euler_angles IMU::euler;

vector_ijk gravity_vector( 0,0,-1 );

// Kalman Function Definition

void Kalman_Init(Kalman *kalPointer, double qang, double qbias, double rmeas )
{
	/* We will set the variables like so, these can also be tuned by the user */
	kalPointer->Q_angle = 0.01;
	kalPointer->Q_bias =  0.03;
	kalPointer->R_measure = 0.1;

	kalPointer->angle = 0; // Reset the angle
	kalPointer->bias = 0;  // Reset bias

	kalPointer->P[0][0] = 0; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
	kalPointer->P[0][1] = 0;
	kalPointer->P[1][0] = 0;
	kalPointer->P[1][1] = 0;
}

// The angle should be in degrees and the rate should be in degrees per second and the delta time in seconds
double Kalman_GetAngle(Kalman *kalPointer,
		double newAngle, double newRate, double dt)
{
	// KasBot V2  -  Kalman filter module - http://www.x-firm.com/?page_id=145
	// Modified by Kristian Lauszus
	// See my blog post for more information: http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it

	// Discrete Kalman filter time update equations - Time Update ("Predict")
	// Update xhat - Project the state ahead
	/* Step 1 */
	kalPointer->rate = newRate - kalPointer->bias;
	kalPointer->angle += dt * kalPointer->rate;

	// Update estimation error covariance - Project the error covariance ahead
	/* Step 2 */
	kalPointer->P[0][0] += dt * (dt * kalPointer->P[1][1] - kalPointer->P[0][1] - kalPointer->P[1][0] + kalPointer->Q_angle);
	kalPointer->P[0][1] -= dt * kalPointer->P[1][1];
	kalPointer->P[1][0] -= dt * kalPointer->P[1][1];
	kalPointer->P[1][1] += kalPointer->Q_bias * dt;

	// Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
	// Calculate Kalman gain - Compute the Kalman gain
	/* Step 4 */
	kalPointer->S = kalPointer->P[0][0] + kalPointer->R_measure;
	/* Step 5 */
	kalPointer->K[0] = kalPointer->P[0][0] / kalPointer->S;
	kalPointer->K[1] = kalPointer->P[1][0] / kalPointer->S;

	// Calculate angle and bias - Update estimate with measurement zk (newAngle)
	/* Step 3 */
	kalPointer->y = newAngle - kalPointer->angle;
	/* Step 6 */
	kalPointer->angle += kalPointer->K[0] * kalPointer->y;
	kalPointer->bias += kalPointer->K[1] * kalPointer->y;

	// Calculate estimation error covariance - Update the error covariance
	/* Step 7 */
	kalPointer->P[0][0] -= kalPointer->K[0] * kalPointer->P[0][0];
	kalPointer->P[0][1] -= kalPointer->K[0] * kalPointer->P[0][1];
	kalPointer->P[1][0] -= kalPointer->K[1] * kalPointer->P[0][0];
	kalPointer->P[1][1] -= kalPointer->K[1] * kalPointer->P[0][1];

	return kalPointer->angle;
};

void IMU::init()
{
	Kalman_Init(&kalmanX);
	Kalman_Init(&kalmanY);
	Kalman_Init(&kalmanZ);

	MPU6050Read();
	// sleep( 0.1 );
	double roll=0;
	double pitch=0;

	kalmanX.angle = roll; // Set starting angle
	kalmanY.angle = pitch;

	att_quat = Quaternion(1.0,0.0,0.0,0.0);
	att_vector = vector_ijk(0.0,0.0,-1.0);
	euler = { 0,0,0 };
	ESP_LOGI(FNAME, "Finished IMU setup");
}

double IMU::getRollRad() {
	return filterRoll*DEG_TO_RAD;
}

double IMU::getPitchRad()  {
	return -filterPitch*DEG_TO_RAD;
}

void IMU::read()
{
	double dt=0;
	bool ret=false;
	MPU6050Read();
	uint64_t rts = esp_timer_get_time();
	if( last_rts == 0 )
		ret=true;
	dt = (double)(rts - last_rts)/1000000.0;
	last_rts = rts;
	if( ret )
		return;

	float ax = accelX;
	float ay = accelY;
	float az = accelZ;
	float loadFactor = sqrt( accelX * accelX + accelY * accelY + accelZ * accelZ );
	float lf = loadFactor > 2.0 ? 2.0 : loadFactor;  // limit to +-1g
	lf = lf < 0 ? 0 : lf;
	// to get pitch and roll independent of circling, image pitch and roll values into 3D vector
	float omega = -atan(((D2R(gyroZ)/ cos( D2R(euler.roll))) * (getTAS()/3.6)) / 9.80665);  // removed as this can cause overswing:  cos( D2R(euler.roll) )
	double pitch;
	IMU::PitchFromAccelRad(&pitch);
	// estimate angle of bank from increased acceleration in Z axis
	positiveG += (lf - positiveG)*ahrs_gforce_lp.get();  // some low pass filtering makes sense here
	// only positive G-force is to be considered, curve at negative G is not defined
	if( positiveG < 1.0)
		positiveG = 1.0;
	float groll = acos( 1/positiveG );
	if( omega < 0 ) // estimate sign of acceleration related angle from gyro
		groll = -groll;
	float T = pow( 10, (positiveG-1)/ahrs_gbank_dynamic.get() ) -1;      // merge g load depending angle of bank depending of load factor

	double roll = (omega + groll*T )/(T+1);                              // left turn is left wing down so negative roll
	float Q = abs(R2D(roll))/ahrs_virtg_bank_trust.get();                 // how much we trust in airspeed and omega based virtual gravity, depending on angle of bank
	// Virtual gravity from centripedal forces to keep angle of bank while circling
	ax1 += (sin(pitch) -ax1) * ahrs_virt_g_lowpass.get();                // Nose down (positive Y turn) results in positive X
	ay1 += (-sin(roll)*cos(pitch) -ay1) * ahrs_virt_g_lowpass.get();     // Left wing down (or negative X roll) results in positive Y
	az1 += (-cos(roll)*cos(pitch) -az1) * ahrs_virt_g_lowpass.get();     // Any roll or pitch creates a less negative Z, unaccelerated Z is negative
	// trust in gyro at load factors unequal 1 g
	float gyro_trust = ahrs_min_gyro_factor.get() + (ahrs_gyro_factor.get() * ( pow(10, abs(lf-1) * ahrs_dynamic_factor.get()) - 1));
	// ESP_LOGI( FNAME,"ax:%f ay:%f az:%f  ax1:%f ay1:%f az1:%f GT:%f Q:%f Pitch:%.1f Roll:%.1f GR:%.1f OR:%.1f Y:%f Z:%f", ax,ay,az, ax1, ay1, az1, gyro_trust, Q, R2D(pitch), R2D(roll), R2D(groll), R2D(omega), (ay+ay1*Q)/(Q+1), (az+az1*Q)/(Q+1) );
	// ESP_LOGI( FNAME,"GT:%f Q:%f Pitch:%.1f Roll:%.1f GR:%.1f OR:%.1f Y:%f Z:%f T:%f lf:%f", gyro_trust, Q, R2D(pitch), R2D(roll), R2D(groll), R2D(omega), (ay+ay1*Q)/(Q+1), (az+az1*Q)/(Q+1), T, lf );

	att_vector = update_fused_vector(att_vector, gyro_trust, (ax+ax1*Q)/(Q+1), (ay+ay1*Q)/(Q+1), (az+az1*Q)/(Q+1),D2R(gyroX),D2R(gyroY),D2R(gyroZ),dt);
	att_quat = quaternion_from_accelerometer(att_vector.a,att_vector.b,att_vector.c);
	euler = att_quat.to_euler_angles();
	// ESP_LOGI( FNAME,"omega*tas: %.3f roll:%.1f° pitch:%.1f° GX:%.1f GY%.1f GZ%.1f AX:%.3f AY:%.3f AZ:%.3f (AXO:%.3f AYO:%.3f AZO:%.3f) e-roll:%.1f e-pitch:%.1f gz:%f tas:%.1f", omega*getTAS(), R2D(roll), R2D(pitch), gyroX,gyroY,gyroZ, ax, ay, az, axo, ayo, azo, euler.roll, euler.pitch, gyroZ, getTAS() );

	// treat gimbal lock, limit to 88 deg
	if( euler.roll > 88.0 )
		euler.roll = 88.0;
	if( euler.pitch > 88.0 )
		euler.pitch = 88.0;
	if( euler.roll < -88.0 )
		euler.roll = -88.0;
	if( euler.pitch < -88.0 )
		euler.pitch = -88.0;

	float curh = 0;
	if( compass ){
		bool ok;
		gravity_vector = vector_ijk(att_vector.a,att_vector.b,att_vector.c);
		gravity_vector.normalize();
		curh = compass->cur_heading( &ok );
		if( ok ){
			float gyroYaw = getGyroYawDelta();
			// tuned to plus 7% what gave the best timing swing in response, 2% for compass is far enough
			// gyro and compass are time displaced, gyro comes immediate, compass a second later
			fused_yaw +=  Vector::angleDiffDeg( curh ,fused_yaw )*0.02 + gyroYaw;
			filterYaw=Vector::normalizeDeg( fused_yaw );
			compass->setGyroHeading( filterYaw );
			//ESP_LOGI( FNAME,"cur magn head %.2f gyro yaw: %.4f fused: %.1f Gyro(%.3f/%.3f/%.3f)", curh, gyroYaw, gh, gyroX, gyroY, gyroZ  );
		}else
		{
			filterYaw=fallbackToGyro();
		}
	}
	else{
		filterYaw=fallbackToGyro();
	}
	filterRoll =  euler.roll;
	filterPitch =  euler.pitch;

	// ESP_LOGI( FNAME,"GV-Pitch=%.1f  GV-Roll=%.1f filterYaw: %.2f curh: %.2f GX:%.3f GY:%.3f GZ:%.3f AX:%.3f AY:%.3f AZ:%.3f  FP:%.1f FR:%.1f", euler.pitch, euler.roll, filterYaw, curh, gyroX,gyroY,gyroZ, accelX, accelY, accelZ, filterPitch, filterRoll  );

}

float IMU::fallbackToGyro(){
	float gyroYaw = getGyroYawDelta();
	fused_yaw +=  gyroYaw;
	return( Vector::normalizeDeg( fused_yaw ) );
}

// IMU Function Definition
void IMU::MPU6050Read()
{
	accelX = accelG[2];
	accelY = -accelG[1];
	accelZ = -accelG[0];
	// Gating ignores Gyro drift < 2 deg per second
	gyroX = abs(gyroDPS.z*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 :  -(gyroDPS.z);
	gyroY = abs(gyroDPS.y*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 :   (gyroDPS.y);
	gyroZ = abs(gyroDPS.x*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 :   (gyroDPS.x);
}

void IMU::PitchFromAccel(double *pitch)
{
	*pitch = atan2(accelX, -accelZ) * RAD_TO_DEG;
}

void IMU::PitchFromAccelRad(double *pitch)
{
	*pitch = atan2(accelX, -accelZ);
}

void IMU::RollPitchFromAccel(double *roll, double *pitch)
{
	// Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
	// atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
	// It is then converted from radians to degrees

	*roll = atan((double)accelY / hypotenuse((double)accelX, (double)accelZ)) * RAD_TO_DEG;
	*pitch = atan2((double)accelX, (double)accelZ) * RAD_TO_DEG;

	// ESP_LOGI( FNAME,"Accelerometer Roll: %f  Pitch: %f  (y:%f x:%f)", *roll, *pitch, (double)accelY, (double)accelX );
}
