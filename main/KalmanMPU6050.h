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
#include "quaternion.h"

#ifndef M_PI
#define M_PI 3.14159265359
#endif // M_PI
#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0/M_PI)
#endif // RAD_TO_DEG

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI/180.0)
#endif // DEG_TO_RAD

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
  static void read();

  /**
   * Get the last time data was fetched from the IMU (in microseconds, equivallent to a micros() call)
   * 
   * @returns Last time the data was fetched from the IMU in microseconds (micros())
   */
  static inline uint32_t getLastReadTime() { return lastProcessed; };

  /**
   * Gets the accelerometer raw X reading, as per last read() call.
   * 
   * @returns The accelerometer raw X reading
   */
  static inline double getRawAccelX()  {  return accelX;  };

  /**
   * Gets the accelerometer raw Y reading, as per last read() call.
   * 
   * @returns The accelerometer raw Y reading
   */
  static inline double getRawAccelY()   {   return accelY;   };

  /**
   * Gets the accelerometer raw Z reading, as per last read() call.
   * 
   * @returns The accelerometer raw Z reading
   */
  static inline double getRawAccelZ()  {  	return accelZ;  };

  /**
   * Gets the gyroscope raw X reading, as per last read() call.
   * 
   * @returns The gyroscope raw X reading.
   */
  static inline double getRawGyroX()   {  	return gyroX;  };

  /**
   * Gets the gyroscope raw Y reading, as per last read() call.
   * 
   * @returns The gyroscope raw Y reading.
   */
  static inline double getRawGyroY()   {  	return gyroY;  };

  /**
   * Gets the gyroscope raw Z reading, as per last read() call.
   * 
   * @returns The gyroscope raw Z reading.
   */
  static inline double getRawGyroZ()   {  return gyroZ;  };


  /**
   * Gets the roll (X rotation) in degress from the Kalman Filter.
   * As per last read() call.
   * 
   * @returns The x rotation (roll) in degrees
   */
  static inline double getRoll() {  return filterRoll;  };
  static inline double getRollRad() {  return filterRoll*DEG_TO_RAD;  };

  /**
   * Gets the pitch (Y rotation) in degrees from the Kalman Filter.\
   * 
   * @returns The y rotation (pitch) in degrees
   */
  static inline double getPitch()  {	return -filterPitch;  }
  static inline double getPitchRad()  {	return -filterPitch*DEG_TO_RAD;  }


private:
  static uint32_t lastProcessed;
  static Kalman kalmanX; // Create the Kalman instances
  static Kalman kalmanY;
  static double gyroXAngle, gyroYAngle, gyroZAngle; // Angle calculate using the gyro only

  static double accelX, accelY, accelZ;
  static double gyroX, gyroY, gyroZ;
  static double kalXAngle, kalYAngle;

  static void MPU6050Read();
  static void RollPitchFromAccel(double *roll, double *pitch);
  static void PitchFromAccel(double *pitch);
  static uint64_t last_rts;
  static float   myrolly;
  static float   myrollz;
  static float   myaccroll;
  static double  mypitch;
  static double  filterPitch;
  static double  filterRoll;
  static float   pitchfilter;
  static float   rollfilter;
  static float   filterAccRoll;
  static float   filterGyroRoll;

  static Quaternion quat;

};

#endif // _KalmanMPU6050_H_
