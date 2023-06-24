#pragma once
#include "quaternion.h"
#include<stdint.h>
#include<stdlib.h>

#define ACCEL_LOWER_LIMIT 6144
#define ACCEL_UPPER_LIMIT 10240

Quaternion quaternion_from_accelerometer(float ax, float ay, float az);
Quaternion quaternion_from_gyro(float wx, float wy, float wz, float time);
Quaternion quaternion_from_compass(float wx, float wy, float wz );
float fusion_coeffecient(vector_ijk virtual_gravity, vector_ijk sensor_gravity, float loadFactor);
vector_ijk sensor_gravity_normalized(float ax, float ay, float az);
vector_ijk fuse_vector(vector_ijk virtual_gravity, vector_ijk sensor_gravity, float loadFactor);
vector_ijk update_gravity_vector(vector_ijk gravity_vector,float wx,float wy,float wz,float delta);
vector_ijk update_fused_vector(vector_ijk fused_vector, float loadFactor, float ax, float ay, float az,float wx,float wy,float wz,float delta);
float getGyroYawDelta();
