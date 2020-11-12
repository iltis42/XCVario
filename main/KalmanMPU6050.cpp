#include "KalmanMPU6050.h"

// #include <Wire.h>
#include "logdef.h"
#include "sensor.h"

#if SERIAL_IMU_DEBUG
#define DEBUG_INIT() Serial.begin(115200)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_TS_PRINT(x)  \
  DEBUG_PRINT_TIMESTAMP(); \
  Serial.print(x)
#define DEBUG_TS_PRINTLN(x) \
  DEBUG_PRINT_TIMESTAMP();  \
  Serial.println(x)
#else
#define DEBUG_INIT()
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_TS_PRINT(x)
#define DEBUG_TS_PRINTLN(x)
#endif // SERIAL_IMU_DEBUG

#ifndef M_PI
#define M_PI 3.14159265359
#endif // M_PI
#ifndef RAD_TO_DEG
#define RAD_TO_DEG 180.0 / M_PI
#endif // RAD_TO_DEG

#define sqr(x) x *x
#define hypotenuse(x, y) sqrt(sqr(x) + sqr(y))

// MPU-6050
#define IMU_ADDR 0x68
#define IMU_ACCEL_XOUT_H 0x3B
#define IMU_REG 0x19
#define IMU_PWR_MGMT_1 0x6B

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

// Kalman Variables

static Kalman kalmanX; // Create the Kalman instances
static Kalman kalmanY;

static double gyroXAngle, gyroYAngle; // Angle calculate using the gyro only

uint32_t IMU::lastProcessed = 0;

double IMU::accelX, IMU::accelY, IMU::accelZ;
double IMU::gyroX,  IMU::gyroY,  IMU::gyroZ;

double IMU::kalXAngle, IMU::kalYAngle;

// Kalman Function Definition

inline void Kalman_Init(Kalman *kalPointer)
{
  /* We will set the variables like so, these can also be tuned by the user */
  kalPointer->Q_angle = 0.001;
  kalPointer->Q_bias = 0.003;
  kalPointer->R_measure = 0.03;

  kalPointer->angle = 0; // Reset the angle
  kalPointer->bias = 0;  // Reset bias

  kalPointer->P[0][0] = 0; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
  kalPointer->P[0][1] = 0;
  kalPointer->P[1][0] = 0;
  kalPointer->P[1][1] = 0;
}

// The angle should be in degrees and the rate should be in degrees per second and the delta time in seconds
inline double Kalman_GetAngle(Kalman *kalPointer,
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
  kalPointer->P[0][0] += dt * (dt * kalPointer->P[1][1] - kalPointer->P[0][1] -
                               kalPointer->P[1][0] + kalPointer->Q_angle);
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

  MPU6050Read();
  // sleep( 0.1 );
  double roll=0;
  double pitch=0;
  // IMU::RollPitchFromAccel(&roll, &pitch);

  kalmanX.angle = roll; // Set starting angle
  kalmanY.angle = pitch;
  gyroXAngle = roll;
  gyroYAngle = pitch;

  lastProcessed = micros();
  ESP_LOGD(FNAME, "Finished IMU setup  gyroYAngle:%f ", gyroYAngle);
}


static uint64_t last_rts=0;

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

  double roll, pitch;
  IMU::RollPitchFromAccel(&roll, &pitch);
  ESP_LOGD( FNAME, "RollPitchFromAccel: roll: %f pitch: %f  dt: %f", roll, pitch, dt );

  double gyroXRate, gyroYRate;
  gyroXRate = (double)gyroX; // is already in deg/s
  gyroYRate = (double)gyroY; // dito

#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalXAngle > 90) ||
      (roll > 90 && kalXAngle < -90))
  {
    kalmanX.angle = roll;
    kalXAngle = roll;
    gyroXAngle = roll;
  }
  else
  {
    kalXAngle = Kalman_GetAngle(&kalmanX, roll, gyroXRate, dt); // Calculate the angle using a Kalman filter
  }

  if (abs(kalXAngle) > 90)
    gyroYRate = -gyroYRate; // Invert rate, so it fits the restriced accelerometer reading
  kalYAngle = Kalman_GetAngle(&kalmanY, pitch, gyroYRate, dt);
#else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalYAngle > 90) ||
      (pitch > 90 && kalYAngle < -90))
  {
    kalmanY.angle = pitch;
    kalYAngle = pitch;
    gyroYAngle = pitch;
    ESP_LOGD( FNAME, "1: gyroXAngle Y:%f", gyroYAngle );
  }
  else
  {
    kalYAngle = Kalman_GetAngle(&kalmanY, pitch, gyroYRate, dt); // Calculate the angle using a Kalman filter
  }

  if (abs(kalYAngle) > 90)
    gyroXRate = -gyroXRate;                                   // Invert rate, so it fits the restriced accelerometer reading
  kalXAngle = Kalman_GetAngle(&kalmanX, roll, gyroXRate, dt); // Calculate the angle using a Kalman filter
#endif

  gyroXAngle += gyroXRate * dt; // Calculate gyro angle without any filter
  gyroYAngle += gyroYRate * dt;
  ESP_LOGD( FNAME, "2: gyroXAngle X:%f  Y:%f", gyroXAngle, gyroYAngle );
  //gyroXAngle += kalmanX.rate * dt; // Calculate gyro angle using the unbiased rate
  //gyroYAngle += kalmanY.rate * dt;

  // Reset the gyro angle when it has drifted too much
  if (gyroXAngle < -180 || gyroXAngle > 180)
    gyroXAngle = kalXAngle;
  if (gyroYAngle < -180 || gyroYAngle > 180) {
    gyroYAngle = kalYAngle;
    ESP_LOGD( FNAME, "3: gyroXAngle Y:%f", gyroYAngle );
  }

  ESP_LOGD( FNAME, "KalAngle X:%f  Y:%f", kalXAngle, kalYAngle );
}

uint32_t IMU::getLastReadTime()
{
  return lastProcessed;
}

double IMU::getRawAccelX()
{
  return accelX;
}

double IMU::getRawAccelY()
{
  return accelY;
}

double IMU::getRawAccelZ()
{
  return accelZ;
}

double IMU::getRawGyroX()
{
  return gyroX;
}

double IMU::getRawGyroY()
{
  return gyroY;
}

double IMU::getRawGyroZ()
{
  return gyroZ;
}

double IMU::getRoll()
{
  return kalXAngle;
}

double IMU::getPitch()
{
  return -kalYAngle;
}

// IMU Function Definition

void IMU::MPU6050Read()
{
  accelX = -accelG[2];
  accelY = accelG[1];
  accelZ = accelG[0];
  gyroX = gyroDPS.x+ox;
  gyroY = gyroDPS.y+oy;
  gyroZ = gyroDPS.z+oz;
}

void IMU::RollPitchFromAccel(double *roll, double *pitch)
{
  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  *roll = atan2((double)accelY, (double)accelZ) * RAD_TO_DEG;
  *pitch = atan((double)-accelX / hypotenuse((double)accelY, (double)accelZ)) * RAD_TO_DEG;
#else  // Eq. 28 and 29
  *roll = atan((double)accel.y / hypotenuse((double)accelX, (double)accelZ)) * RAD_TO_DEG;
  *pitch = atan2((double)-accelX, (double)accelZ) * RAD_TO_DEG;
#endif // RESTRICT_PITCH

  ESP_LOGD( FNAME,"Accelerometer Roll: %f  Pitch: %f  (y:%f x:%f)", *roll, *pitch, (double)accelY, (double)accelX );

}
