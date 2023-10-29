#include "KalmanMPU6050.h"
#include "logdef.h"
#include "sensor.h"
#include "quaternion.h"
#include "sensor_processing_lib.h"
#include "vector.h"
#include <simplex.h>
#include <vector>

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
vector_d IMU::accel(0,0,0);
vector_d IMU::gyro(0,0,0);

#define accelX accel.a
#define accelY accel.b
#define accelZ accel.c
#define gyroX gyro.a
#define gyroY gyro.b
#define gyroZ gyro.c
double IMU::kalXAngle = 0.0;
double IMU::kalYAngle = 0.0;
float  IMU::fused_yaw = 0;
float IMU::ax1,IMU::ay1,IMU::az1 = 0.0;
float IMU::positiveG = 1.0;


Quaternion IMU::att_quat(0,0,0,0);
vector_ijk IMU::att_vector;
euler_angles IMU::euler;
int IMU::progress = 0;
vector_3d<double> IMU::bob_right_wing(0,0,0),
	IMU::bob_left_wing(0,0,0);
Quaternion IMU::ref_rot;

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
	progress = 0;
	bob_right_wing = bob_left_wing = vector_3d<double>();
	ref_rot = imu_reference.get();
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
	float gravity_trust = 1;
	double roll = 0;
	if( getTAS() > 10 ){
		float loadFactor = accel.get_norm(); // (hjr) Only the Z axes should be relevant here?!
		float lf = loadFactor > 2.0 ? 2.0 : loadFactor;
		loadFactor = lf < 0 ? 0 : lf; // limit to 0..2g
		// to get pitch and roll independent of circling, image pitch and roll values into 3D vector
		roll = -atan(((D2R(gyroZ) /cos( D2R(euler.roll)) ) * (getTAS()/3.6)) / 9.80665);
		double pitch;
		IMU::PitchFromAccelRad(&pitch);

		// Virtual gravity from centripedal forces to keep angle of bank while circling
		ax1 = sin(pitch);                // Nose down (positive Y turn) results in positive X
		ay1 = -sin(roll)*cos(pitch);     // Left wing down (or negative X roll) results in positive Y
		az1 = -cos(roll)*cos(pitch);     // Any roll or pitch creates a less negative Z, unaccelerated Z is negative
		// trust in gyro at load factors unequal 1 g
		gravity_trust = (ahrs_min_gyro_factor.get() + (ahrs_gyro_factor.get() * ( pow(10, abs(loadFactor-1) * ahrs_dynamic_factor.get()) - 1)));
		// ESP_LOGI( FNAME,"Omega roll: %f Pitch: %f Gyro Trust: %f", R2D(roll), R2D(pitch), gravity_trust );
	}
	else{
		ax1=accel.a;
		ay1=accel.b;
		az1=accel.c;
	}
	// ESP_LOGI( FNAME, " ax1:%f ay1:%f az1:%f Gx:%f Gy:%f GZ:%f GRT:%f Roll:%.1f ", ax1, ay1, az1, gyroX, gyroY, gyroZ, gravity_trust, R2D(roll) );
	att_vector = update_fused_vector(att_vector, gravity_trust, ax1, ay1, az1, D2R(gyroX),D2R(gyroY),D2R(gyroZ),dt);
	att_quat = quaternion_from_accelerometer(att_vector.a,att_vector.b,att_vector.c);
	euler = att_quat.to_euler_angles();
	// ESP_LOGI( FNAME,"Euler R:%.1f P:%.1f T:%f Q:%f GT:%f OR:%f GR:%f R:%f", euler.roll, euler.pitch, T, Q, gyro_trust, R2D(omega), R2D(groll), R2D(roll) );

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
	vector_d tmp(accelG[0],accelG[1], accelG[2]);
	accel = ref_rot * tmp;
	// Gating ignores Gyro drift < 2 deg per second
	// gyroX = abs(gyroDPS.z*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 :  -(gyroDPS.z);
	// gyroY = abs(gyroDPS.y*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 :   (gyroDPS.y);
	// gyroZ = abs(gyroDPS.x*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 :   (gyroDPS.x);
	tmp.a = abs(gyroDPS.x*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 : gyroDPS.x;
	tmp.b = abs(gyroDPS.y*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 : gyroDPS.y;
	tmp.c = abs(gyroDPS.z*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 : gyroDPS.z;
	gyro = ref_rot * tmp;
	ESP_LOGI(FNAME,"XYZ:\t%f\t%f\t%f \tL%.2f", accel.a, accel.b, accel.c, accel.get_norm());
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


// IMU reference calibration
class IMU_Ref
{
    public:
	void setBr(vector_d& p) { Br=p; }
	void setBl(vector_d& p) { Bl=p; }
    double operator()(const std::vector<double> x) const
   {
		const vector_d tmp(x[0], x[1], x[2]);
        double Nr = (Br - tmp).get_norm();  // |Mr-B|
		double Nl = (Bl - tmp).get_norm();

		// ESP_LOGI(FNAME, "iter: %f,%f,%f", x[0],x[1],x[2]);
        return pow(Nr-1,2) + pow(Nl-1,2) + pow(Nl- Nr,2)/10.;
    }
	private:
	vector_d Br, Bl;
};

void IMU::getAccelSamplesAndCalib(int side)
{
	esp_err_t err;
	vector_d *bob = nullptr;
	if ( side == 1 ) {
		bob = &bob_right_wing;
	}
	else if ( side == 2 ) {
		bob = &bob_left_wing;
	}
	else {
		ESP_LOGI(FNAME, "Wrong wing down parameter %d", side);
		return;
	}

	err = MPU.getAccelSamplesG(bob->a, bob->b, bob->c);
	ESP_LOGI(FNAME, "wing down bob: %f/%f/%f", bob->a, bob->b, bob->c);
	if ( err == 0 ) {
		progress |= side;
		if ( progress == 0x3 ) {
			// Ectract the current bias from wing down measurments
			std::vector<double> start{.0, .0, .0};
			std::vector<std::vector<double> > imu_simp{{0.05,0,0},{0,-0.05,0},{0,0,0.05},{0,0,0}};
			IMU_Ref bias_min;
			bias_min.setBr(bob_right_wing);
			bias_min.setBl(bob_left_wing);
			std::vector<double> x = BT::Simplex(bias_min, start, 1e-10, imu_simp);
			vector_d bias(x[0],x[1],x[2]);
			ESP_LOGI(FNAME, "Bias: %f,%f,%f", x[0],x[1],x[2]);

			// Calculate the rotation to the glieder reference from the two measurments
			// Corrected wing down bob vectores
			vector_d pureBr = bob_right_wing - bias;
			vector_d pureBl = bob_left_wing - bias;
			ESP_LOGI(FNAME,"pureBr:\t%f\t%f\t%f \tL%.2f", pureBr.a, pureBr.b, pureBr.c, pureBr.get_norm());
			ESP_LOGI(FNAME,"pureBl:\t%f\t%f\t%f \tL%.2f", pureBl.a, pureBl.b, pureBl.c, pureBl.get_norm());

			// A vector from skid touch points towards the main wheel touch point
			// The X in glider reference (points towards the nose)
			vector_d X = pureBr.cross(pureBl); // Br x Bl
			X.normalize();
			ESP_LOGI(FNAME, "X: %f,%f,%f", X.a, X.b, X.c);
			// The Z in glider reference
			// The bob in glider ref, skid stil on the ground (points up)
			vector_d Z(pureBr+pureBl);
			Z.normalize();
			// The Y in glider reference
			vector_d Y = Z.cross(X);
			Y.normalize();
			ESP_LOGI(FNAME, "Y: %f,%f,%f", Y.a, Y.b, Y.c);
			ESP_LOGI(FNAME, "Z: %f,%f,%f", Z.a, Z.b, Z.c);

			// Correct the result by the ground angle of attac
			Quaternion rot_groundAA(glider_ground_aa.get(), vector_ijk(0,1,0)); // rotate positive around Y
			ref_rot = Quaternion::fromRotationMatrix(X, Y).get_conjugate();// * rot_groundAA;
			ref_rot.normalize();

			// Save to nvs storage
			imu_reference.set(ref_rot, false);
			mpud::raw_axes_t raw_bias(bias.a*-2048., bias.b*-2048., bias.c*-2048.);
			accl_bias.set(raw_bias, false);
			// Reprogam MPU bias
			MPU.setAccelOffset(raw_bias);

		}
	}
}

void IMU::defaultImuReference(bool topDown)
{
	// Revert from calibrated IMU to default mapping, which fits 
	// roughly to an upright or top down installation.
	Quaternion accelDefaultRef = Quaternion(90.0f, vector_ijk(0,1,0)).get_conjugate();
	ESP_LOGI(FNAME, "default ref: %f %f %f %f a:%f", accelDefaultRef.a, accelDefaultRef.b, accelDefaultRef.c, accelDefaultRef.d, accelDefaultRef.getAngle()*180./M_PI );

	if ( topDown ) {
		accelDefaultRef = accelDefaultRef * Quaternion(180.0f, vector_ijk(1,0,0));
	}
	ref_rot = accelDefaultRef; //Quaternion();
	imu_reference.set(ref_rot, false);
	progress = 0;
}
