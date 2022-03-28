//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h
//#include "logdef.h"
#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------------------------------------------------------------------
// Variable declaration

extern float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame

//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickAHRSupdate(float dt,float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float *pq0, float *pq1, float *pq2, float *pq3);
void MadgwickAHRSupdateIMU(float dt,float gx, float gy, float gz, float ax, float ay, float az, float *pq0, float *pq1, float *pq2, float *pq3);

#ifdef __cplusplus
}
#endif
#endif
//=====================================================================================================
// End of file
//=====================================================================================================
