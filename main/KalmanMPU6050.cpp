#include "KalmanMPU6050.h"
#include <MPU.hpp>
#include <mpu/math.hpp>
#include "logdef.h"
#include "sensor.h"
#include "quaternion.h"
#include "vector.h"
#include <simplex.h>
#include <vector>

#define sqr(x) x *x
#define hypotenuse(x, y) sqrt(sqr(x) + sqr(y))

// Kalman Variables
Kalman IMU::kalmanX; // Create the Kalman instances
Kalman IMU::kalmanY;
Kalman IMU::kalmanZ;

float  IMU::filterPitch_rad = 0;
float  IMU::filterRoll_rad = 0;
double  IMU::filterYaw = 0;

uint64_t IMU::last_rts=0;
vector_i   IMU::raw_gyro(0,0,0);
vector_ijk IMU::accel(0,0,0);
vector_ijk IMU::gyro(0,0,0);

#define gyroX gyro.a
#define gyroY gyro.b
#define gyroZ gyro.c
double IMU::kalXAngle = 0.0;
double IMU::kalYAngle = 0.0;
float  IMU::fused_yaw = 0;
vector_ijk IMU::petal(0,0,0);


Quaternion IMU::att_quat(0,0,0,0);
Quaternion IMU::omega_step(0,0,0,0);
vector_ijk IMU::att_vector;
EulerAngles IMU::euler_rad;

// Reference calib
static int progress = 0; // bit-wise 0 -> 1 -> 3 -> 0 // start -> right -> left -> finish
static vector_d bob_right_wing, bob_left_wing;
static mpud::axes_t<int> gyro_bias_one, gyro_bias_two;
static Quaternion ref_rot;

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
	att_vector = vector_ijk(0.0,0.0,1.0);
	euler_rad = { 0,0,0 };
	progress = 0;
	bob_right_wing = bob_left_wing = vector_3d<double>();
	gyro_bias_one = gyro_bias_two = mpud::axes_t<int>();
	Quaternion basic_ref = imu_reference.get();
	if ( basic_ref == Quaternion() ) {
		// If unset, set to a rough default
		defaultImuReference();
	}
	else {
		applyImuReference(glider_ground_aa.get(), basic_ref);
	}
	ESP_LOGI(FNAME, "Finished IMU setup");
}

float IMU::getGyroYawDelta()
{
    EulerAngles e = rad2deg(omega_step.toEulerRad());
    // ESP_LOGI(FNAME,"ngrav deg r=%.3f  p=%.3f  y=%.3f ", e.Roll(), e.Pitch(), e.Yaw() );
	return e.Yaw();
}

void IMU::update_fused_vector(vector_ijk& fused, float gyro_trust, vector_ijk& petal_force, Quaternion& omega_step)
{
    // move the previos fused attitude by the new omega step
    vector_ijk virtual_gravity = omega_step * fused;
    virtual_gravity.normalize();
    virtual_gravity *= gyro_trust;
    // ESP_LOGI(FNAME,"fused/virtual %.4f,%.4f,%.4f/%.4f,%.4f,%.4f", fused.a, fused.b, fused.c, virtual_gravity.a, virtual_gravity.b, virtual_gravity.c);

    // fuse the centripetal and gyro estimation
    petal_force.normalize();
    fused = virtual_gravity + petal_force;
    // ESP_LOGI(FNAME,"fused %.4f,%.4f,%.4f", fused.a, fused.b, fused.c);
    fused.normalize();
    // ESP_LOGI(FNAME,"fusedn %.4f,%.4f,%.4f", fused.a, fused.b, fused.c);
}

