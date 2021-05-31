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

extern double q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame

//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickAHRSupdate(double dt,double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz, double *pq0, double *pq1, double *pq2, double *pq3);
void MadgwickAHRSupdateIMU(double dt,double gx, double gy, double gz, double ax, double ay, double az, double *pq0, double *pq1, double *pq2, double *pq3);

#ifdef __cplusplus
}
#endif
#endif
//=====================================================================================================
// End of file
//=====================================================================================================
