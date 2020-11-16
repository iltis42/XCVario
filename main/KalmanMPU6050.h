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
  static uint32_t getLastReadTime();

  /**
   * Gets the accelerometer raw X reading, as per last read() call.
   * 
   * @returns The accelerometer raw X reading
   */
  static double getRawAccelX();

  /**
   * Gets the accelerometer raw Y reading, as per last read() call.
   * 
   * @returns The accelerometer raw Y reading
   */
  static double getRawAccelY();

  /**
   * Gets the accelerometer raw Z reading, as per last read() call.
   * 
   * @returns The accelerometer raw Z reading
   */
  static double getRawAccelZ();

  /**
   * Gets the gyroscope raw X reading, as per last read() call.
   * 
   * @returns The gyroscope raw X reading.
   */
  static double getRawGyroX();

  /**
   * Gets the gyroscope raw Y reading, as per last read() call.
   * 
   * @returns The gyroscope raw Y reading.
   */
  static double getRawGyroY();

  /**
   * Gets the gyroscope raw Z reading, as per last read() call.
   * 
   * @returns The gyroscope raw Z reading.
   */
  static double getRawGyroZ();

  /**
   * Gets the roll (X rotation) in degress from the Kalman Filter.
   * As per last read() call.
   * 
   * @returns The x rotation (roll) in degrees
   */
  static double getRoll();

  /**
   * Gets the pitch (Y rotation) in degrees from the Kalman Filter.\
   * 
   * @returns The y rotation (pitch) in degrees
   */
  static double getPitch();

private:
  static uint32_t lastProcessed;

  static double accelX, accelY, accelZ;
  static double gyroX, gyroY, gyroZ;
  
  static double kalXAngle, kalYAngle;

  static void MPU6050Read();
  static void RollPitchFromAccel(double *roll, double *pitch);
  static void PitchFromAccel(double *pitch);
};

#endif // _KalmanMPU6050_H_
