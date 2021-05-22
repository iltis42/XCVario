/*
 * ahrs.h
 *
 *  Created on: 22 mai 2021
 *      Author: GFM
 */

#ifndef MAIN_AHRS_H_
#define MAIN_AHRS_H_

#include "Arduino.h"

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
#define G 9.806
#endif // G
//fin modif gfm

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
  static inline uint32_t getLastReadTime() { return (lastProcessed); };

  /**
   * Gets the accelerometer raw X reading, as per last read() call.
   *
   * @returns The accelerometer raw X reading
   */
  static inline double getRawAccelX()  {  return (accelX);  };

  /**
   * Gets the accelerometer raw Y reading, as per last read() call.
   *
   * @returns The accelerometer raw Y reading
   */
  static inline double getRawAccelY()   {   return (accelY);   };

  /**
   * Gets the accelerometer raw Z reading, as per last read() call.
   *
   * @returns The accelerometer raw Z reading
   */
  static inline double getRawAccelZ()  {  	return (accelZ);  };

  /**
   * Gets the gyroscope raw X reading, as per last read() call.
   *
   * @returns The gyroscope raw X reading.
   */
  static inline double getRawGyroX()   {  	return (gyroX);  };

  /**
   * Gets the gyroscope raw Y reading, as per last read() call.
   *
   * @returns The gyroscope raw Y reading.
   */
  static inline double getRawGyroY()   {  	return (gyroY);  };

  /**
   * Gets the gyroscope raw Z reading, as per last read() call.
   *
   * @returns The gyroscope raw Z reading.
   */
  static inline double getRawGyroZ()   {  return (gyroZ);  };
  //modif gfm
  #define cphi cos((double)getRollRad())
  #define sphi sin((double)getRollRad())
  #define cteta cos((double)getPitchRad())
  #define steta sin((double)getPitchRad())

  static inline double getEarthAccelX() {return ( (accelX*(cteta*cphi+sphi)+accelY*(-cteta*sphi+cphi)-accelZ*sphi)*G );}
  static inline double getEarthAccelY() {return ( (accelX*sphi+accelY*cphi)*G );}
  static inline double getEarthAccelZ() {return ( (accelX*(steta*cphi-sphi)+accelY*(steta*sphi+cphi)+accelZ*cteta - 1.0f)*G );}//Substract pesanteur
//fin modif gfm

  /**
   * Gets the roll (X rotation) in degress from the Kalman Filter.
   * As per last read() call.
   *
   * @returns The x rotation (roll) in degrees
   */
  static inline double getRoll() {  return (filterRoll);  };
  static inline double getRollRad() {  return (filterRoll*DEG_TO_RAD);  };

  /**
   * Gets the pitch (Y rotation) in degrees from the Kalman Filter.\
   *
   * @returns The y rotation (pitch) in degrees
   */
  static inline double getPitch()  {	return (-filterPitch);  }
  static inline double getPitchRad()  {	return (-filterPitch*DEG_TO_RAD);  }


private:
  static uint32_t lastProcessed;
  static double gyroXAngle, gyroYAngle; /* Angle calculate using the gyro only*/

  static double accelX, accelY, accelZ;
  static double accel_earthX, accel_earthY, accel_earthZ;
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

};



#endif /* MAIN_AHRS_H_ */
