#include "sensor_processing_lib.h"
#include "logdef.h"
#include "SetupNG.h"

Quaternion quaternion_from_accelerometer(float ax, float ay, float az)
{
	// ESP_LOGI(FNAME,"ax=%.3f ay=%.3f az=%.3f", ax, ay, az);
    /*vector_ijk gravity = vector_3d_initialize(0.0f, 0.0f, -1.0f);
    vector_ijk accelerometer = vector_3d_initialize(ax, ay, az);
    Quaternion orientation = between_vectors(gravity,accelerometer);
    return orientation;*/
    // float norm_u_norm_v = 1.0;
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

float fusion_coeffecient(vector_ijk virtual_gravity, vector_ijk sensor_gravity, float loadFactor )
{
    vector_ijk v = sensor_gravity;
    float lf = loadFactor > 2.0 ? 2.0 : loadFactor;  // limit to +-1g
    lf = lf < -1.0 ? -1.0 : lf;
    float trust = ahrs_min_gyro_factor.get() + ahrs_gyro_factor.get() * ( pow(10, abs(lf-1) * ahrs_dynamic_factor.get()) - 1);
    // ESP_LOGI(FNAME,"LF= %f trust= %f", loadFactor, trust);
    return trust;
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

vector_ijk fuse_vector(vector_ijk virtual_gravity, vector_ijk sensor_gravity, float loadFactor )
{
    float fusion = fusion_coeffecient(virtual_gravity, sensor_gravity, loadFactor);
    virtual_gravity.scale(fusion);
    vector_ijk result = virtual_gravity;
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

vector_ijk update_fused_vector(vector_ijk fused_vector, float ax, float ay, float az,float wx,float wy,float wz,float delta)
{
    vector_ijk virtual_gravity = update_gravity_vector(fused_vector,wx,wy,wz,delta);
    vector_ijk sensor_gravity = sensor_gravity_normalized(ax,ay,az);
    float loadFactor = sqrt( ax * ax + ay * ay + az * az );
    // ESP_LOGI(FNAME,"ax=%f ay=%f az=%f lf=%f", ax, ay, az, loadFactor );
    fused_vector = fuse_vector(virtual_gravity,sensor_gravity, loadFactor);
    return fused_vector;
}