// Only call when successfully called MPU6050Read() beforehand 
void IMU::Process()
{
	float dt=0;
	bool ret=false;
	uint64_t rts = esp_timer_get_time();
	if( last_rts == 0 )
		ret=true;
	dt = (double)(rts - last_rts)/1000000.0;
	last_rts = rts;
	if( ret )
		return;
	float gravity_trust = 1;

	// create a gyro base rotation axis
	vector_ijk gyro_rad = deg2rad(gyro);
	omega_step = Quaternion::fromGyro(gyro_rad, dt);
	vector_ijk axis;
	float w = omega_step.getAngleAndAxis(axis) * 1.f / dt; // angular speed [rad/sec]
	omega_step.conjugate(); // inverse step
	// ESP_LOGI( FNAME,"Omega: %f axis: %.3f,%.3f,%.3f", w, axis.a, axis.b, axis.c);

	float roll=0, pitch=0;
	if( getTAS() > 10 ){
		float loadFactor = accel.get_norm();
		float lf = loadFactor > 2.0 ? 2.0 : loadFactor;
		loadFactor = lf < 0 ? 0 : lf; // limit to 0..2g
		// the yz portion of w is proportional to the length of YZ portion of the normalized axis.
		float w_yz = w * sqrt(axis.b*axis.b + axis.c*axis.c);
		// tan(roll):= petal force/G = m w v / m g
		float tanw = w_yz * getTAS() / (3.6f * 9.80665f);
		roll = (std::signbit(gyro_rad.c)?1.f:-1.f) * atan( tanw );
		if ( ahrs_roll_check.get() ) {
			// expected extra load c = sqrt(aa+bb) - 1, here a = 1/9.81 x atan, b=1
			float loadz_exp = sqrt(tanw*tanw/(9.80665f*9.80665f)+1.f) - 1.f;
			float loadz_check = (loadz_exp > 0.f) ? std::min(std::max((accel.c-.99f)/loadz_exp,0.f), 1.f) : 0.f;
			// ESP_LOGI( FNAME,"tanw: %f loadexp: %.2f loadf: %.2f c:%.2f", tanw, loadz_exp, loadFactor, loadz_check );
			// Scale according to real experienced load factor with x 0..1
			roll *= loadz_check;
		}
		// get pitch from accelerometer
		pitch = IMU::PitchFromAccelRad();

		// Centripetal forces to keep angle of bank while circling
		petal.a = -sin(pitch);               // Nose down (positive Y turn) results in negative X force
		petal.b = sin(roll)*cos(pitch);      // Rigrht wing down (or positive X roll) results in positive Y force
		petal.c = cos(roll)*cos(pitch);      // Any roll or pitch creates a smaller positive Z, gravity Z is positive
		// trust in gyro at load factors unequal 1 g
		gravity_trust = (ahrs_min_gyro_factor.get() + (ahrs_gyro_factor.get() * ( pow(10, abs(loadFactor-1) * ahrs_dynamic_factor.get()) - 1)));
		// ESP_LOGI( FNAME,"Omega roll: %f Pitch: %f W_yz: %f Gyro Trust: %f", R2D(roll), R2D(pitch), w_yz, gravity_trust );
	}
	else {
		// For still stand centripetal forces are taken from the accelerometer
		petal = accel;
	}
	// ESP_LOGI( FNAME, " ax1:%f ay1:%f az1:%f Gx:%f Gy:%f GZ:%f dT:%f", petal.a, petal.b, petal.c, gyro.a, gyro.b, gyro.c, dt );
	vector_ijk att_prev = att_vector;
	update_fused_vector(att_vector, gravity_trust, petal, omega_step);
	// ESP_LOGI(FNAME,"attv: %.3f %.3f %.3f", att_vector.a, att_vector.b, att_vector.c);
	att_quat = Quaternion::fromAccelerometer(att_vector);
	// ESP_LOGI(FNAME,"attq: %.3f %.3f %.3f %.3f", att_quat.a, att_quat.b, att_quat.c, att_quat.d );
	euler_rad = att_quat.toEulerRad() * -1.f;
	if ( (att_vector-att_prev).get_norm2() > 0.5 ) {
		EulerAngles euler = rad2deg(euler_rad);
		ESP_LOGI( FNAME,"Euler R:%.1f P:%.1f OR:%.1f IMUP:%.1f %.1f@GA(%.3f,%.3f,%.3f)", euler.Roll(), euler.Pitch(), R2D(roll), R2D(pitch), R2D(w), axis.a, axis.b, axis.c );
	}

	// treat gimbal lock, limit to 88 deg
	const float limit = deg2rad(88.);
	if( euler_rad.Roll() > limit )
		euler_rad.setRoll(limit);
	if( euler_rad.Pitch() > limit )
		euler_rad.setPitch(limit);
	if( euler_rad.Roll() < -limit )
		euler_rad.setRoll(-limit);
	if( euler_rad.Pitch() < -limit )
		euler_rad.setPitch(-limit);

	float curh = 0;
	if( compass ){
		bool ok;
		gravity_vector = att_vector;
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
	filterRoll_rad =  euler_rad.Roll();
	filterPitch_rad =  euler_rad.Pitch();

	// ESP_LOGI( FNAME,"GV-Pitch=%.1f  GV-Roll=%.1f filterYaw: %.2f curh: %.2f GX:%.3f GY:%.3f GZ:%.3f AX:%.3f AY:%.3f AZ:%.3f  FP:%.1f FR:%.1f", euler.Pitch(), euler.Roll(), filterYaw, curh, gyro.a,gyro.b,gyro.c, accel.a, accel.b, accel.c, filterPitch_rad, filterRoll_rad  );

}

float IMU::fallbackToGyro(){
	float gyroYaw = getGyroYawDelta();
	fused_yaw +=  gyroYaw;
	return( Vector::normalizeDeg( fused_yaw ) );
}

// Central read of IMU with reotation into the glider reference system
esp_err_t IMU::MPU6050Read()
{
	static vector_ijk prev_accel(1,0,0), prev_gyro(0,0,0);

	// Get new accelerometer values from MPU6050
	mpud::raw_axes_t imuRaw; // holds x, y, z axes as int16
	esp_err_t err = MPU.acceleration(&imuRaw);  // fetch raw data from the registers
	if( err != ESP_OK ) {
		ESP_LOGE(FNAME, "accel I2C error, X:%d Y:%d Z:%d", imuRaw.x, imuRaw.y, imuRaw.z );
	}
	mpud::float_axes_t tmp = mpud::accelGravity(imuRaw, mpud::ACCEL_FS_8G); // raw data to gravity
	vector_ijk tmpvec(tmp.x, tmp.y, tmp.z);

	// Check on irrational changes
	if ( (tmpvec-prev_accel).get_norm2() > 25 ) {
		vector_ijk d(tmpvec-prev_accel); 
		ESP_LOGE(FNAME, "accelaration change > 5 g in 0.2 S:  X:%+.2f Y:%+.2f Z:%+.2f", d.a, d.b, d.c );
		err |= ESP_FAIL;
	}
	else {
		// into glide reference system
		accel = ref_rot * tmpvec;
	}
	prev_accel = tmpvec;

	// Get new gyro values from MPU6050
	err |= MPU.rotation(&imuRaw);       // fetch raw data from the registers
	if( err != ESP_OK ) {
		ESP_LOGE(FNAME, "gyro I2C error, X:%d Y:%d Z:%d",  imuRaw.x, imuRaw.y, imuRaw.z );
	}
	tmp = mpud::gyroDegPerSec(imuRaw, GYRO_FS);  // raw data to º/s
	tmpvec = vector_ijk(tmp.x, tmp.y, tmp.z);

	// Check on irrational changes
	if ( (tmpvec-prev_gyro).get_norm2() > 30000 ) {
		vector_ijk d(tmpvec-prev_gyro); 
		ESP_LOGE(FNAME, "gyro angle >300 deg/s in 0.2 S: X:%+.2f Y:%+.2f Z:%+.2f", d.a, d.b, d.c );
		err |= ESP_FAIL;
	}
	else {
		// Gating ignores Gyro drift < 2 deg per second
		tmpvec.a = abs(tmpvec.a*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 : tmpvec.a;
		tmpvec.b = abs(tmpvec.b*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 : tmpvec.b;
		tmpvec.c = abs(tmpvec.c*ahrs_gyro_cal.get()) < gyro_gating.get() ? 0.0 : tmpvec.c;
		// into glider reference system
		gyro = ref_rot * tmpvec;
		// preserve the raw read-out
		raw_gyro.a = imuRaw.x; raw_gyro.b = imuRaw.y; raw_gyro.b = imuRaw.z;
	}
	prev_gyro = tmpvec;
	return err;
}

double IMU::PitchFromAccel()
{
	return -atan2(accel.a, accel.c) * RAD_TO_DEG;
}

double IMU::PitchFromAccelRad()
{
	return -atan2(accel.a, accel.c);
}

// void IMU::RollPitchFromAccel(double *roll, double *pitch)
// {
// 	// Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
// 	// atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
// 	// It is then converted from radians to degrees

// 	*roll = atan(accel.b / hypotenuse(accel.a, accel.c)) * RAD_TO_DEG;
// 	*pitch = -atan2(accel.a, accel.c) * RAD_TO_DEG;

// 	// ESP_LOGI( FNAME,"Accelerometer Roll: %f  Pitch: %f  (y:%f x:%f)", *roll, *pitch, accel.b, accel.a );
// }


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

// Callback for the two vector samples needed for the reference calibration
void IMU::getAccelSamplesAndCalib(int side)
{
	esp_err_t err;
	vector_d *bob;
	mpud::axes_t<int> *gbias;
	if ( side == 1 ) {
		bob = &bob_right_wing;
		gbias = &gyro_bias_one;
	}
	else if ( side == 2 ) {
		bob = &bob_left_wing;
		gbias = &gyro_bias_two;
	}
	else {
		ESP_LOGI(FNAME, "Wrong wing down parameter %d", side);
		return;
	}

	err = MPU.getMPUSamples(bob->a, bob->b, bob->c, *gbias);
	ESP_LOGI(FNAME, "wing down bob: %f/%f/%f", bob->a, bob->b, bob->c);
	if ( err == 0 ) {
		progress |= side; // accumulate progress
		if ( progress == 0x3 ) {
			// Extract the current bias from wing down measurments
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

			// The inverted rotation we need to apply
			Quaternion basic_reference = Quaternion::fromRotationMatrix(X, Y).get_conjugate();
			// Concatenated with ground angle
			applyImuReference(glider_ground_aa.get(), basic_reference);

			// Save the basic part to nvs storage
			imu_reference.set(basic_reference, false);
			// Save the accel bias
			mpud::raw_axes_t raw_bias(bias.a*-2048., bias.b*-2048., bias.c*-2048.);
			ESP_LOGI(FNAME, "raw  Bias: %d,%d,%d", raw_bias.x, raw_bias.y, raw_bias.z);
			accl_bias.set(raw_bias, false);
			// Reprogam MPU bias
			MPU.setAccelOffset(raw_bias);

			// Additionaly use gyro sample to calc offset and save it
			raw_bias = mpud::raw_axes_t((gyro_bias_one.x + gyro_bias_two.x) / -2,
				(gyro_bias_one.y + gyro_bias_two.y) / -2,
				(gyro_bias_one.z + gyro_bias_two.z) / -2);
			ESP_LOGI(FNAME, "raw  Gyro: %d,%d,%d", raw_bias.x, raw_bias.y, raw_bias.z);
			mpud::raw_axes_t curr_bias = MPU.getGyroOffset();
			ESP_LOGI(FNAME, "curr Gyro: %d,%d,%d", curr_bias.x, curr_bias.y, curr_bias.z);
			raw_bias += curr_bias;
			ESP_LOGI(FNAME, "new  Gyro: %d,%d,%d", raw_bias.x, raw_bias.y, raw_bias.z);
			gyro_bias.set(raw_bias, false);
			// Reprogam MPU bias
			MPU.setGyroOffset(raw_bias);
		}
	}
}

// Setup the rotation for the "upright" and "topdown" vario mounting positions
void IMU::defaultImuReference()
{
	// Revert from calibrated IMU to default mapping, which fits 
	// roughly to an upright or top down installation.
	Quaternion accelDefaultRef = Quaternion(deg2rad(90.0f), vector_ijk(0,1,0)).get_conjugate();

	if ( display_orientation.get() == DISPLAY_TOPDOWN ) {
		accelDefaultRef = Quaternion(deg2rad(180.0f), vector_ijk(1,0,0)) * accelDefaultRef;
	}
	ref_rot = accelDefaultRef;
	imu_reference.set(ref_rot, false); // nvs
	progress = 0; // reset the calibration procedure
}
// Concatenation of ground angle of attack and the basic reference calibration rotation
void IMU::applyImuReference(const float gAA, const Quaternion& basic)
{
	ref_rot = Quaternion(deg2rad(gAA), vector_ijk(0,1,0)) * basic; // rotate positive around Y
	ref_rot.normalize();
}

void IMU::doImuCalibration( SetupMenuSelect *p ){
	p->ucg->setFont( ucg_font_ncenR14_hr, true );
	p->clear();
	p->ucg->setPrintPos( 1, 30 );
	p->ucg->printf( "AHRS calibration" );
	p->ucg->setPrintPos( 1, 60 );
	p->ucg->printf( "Ensure ground is flat," );
	p->ucg->setPrintPos( 1, 90 );
	p->ucg->printf( "with zero inclination." );
	p->ucg->setPrintPos( 1, 120 );
	p->ucg->printf( "Press button to start" );
	while( !p->readSwitch() ){ delay( 100 ); }
	p->clear();
	p->ucg->setPrintPos( 1, 30 );
	p->ucg->printf( "Now put down RIGHT wing" );
	p->ucg->setPrintPos( 1, 60 );
	p->ucg->printf( "on the ground," );
	p->ucg->setPrintPos( 1, 90 );
	p->ucg->printf( "then press button.." );
	while( !p->readSwitch() ){ delay( 100 ); }
	IMU::getAccelSamplesAndCalib(IMU_RIGHT);
	p->clear();
	p->ucg->setPrintPos( 1, 30 );
	p->ucg->printf( "Now put down LEFT wing" );
	p->ucg->setPrintPos( 1, 60 );
	p->ucg->printf( "on the ground," );
	p->ucg->setPrintPos( 1, 90 );
	p->ucg->printf( "then press button.." );
	while( !p->readSwitch() ){ delay( 100 ); }
	IMU::getAccelSamplesAndCalib(IMU_LEFT);
	p->ucg->setPrintPos( 1, 130 );
	p->ucg->printf( "Finished!" );
	delay(2000);
}


