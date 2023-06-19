#include "sensor_processing_lib.h"
#include "logdef.h"
#include "SetupNG.h"

Quaternion quaternion_from_accelerometer(float ax, float ay, float az)
{
	// ESP_LOGI(FNAME,"ax=%.3f ay=%.3f az=%.3f", ax, ay, az);
    float cos_theta = -az;
    //float half_cos = sqrt(0.5*(1.0 + cos_theta));
    float half_cos = 0.7071*sqrt(1.0 + cos_theta);
    float temp = 0.5/half_cos;
    Quaternion orientation( half_cos, -ay*temp, ax*temp, 0.0 );
    return orientation;
}

Quaternion quaternion_from_gyro(float wx, float wy, float wz, float time)
{
    // wx,wy,wz in radians per second: time in seconds
    float alpha = 0.5*time;
    float a,b,c,d;
    b = -alpha*(wx);
    c = -alpha*(wy);
    d = -alpha*(wz);
    a = 1 - 0.5*(b*b+c*c+d*d);
    Quaternion result(a,b,c,d);
    return result;
}

Quaternion quaternion_from_compass(float wx, float wy, float wz )
{
	// float a = 1;
	// - 0.5*(wx*wx+wy*wy+wz*wz);

	Quaternion result(0,wx,wy,wz);
	return result;
}

float fusion_coeffecient(vector_ijk virtual_gravity, vector_ijk sensor_gravity, float gyro_trust )
{
    // ESP_LOGI(FNAME,"trust= %f", gyro_trust);
    return gyro_trust;
}

vector_ijk sensor_gravity_normalized(float ax, float ay, float az)
{
    vector_ijk result;
    result.a = ax;
    result.b = ay;
    result.c = az;
    result.normalize();
    return result;
}

vector_ijk fuse_vector(vector_ijk virtual_gyro_gravity, vector_ijk sensor_gravity, float gyro_trust )
{
    float fusion = fusion_coeffecient(virtual_gyro_gravity, sensor_gravity, gyro_trust);
    virtual_gyro_gravity.scale(fusion);
    vector_ijk result = virtual_gyro_gravity;
    result.sum(sensor_gravity);
    result.normalize();
    return result;
}

static float gyro_yaw_delta = 0;

float getGyroYawDelta(){
	return gyro_yaw_delta;
}

vector_ijk update_gravity_vector(vector_ijk gravity_vector,float wx,float wy,float wz,float delta)
{
    Quaternion q_gyro = quaternion_from_gyro(wx,wy,wz,delta);
    euler_angles e = q_gyro.to_euler_angles();
    // ESP_LOGI(FNAME,"e.yaw=%.3f ", e.yaw );
    gyro_yaw_delta = e.yaw;
    gravity_vector = q_gyro * gravity_vector;
    gravity_vector.normalize();
    return gravity_vector;
}

vector_ijk update_fused_vector(vector_ijk fused_vector, float gyro_trust, float ax, float ay, float az,float wx,float wy,float wz,float delta)
{
	// ESP_LOGI(FNAME,"UFV ax=%f ay=%f az=%f trust=%f, gx=%f gy=%f gz=%f", ax, ay, az, gyro_trust, wx,wy,wz );
    vector_ijk virtual_gravity = update_gravity_vector(fused_vector,wx,wy,wz,delta);
    vector_ijk sensor_gravity = sensor_gravity_normalized(ax,ay,az);
    fused_vector = fuse_vector(virtual_gravity,sensor_gravity, gyro_trust);
    return fused_vector;
}
