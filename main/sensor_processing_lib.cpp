#include "sensor_processing_lib.h"
#include "logdef.h"
#include "SetupNG.h"


// Quaternion quaternion_from_compass(float wx, float wy, float wz )
// {
// 	// float a = 1; 
// 	// - 0.5*(wx*wx+wy*wy+wz*wz);

// 	Quaternion result(0,wx,wy,wz);
// 	return result;
// }

// float fusion_coeffecient(vector_ijk virtual_gravity, vector_ijk sensor_gravity, float gyro_trust )
// {
//     // ESP_LOGI(FNAME,"trust= %f", gyro_trust);
//     return gyro_trust;
// }

static vector_ijk sensor_gravity_normalized(float ax, float ay, float az)
{
    vector_ijk result;
    result.a = ax;
    result.b = ay;
    result.c = az;
    result.normalize();
    return result;
}

static vector_ijk fuse_vector(vector_ijk virtual_gyro_gravity, vector_ijk sensor_gravity, float gyro_trust )
{
    // float fusion = fusion_coeffecient(virtual_gyro_gravity, sensor_gravity, gyro_trust);
    // virtual_gyro_gravity *= fusion;
    virtual_gyro_gravity *= gyro_trust;
    virtual_gyro_gravity += sensor_gravity;
    virtual_gyro_gravity.normalize();
    return virtual_gyro_gravity;
}

static float gyro_yaw_delta = 0;

float getGyroYawDelta(){
	return gyro_yaw_delta;
}

static vector_ijk virtual_gravity_vector(const vector_ijk& gravity_vector, const vector_ijk& w,float delta)
{
    Quaternion q_gyro = quaternion_from_gyro(w, delta);
    EulerAngles e = rad2deg(non_gravity.toEulerRad());
    // ESP_LOGI(FNAME,"e.yaw=%.3f ", e.yaw );
    gyro_yaw_delta = e.Yaw();
    vector_ijk virtual_gravity = q_gyro * gravity_vector;
    virtual_gravity.normalize();
    return virtual_gravity;
}

void update_fused_vector(vector_ijk& fused_vector, float gyro_trust, const vector_ijk& a1, const vector_ijk& w, float delta)
{
	// ESP_LOGI(FNAME,"UFV ax=%f ay=%f az=%f trust=%f, gx=%f gy=%f gz=%f", ax, ay, az, gyro_trust, wx,wy,wz );
    vector_ijk virtual_gravity = virtual_gravity_vector(fused_vector, w, delta);
    fused_vector = fuse_vector(virutal_gravity, a1, gyro_trust);
}
