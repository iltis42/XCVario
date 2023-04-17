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

#ifndef _ahrs_H_
#define _ahrs_H_

#include "Arduino.h"
#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "QMC5883L.h"

#ifndef M_PI
#define M_PI 3.14159265359
#endif // M_PI
#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0/M_PI)
#endif // RAD_TO_DEG

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI/180.0)
#endif // DEG_TO_RAD
//modif gfm
#ifndef G
#define G 9.807
#endif // G
//fin modif gfm

//#define RESTRICT_PITCH     // Comment out to restrict roll to ±90deg instead please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf

class IMU
{
public:
  /**
   * Initializes the Wire library, the MPU and the Kalman Filter.
   */
  static void init(bool f=true);

  /**
   * Fetches the IMU data and proccesses it through the Kalman Filter.
   */
  static void read();

  /**
   * Get the last time data was fetched from the IMU (in microseconds, equivallent to a micros() call)
   *
   * @returns Last time the data was fetched from the IMU in microseconds (micros())
   */
  static inline uint32_t getLastReadTime() { return (lastProcessed); };

  /**
   * Gets the accelerometer raw X reading, as per last read() call.
   *
   * @returns The accelerometer raw X reading
   */
  static inline float getRawAccelX()  {  return (accelX);  };
  static inline float getFilterAccelX()  {  return (filterAccelX);  };

  /**
   * Gets the accelerometer raw Y reading, as per last read() call.
   *
   * @returns The accelerometer raw Y reading
   */
  static inline float getRawAccelY()   {   return (accelY);   };
  static inline float getFilterAccelY()   {   return (filterAccelY);   };

  /**
   * Gets the accelerometer raw Z reading, as per last read() call.
   *
   * @returns The accelerometer raw Z reading
   */
  static inline float getRawAccelZ()  {  	return (accelZ);  };
  static inline float getFilterAccelZ()  {  	return (filterAccelZ);  };

  /**
   * Gets the gyroscope raw X reading, as per last read() call.
   *
   * @returns The gyroscope raw X reading.
   */
  static inline float getRawGyroX()   {  	return (gyroX);  };
  static inline float getFilterGyroX()   {  	return (filterGyroX);  };

  /**
   * Gets the gyroscope raw Y reading, as per last read() call.
   *
   * @returns The gyroscope raw Y reading.
   */
  static inline float getRawGyroY()   {  	return (gyroY);  };
  static inline float getFilterGyroY()   {  	return (filterGyroY);  };

  /**
   * Gets the gyroscope raw Z reading, as per last read() call.
   *
   * @returns The gyroscope raw Z reading.
   */
  static inline float getRawGyroZ()   {  return (gyroZ);  };
  static inline float getFilterGyroZ()   {  return (filterGyroZ);  };
  //modif gfm
  static inline float getEarthAccelX() { return (accel_earthX);};
  static inline float getEarthAccelY() { return (accel_earthY);};
  static inline float getEarthAccelZ() { return (accel_earthZ);};
//fin modif gfm

  /**
   * Gets the roll (X rotation) in degress from the Kalman Filter.
   * As per last read() call.
   *
   * @returns The x rotation (roll) in degrees
   */
  static inline float getRoll() {  return filterRoll*RAD_TO_DEG;  };
  static inline float getRollRad() {  return filterRoll;  };

  /**
   * Gets the pitch (Y rotation) in degrees from the Kalman Filter.\
   *
   * @returns The y rotation (pitch) in degrees
   */
  static inline float getPitch()  {	return filterPitch*RAD_TO_DEG;  };
  static inline float getPitchRad()  {	return filterPitch;  };
  static inline float getYawRad()  {	return filterYaw;  };
  static inline bool getInitdone() {return (initdone);};

private:
  static uint32_t lastProcessed;

  static float accelX, accelY, accelZ;
  static float offset_accelX, offset_accelY, offset_accelZ;
  static float accel_earthX, accel_earthY, accel_earthZ;
  static float gyroX, gyroY, gyroZ;
  static float mX, mY, mZ;
  static float offset_gyroX, offset_gyroY, offset_gyroZ;

  static void MPU6050Read();
  static void RollPitchFromAccel(float *roll, float *pitch);
  static void PitchFromAccel(float *pitch);
  static uint64_t last_rts;

  static float  offset_roll;
  static float  offset_pitch;
  static float  offset_yaw;
  static bool initdone;
  static float  filterPitch;
  static float  filterRoll;
  static float  filterYaw;
  static float  filterAccelX;
  static float  filterAccelY;
  static float  filterAccelZ;
  static float  filterGyroX;
  static float  filterGyroY;
  static float  filterGyroZ;
};

#endif // _ahrs_H_
