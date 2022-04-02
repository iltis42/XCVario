#include "vector_3d.h"
#pragma once

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

    static Quaternion product(Quaternion q1, Quaternion q2);
    static Quaternion normalize(Quaternion q);
    static vector_ijk   rotate_vector(vector_ijk v, Quaternion q);

    euler_angles to_euler_angles();
    Quaternion conjugate();
};
