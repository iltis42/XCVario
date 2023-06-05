#include "vector_3d.h"
#pragma once

//#define Quaternionen_Test 1

typedef struct euler_angles {

    float roll;
    float pitch;
    float yaw;

} euler_angles;

float Compass_atan2( float y, float x );

// Quaternion class in the form:
// q = a + bi + cj + dk

class Quaternion {
public:
    float a;
    float b;
    float c;
    float d;
    Quaternion(float a, float b, float c, float d);
    Quaternion(const float angle, const vector_ijk& axis);
    Quaternion(Quaternion &&) = default; // Allow std::move
    Quaternion(const Quaternion &) = default;
    Quaternion& operator=(const Quaternion&) = default;
    
    // API
    float getAngle() const;
    friend Quaternion operator*(const Quaternion& left, const Quaternion& right);
    Quaternion get_normalized() const;
    Quaternion normalize();
    vector_ijk operator*(const vector_ijk& p) const;
    friend Quaternion slerp(Quaternion q1, Quaternion q2, double lambda);
    static Quaternion AlignVectors(const vector_ijk &start, const vector_ijk &dest);
    euler_angles to_euler_angles();
    Quaternion get_conjugate() const;

    // something like a unit test
    static void quaternionen_test();
};
