#include "quaternion.h"
#include "logdef.h"


Quaternion::Quaternion(float _a, float _b, float _c, float _d)
{
    a = _a;
    b = _b;
    c = _c;
    d = _d;
}

Quaternion Quaternion::product(Quaternion q1, Quaternion q2)
{
    //q = q1*q2
    Quaternion q( (q1.a*q2.a) - (q1.b*q2.b) - (q1.c*q2.c) - (q1.d*q2.d),
                  (q1.a*q2.b) + (q1.b*q2.a) + (q1.c*q2.d) - (q1.d*q2.c),
                  (q1.a*q2.c) - (q1.b*q2.d) + (q1.c*q2.a) + (q1.d*q2.b),
                  (q1.a*q2.d) + (q1.b*q2.c) - (q1.c*q2.b) + (q1.d*q2.a) );
    return q;
}

Quaternion Quaternion::conjugate()
{
    Quaternion q2( a, -b, -c, -d );
    return q2;
}

Quaternion Quaternion::normalize(Quaternion q1)
{
    float one_by_sqrt;
    one_by_sqrt = InvSqrt(q1.a*q1.a + q1.b*q1.b + q1.c*q1.c + q1.d*q1.d);
    Quaternion q2( q1.a*one_by_sqrt, q1.b*one_by_sqrt, q1.c*one_by_sqrt, q1.d*one_by_sqrt );
    // ESP_LOGI(FNAME,"Q1: a=%.3f b=%.3f c=%.3f d=%.3f  Q2: a=%.3f b=%.3f c=%.3f d=%.3f obs:%f", q1.a, q1.b, q1.c, q1.d, q2.a, q2.b, q2.c, q2.d, one_by_sqrt );
    return q2;
}


vector_ijk Quaternion::rotate_vector(vector_ijk v, Quaternion q)
{
    Quaternion vector = Quaternion(0.0, v.a, v.b, v.c);
    Quaternion q_inverse = q.conjugate();
    Quaternion rotated_vector = product(product(q, vector),q_inverse);
    vector_ijk rotated(rotated_vector.b,rotated_vector.c,rotated_vector.d);
    return rotated;
}

euler_angles Quaternion::to_euler_angles()
{
    euler_angles result;
    double q0 = a;
    double q1 = b;
    double q2 = c;
    double q3 = d;
    result.roll = atan2(2*(q0*q1 + q2*q3),1 - 2*(q1*q1 + q2*q2))*180/3.14;
    result.pitch = asin(2*(q0*q2 - q3*q1))*180/3.14;
    if (d==0)
        result.yaw = 0.0;
    else
        result.yaw = atan2(2*(q0*q3 + q1*q2),1 - 2*(q2*q2 + q3*q3))*180/3.14;
    return result;
}
