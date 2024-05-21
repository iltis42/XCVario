// IMU Library for use with Kalman's filter for the MPU6050.
// Copyright (C) 2018 Pedro F Linhares,
// derivative work from https://github.com/TKJElectronics/KalmanFilter/tree/master/examples/MPU6050,
// by Kristian Lauszus, TKJ Electronics.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef _KalmanMPU6050_H_
#define _KalmanMPU6050_H_

#include "Arduino.h"
#include "vector_3d.h"
#include "SetupMenuSelect.h"
#include "quaternion.h"

class Quaternion;

#ifndef M_PI
#define M_PI 3.14159265359
#endif // M_PI
#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0/M_PI)
#endif // RAD_TO_DEG

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI/180.0)
#endif // DEG_TO_RAD

typedef enum e_imudir { IMU_RIGHT=1, IMU_LEFT=2 } e_imudir_t;

typedef struct kalman_t
{
	double Q_angle;   // Process noise variance for the accelerometer
	double Q_bias;    // Process noise variance for the gyro bias
	double R_measure; // Measurement noise variance - this is actually the variance of the measurement noise

	double angle; // The angle calculated by the Kalman filter - part of the 2x1 state vector
	double bias;  // The gyro bias calculated by the Kalman filter - part of the 2x1 state vector
	double rate;  // Unbiased rate calculated from the rate and the calculated bias - you have to call getAngle to update the rate

	double P[2][2]; // Error covariance matrix - This is a 2x2 matrix
	double K[2];    // Kalman gain - This is a 2x1 vector
	double y;       // Angle difference
	double S;       // Estimate error
} Kalman;

void Kalman_Init(Kalman *kalPointer, double qang=0.001, double qbias=0.003, double rmeas=0.03 );
double Kalman_GetAngle(Kalman *kalPointer, double newAngle, double newRate, double dt);

#define SERIAL_KalmanMPU6050_DEBUG 0 // 1 Enables, 0 Disables

#define RESTRICT_PITCH     // Comment out to restrict roll to ±90deg instead please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf

class IMU
{
public:
  /**
   * Initializes the Wire library, the MPU and the Kalman Filter.
   */
  static void init();

  /**
   * Fetches the IMU data and proccesses it through the Kalman Filter.
   */
  static esp_err_t MPU6050Read();
  static void Process();

  // Accelerometer reading in glider reference and in [g]
  static inline vector_ijk getGliderAccel() { return accel; };

  /**
   * Gets the accelerometer X reading, as per last read() call.
   * 
   * @returns The accelerometer X reading in glider reference
   */
  static inline float getGliderAccelX()  { return accel.a; };

  /**
   * Gets the accelerometer Y reading, as per last read() call.
   * 
   * @returns The accelerometer Y reading
   */
  static inline float getGliderAccelY()   { return accel.b; };

  /**
   * Gets the accelerometer Z reading, as per last read() call.
   * 
   * @returns The accelerometer Z reading
   */
  static inline float getGliderAccelZ()  { return accel.c; };

  // Gets the acceleration along the earth g-vector in [times g]
  static float getVerticalAcceleration();

  // Gets the rotation rate in [red/sec] of the glider around a vertical circling axes
  static float getVerticalOmega();

  // Gyro reading in glider reference and in DPS
  static inline vector_ijk getGliderGyro() { return gyro; };

  /**
   * Gets the gyroscope X reading, as per last read() call.
   * 
   * @returns The gyroscope X reading in glider reference.
   */
  static inline float getGliderGyroX()   { return gyro.a; };
  static inline float getGliderNogateGyroX()   { return nogate_gyro.a; };

  /**
   * Gets the gyroscope Y reading, as per last read() call.
   * 
   * @returns The gyroscope Y reading.
   */
  static inline float getGliderGyroY()   { return gyro.b; };
  static inline float getGliderNogateGyroY()   { return nogate_gyro.b; };

  /**
   * Gets the gyroscope Z reading, as per last read() call.
   * 
   * @returns The gyroscope Z reading.
   */
  static inline float getGliderGyroZ()   { return gyro.c; };
  static inline float getGliderNogateGyroZ()   { return nogate_gyro.c; };

  // Get the last raw gyro reads
  static inline float getRawGyroX()   { return raw_gyro.a; };
  static inline float getRawGyroY()   { return raw_gyro.b; };
  static inline float getRawGyroZ()   { return raw_gyro.c; };

  /**
   * Gets the roll (X rotation) in degress from the Kalman Filter.
   * As per last read() call.
   * 
   * @returns The x rotation (roll) in degrees
   */
  static inline float getRoll() { return float(RAD_TO_DEG) * filterRoll_rad; };
  static inline float getRollRad() { return filterRoll_rad; };

  /**
   * Gets the pitch (Y rotation) in degrees from the Kalman Filter.\
   * 
   * @returns The y rotation (pitch) in degrees
   */
  static inline float getPitch()  { return float(RAD_TO_DEG) * filterPitch_rad; }
  static inline float getPitchRad()  { return filterPitch_rad; }

  // XCSoar uses a 180 deg rotated reference system with Z vector pointing down, so Yaw and Pitch inverted
  // hence only Pitch and Roll is used for XCSoar
  static inline float getXCSPitch()  { return -float(RAD_TO_DEG) * filterPitch_rad;  }
  static inline double getYaw()  { return filterYaw;  }
  
  static inline double getGyroRate()  {	return abs(gyro.a)+abs(gyro.b)+abs(gyro.c); }

  // Reference calibration
  static int getAccelSamplesAndCalib(int side, float &wing_angle );
  static void getGyroSamplesAndZero();
  static void defaultImuReference();
  static void applyImuReference(const float gAA, const Quaternion& basic);
  static void doImuCalibration( SetupMenuSelect *p );
  static inline Quaternion getAHRSQuaternion() { return att_quat; };
  static inline vector_ijk getAHRSVector() { return att_vector; };

private:
  static float getGyroYawDelta();
  static void update_fused_vector(vector_ijk& fused, float gyro_trust, vector_ijk& petal_force, Quaternion& omega_step);
  static Kalman kalmanX; // Create the Kalman instances
  static Kalman kalmanY;
  static Kalman kalmanZ;

  static vector_i   raw_gyro;
  static vector_ijk nogate_gyro;
  static vector_ijk accel;
  static vector_ijk gyro;
  static vector_ijk petal;
  static float  circle_omega;
  static double kalXAngle, kalYAngle;

  static float fallbackToGyro();
  // static void RollPitchFromAccel(double *roll, double *pitch);
  static double PitchFromAccel();
  static double PitchFromAccelRad();
  static uint64_t last_rts;
  static float  filterPitch_rad;
  static float  filterRoll_rad;
  static double  filterYaw;

  static float   fused_yaw;

  static Quaternion att_quat;
  static Quaternion omega_step;
  static vector_ijk att_vector;
  static EulerAngles euler_rad;
};

#endif // _KalmanMPU6050_H_
