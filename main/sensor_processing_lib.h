#ifdef __cplusplus
extern "C" {
#endif

#ifndef SENSOR_PROCESSING_INCLUDED
#define SENSOR_PROCESSING_INCLUDED

#include "quaternion.h"
#include<stdint.h>
#include<stdlib.h>

#define ACCEL_LOWER_LIMIT 6144
#define ACCEL_UPPER_LIMIT 10240

Quaternion quaternion_from_accelerometer(float ax, float ay, float az);
Quaternion quaternion_from_gyro(float wx, float wy, float wz, float time);
float fusion_coeffecient(vector_ijk virtual_gravity, vector_ijk sensor_gravity);
vector_ijk sensor_gravity_normalized(int16_t ax, int16_t ay, int16_t az);
vector_ijk fuse_vector(vector_ijk virtual_gravity, vector_ijk sensor_gravity);
vector_ijk update_gravity_vector(vector_ijk gravity_vector,float wx,float wy,float wz,float delta);
vector_ijk update_fused_vector(vector_ijk fused_vector, int16_t ax, int16_t ay, int16_t az,float wx,float wy,float wz,float delta);

#endif // SENSOR_FUSION_LIB_H_INCLUDED

#ifdef __cplusplus
}
#endif
