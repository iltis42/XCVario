#include "vector_3d.h"
#pragma once

//#define Quaternionen_Test 1

typedef struct euler_angles {

    float roll;
    float pitch;
    float yaw;

} euler_angles;


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
    
    float getAngle() const;

    static Quaternion product(Quaternion q1, Quaternion q2);
    static Quaternion get_normalized(Quaternion q);
    Quaternion normalize();
    static vector_ijk rotate_vector(vector_ijk v, Quaternion q);
    vector_ijk operator*(const vector_ijk& p) const;
    static Quaternion slerp(Quaternion q1, Quaternion q2, double lambda);
    static Quaternion AlignVectors(const vector_ijk &start, const vector_ijk &dest);

    static void quaternionen_test();

    euler_angles to_euler_angles();
    Quaternion conjugate();
};
