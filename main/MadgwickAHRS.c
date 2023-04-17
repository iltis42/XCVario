/*
//=====================================================================================================
// MadgwickAHRS.c
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalised
//
//=====================================================================================================

//---------------------------------------------------------------------------------------------------
// Header files
*/
#include "MadgwickAHRS.h"
#include <math.h>

/*---------------------------------------------------------------------------------------------------
// Definitions
*/
#define beta_quat		0.1f		/* 2 * proportional gain*/

/*====================================================================================================
// Functions

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update
*/
void MadgwickAHRSupdate(float dt,float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float *pq0, float *pq1, float *pq2, float *pq3) {
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float hx, hy;
	float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	/* Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)*/
	if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
		MadgwickAHRSupdateIMU(dt,gx, gy, gz, ax, ay, az, &q0, &q1, &q2, &q3);
		return;
	}

	/* Rate of change of quaternion from gyroscope*/
	qDot1 = 0.5f * (-*pq1 * gx - *pq2 * gy - *pq3 * gz);
	qDot2 = 0.5f * (*pq0 * gx + *pq2 * gz - *pq3 * gy);
	qDot3 = 0.5f * (*pq0 * gy - *pq1 * gz + *pq3 * gx);
	qDot4 = 0.5f * (*pq0 * gz + *pq1 * gy - *pq2 * gx);

	/* Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)*/
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		/* Normalise accelerometer measurement*/
		recipNorm = 1.0f / sqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;   

		/* Normalise magnetometer measurement*/
		recipNorm = 1.0f / sqrt(mx * mx + my * my + mz * mz);
		mx *= recipNorm;
		my *= recipNorm;
		mz *= recipNorm;

		/* Auxiliary variables to avoid repeated arithmetic*/
		_2q0mx = 2.0f * *pq0 * mx;
		_2q0my = 2.0f * *pq0 * my;
		_2q0mz = 2.0f * *pq0 * mz;
		_2q1mx = 2.0f * *pq1 * mx;
		_2q0 = 2.0f * *pq0;
		_2q1 = 2.0f * *pq1;
		_2q2 = 2.0f * *pq2;
		_2q3 = 2.0f * *pq3;
		_2q0q2 = 2.0f * *pq0 * *pq2;
		_2q2q3 = 2.0f * *pq2 * *pq3;
		q0q0 = *pq0 * *pq0;
		q0q1 = *pq0 * *pq1;
		q0q2 = *pq0 * *pq2;
		q0q3 = *pq0 * *pq3;
		q1q1 = *pq1 * *pq1;
		q1q2 = *pq1 * *pq2;
		q1q3 = *pq1 * *pq3;
		q2q2 = *pq2 * *pq2;
		q2q3 = *pq2 * *pq3;
		q3q3 = *pq3 * *pq3;

		/* Reference direction of Earth's magnetic field*/
		hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
		hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
		_2bx = sqrt(hx * hx + hy * hy);
		_2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
		_4bx = 2.0f * _2bx;
		_4bz = 2.0f * _2bz;

		/* Gradient decent algorithm corrective step*/
		s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		recipNorm =  1.0f / sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); /* normalise step magnitude*/
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		/* Apply feedback step*/
		qDot1 -= beta_quat * s0;
		qDot2 -= beta_quat * s1;
		qDot3 -= beta_quat * s2;
		qDot4 -= beta_quat * s3;
	}

	/* Integrate rate of change of quaternion to yield quaternion*/
	*pq0 += qDot1 * dt;
	*pq1 += qDot2 * dt;
	*pq2 += qDot3 * dt;
	*pq3 += qDot4 * dt;

	/* Normalise quaternion*/
	recipNorm = 1.0f / sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	*pq0 *= recipNorm;
	*pq1 *= recipNorm;
	*pq2 *= recipNorm;
	*pq3 *= recipNorm;
}

/*---------------------------------------------------------------------------------------------------
// IMU algorithm update
*/
void MadgwickAHRSupdateIMU(float dt,float gx, float gy, float gz, float ax, float ay, float az, float *pq0, float *pq1, float *pq2, float *pq3) {
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

//	ESP_LOGI(FNAME, "Début MAg:%f,%f,%f,%f,%f,%f ", gx,ax,*pq0,*pq1,*pq2,*pq3);
	/* Rate of change of quaternion from gyroscope*/
	qDot1 = 0.5f * (-*pq1 * gx - *pq2 * gy - *pq3 * gz);
	qDot2 = 0.5f * (*pq0 * gx + *pq2 * gz - *pq3 * gy);
	qDot3 = 0.5f * (*pq0 * gy - *pq1 * gz + *pq3 * gx);
	qDot4 = 0.5f * (*pq0 * gz + *pq1 * gy - *pq2 * gx);

	/* Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)*/
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		/* Normalise accelerometer measurement*/
		recipNorm = 1.0f / sqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;   

		/* Auxiliary variables to avoid repeated arithmetic*/
		_2q0 = 2.0f * *pq0;
		_2q1 = 2.0f * *pq1;
		_2q2 = 2.0f * *pq2;
		_2q3 = 2.0f * *pq3;
		_4q0 = 4.0f * *pq0;
		_4q1 = 4.0f * *pq1;
		_4q2 = 4.0f * *pq2;
		_8q1 = 8.0f * *pq1;
		_8q2 = 8.0f * *pq2;
		q0q0 = *pq0 * *pq0;
		q1q1 = *pq1 * *pq1;
		q2q2 = *pq2 * *pq2;
		q3q3 = *pq3 * *pq3;

		/* Gradient decent algorithm corrective step*/
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
		recipNorm = 1.0f / sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); /* normalise step magnitude*/
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		/* Apply feedback step*/
		qDot1 -= beta_quat * s0;
		qDot2 -= beta_quat * s1;
		qDot3 -= beta_quat * s2;
		qDot4 -= beta_quat * s3;
	}

	 /*Integrate rate of change of quaternion to yield quaternion*/
	*pq0 += qDot1 *  dt;
	*pq1 += qDot2 *  dt;
	*pq2 += qDot3 *  dt;
	*pq3 += qDot4 *  dt;

	/* Normalise quaternion*/
	recipNorm = 1.0f/ sqrt(*pq0 * *pq0 + *pq1 * *pq1 + *pq2 * *pq2 + *pq3 * *pq3);
	*pq0 *= recipNorm;
	*pq1 *= recipNorm;
	*pq2 *= recipNorm;
	*pq3 *= recipNorm;
}

/*---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(float*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return (y);
}
*/
/*====================================================================================================
// END OF CODE
//====================================================================================================*/
