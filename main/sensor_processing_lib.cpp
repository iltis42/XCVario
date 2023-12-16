#include "sensor_processing_lib.h"
#include "logdef.h"
#include "SetupNG.h"

Quaternion quaternion_from_accelerometer(const vector_ijk& a)
{
	// ESP_LOGI(FNAME,"ax=%.3f ay=%.3f az=%.3f", ax, ay, az);
    float half_cos = sqrt(0.5*(1.0 + a.c));
    float temp = 2.*half_cos;
    Quaternion orientation( half_cos, -a.b/temp, a.a/temp, 0.0 );
    return orientation;
}

static Quaternion quaternion_from_gyro(vector_ijk w, float time)
{
    // wx,wy,wz in radians per second: time in seconds
    float alpha = 0.5*time;
    float a,b,c,d;
    b = alpha*(w.a);
    c = alpha*(w.b);
    d = alpha*(w.c);
    a = 1 - 0.5*(b*b+c*c+d*d);
    Quaternion result(a,b,c,d);

    // // ESP_LOGI(FNAME,"Quat: %.3f %.3f %.3f", w.a, w.b, w.c );
    // // omega=(alpha,beta,gamma)
    // // theta = ||omega||*dt; //length of angular velocity vector
    // float norm = w.normalize(); // normalized orientation of angular velocity vector
    // float theta_05 = norm * 0.5 * time;
    // Quaternion result(cos(theta_05), w.a * sin(theta_05), w.b * sin(theta_05), w.c * sin(theta_05));
    // ESP_LOGI(FNAME,"Quat: %.3f %.3f %.3f %.3f", result.a, result.b, result.c, result.d );
    return result;
}

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
    euler_angles e = q_gyro.to_euler_angles();
    // ESP_LOGI(FNAME,"e.yaw=%.3f ", e.yaw );
    gyro_yaw_delta = e.yaw;
    vector_ijk virtual_gravity = q_gyro * gravity_vector;
    virtual_gravity.normalize();
    return virtual_gravity;
}

void update_fused_vector(vector_ijk& fused_vector, float gyro_trust, float ax, float ay, float az, const vector_ijk& w, float delta)
{
	// ESP_LOGI(FNAME,"UFV ax=%f ay=%f az=%f trust=%f, gx=%f gy=%f gz=%f", ax, ay, az, gyro_trust, wx,wy,wz );
    vector_ijk virtual_gravity = virtual_gravity_vector(fused_vector, w, delta);
    vector_ijk sensor_gravity = sensor_gravity_normalized(ax,ay,az);
    fused_vector = fuse_vector(virtual_gravity, sensor_gravity, gyro_trust);
}
