#pragma once

#include "vector_3d.h"
#include <cmath>

#define deg2rad(degrees) ((degrees) * (float(M_PI) / 180.0f))
#define rad2deg(rad) ((rad) * (180.0f / float(M_PI)))

//#define Quaternionen_Test 1

float Compass_atan2( float y, float x );

// Quaternion class in the form:
// q = a + bi + cj + dk

class Quaternion {
public:
    float a;
    float b;
    float c;
    float d;
    Quaternion() : a(1.), b(0), c(0), d(0) {};
    Quaternion(float a, float b, float c, float d);
    Quaternion(const float angle, const vector_ijk& axis);
    Quaternion(Quaternion &&) = default; // Allow std::move
    Quaternion(const Quaternion &) = default;
    Quaternion& operator=(const Quaternion&) = default;
    bool operator==(const Quaternion r) { return a==r.a && b==r.b && c==r.c && d==r.d; };
    
    // API
    float getAngle() const;
    friend Quaternion operator*(const Quaternion& left, const Quaternion& right);
    Quaternion get_normalized() const;
    Quaternion normalize();
    vector_ijk operator*(const vector_ijk& p) const;
    vector_d operator*(const vector_d& p) const;
    friend Quaternion slerp(Quaternion q1, Quaternion q2, double lambda);
    static Quaternion AlignVectors(const vector_ijk &start, const vector_ijk &dest);
    static Quaternion fromRotationMatrix(const vector_d &X, const vector_d &Y);
    EulerAngles toEulerRad();
    Quaternion get_conjugate() const;

    // something like a unit test
    static void quaternionen_test();
};
