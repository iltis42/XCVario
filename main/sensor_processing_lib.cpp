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
    float cos_theta = -1.0*az;
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
    b = alpha*(-wx);
    c = alpha*(-wy);
    d = alpha*(-wz);
    a = 1 - 0.5*(b*b+c*c+d*d);
    Quaternion result(a,b,c,d);
    return result;
}

Quaternion quaternion_from_compass(float wx, float wy, float wz )
{
	float alpha = 1;
	float a,b,c,d;
	b = alpha*(-wz);
	c = alpha*(wy);
	d = alpha*(wx);
	a = 1;
	Quaternion result(a,b,c,d);
	return result;

}

float fusion_coeffecient(vector_ijk virtual_gravity, vector_ijk sensor_gravity)
{
    vector_ijk v = sensor_gravity;
    float dot = v.dot_product(virtual_gravity);
    // ESP_LOGI(FNAME,"dot: %f ax: %.3f ay: %.3f az:%.3f", dot, virtual_gravity.a, virtual_gravity.b, virtual_gravity.c );
    if (dot<=0.96){
       return ahrs_gyro_factor.get();   // if both are close, trust more gyro
    }
    return ahrs_gyro_factor.get()/4;
}

vector_ijk sensor_gravity_normalized(int16_t ax, int16_t ay, int16_t az)
{
    vector_ijk result;
    result.a = ax;
    result.b = ay;
    result.c = az;
    result.normalize();
    return result;
}

vector_ijk fuse_vector(vector_ijk virtual_gravity, vector_ijk sensor_gravity)
{
    float fusion = fusion_coeffecient(virtual_gravity, sensor_gravity);
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
    gravity_vector = Quaternion::rotate_vector(gravity_vector, q_gyro);
    gravity_vector.normalize();
    return gravity_vector;
}

vector_ijk update_fused_vector(vector_ijk fused_vector, int16_t ax, int16_t ay, int16_t az,float wx,float wy,float wz,float delta)
{
	// ESP_LOGI(FNAME,"ax=%d ay=%d az=%d", ax, ay, az);
    vector_ijk virtual_gravity = update_gravity_vector(fused_vector,wx,wy,wz,delta);
    vector_ijk sensor_gravity = sensor_gravity_normalized(ax,ay,az);
    fused_vector = fuse_vector(virtual_gravity,sensor_gravity);
    return fused_vector;
}
