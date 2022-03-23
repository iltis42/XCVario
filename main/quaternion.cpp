#include "quaternion.h"
#include "logdef.h"


// basic constructor
Quaternion::Quaternion(float _a, float _b, float _c, float _d)
{
    a = _a;
    b = _b;
    c = _c;
    d = _d;
}

// should be the cross product
Quaternion Quaternion::product(Quaternion q1, Quaternion q2)
{
    //q = q1*q2
    Quaternion q( (q1.a*q2.a) - (q1.b*q2.b) - (q1.c*q2.c) - (q1.d*q2.d),
                  (q1.a*q2.b) + (q1.b*q2.a) + (q1.c*q2.d) - (q1.d*q2.c),
                  (q1.a*q2.c) - (q1.b*q2.d) + (q1.c*q2.a) + (q1.d*q2.b),
                  (q1.a*q2.d) + (q1.b*q2.c) - (q1.c*q2.b) + (q1.d*q2.a) );
    return q;
}

// return the conjugated quaternion
Quaternion Quaternion::conjugate()
{
    Quaternion q2( a, -b, -c, -d );
    return q2;
}

// Spherically interpolates between quaternions q1 and q2 by ratio lambda.
// The parameter lambda is clamped to the range [0, 1].
// Use this to create a rotation which smoothly interpolates between the first
// quaternion q1 to the second quaternion q2, based on the value of the parameter lambda.
// If the value of the parameter is close to 0, the output will be close to q1,
// if it is close to 1, the output will be close to q2.

Quaternion Quaternion::slerp(Quaternion q1, Quaternion q2, double lambda)
{
	float dotproduct = q1.b * q2.b + q1.c * q2.c + q1.d * q2.d + q1.a * q2.a;
	float theta, st, sut, sout, coeff1, coeff2;
	// algorithm adapted from Shoemake's paper
	lambda=lambda/2.0;

	theta = (float) acos(dotproduct);
	if (theta<0.0) theta=-theta;

	st = (float) sin(theta);
	sut = (float) sin(lambda*theta);
	sout = (float) sin((1-lambda)*theta);
	coeff1 = sout/st;
	coeff2 = sut/st;

	Quaternion qr( coeff1*q1.a + coeff2*q2.a, coeff1*q1.b + coeff2*q2.b, coeff1*q1.c + coeff2*q2.c, coeff1*q1.d + coeff2*q2.d );
	return( normalize( qr ) );
}

// Normalize quaternion to magnitude of 1
Quaternion Quaternion::normalize(Quaternion q1)
{
    float one_by_sqrt;
    one_by_sqrt = InvSqrt(q1.a*q1.a + q1.b*q1.b + q1.c*q1.c + q1.d*q1.d);
    Quaternion q2( q1.a*one_by_sqrt, q1.b*one_by_sqrt, q1.c*one_by_sqrt, q1.d*one_by_sqrt );
    // ESP_LOGI(FNAME,"Q1: a=%.3f b=%.3f c=%.3f d=%.3f  Q2: a=%.3f b=%.3f c=%.3f d=%.3f obs:%f", q1.a, q1.b, q1.c, q1.d, q2.a, q2.b, q2.c, q2.d, one_by_sqrt );
    return q2;
}

// rotate vector v by quaternion defined by q
vector_ijk Quaternion::rotate_vector(vector_ijk v, Quaternion q)
{
    Quaternion vector = Quaternion(0.0, v.a, v.b, v.c);
    Quaternion q_inverse = q.conjugate();
    Quaternion rotated_vector = product(product(q, vector),q_inverse);
    vector_ijk rotated(rotated_vector.b,rotated_vector.c,rotated_vector.d);
    return rotated;
}
// return euler angles yaw, pitch, roll as degree from quaternion
euler_angles Quaternion::to_euler_angles()
{
    euler_angles result;
    double q0 = a;
    double q1 = b;
    double q2 = c;
    double q3 = d;
    result.roll = atan2(2*(q0*q1 + q2*q3),1 - 2*(q1*q1 + q2*q2))*180/M_PI;
    result.pitch = asin(2*(q0*q2 - q3*q1))*180/M_PI;
    if (d==0)
        result.yaw = 0.0;
    else
        result.yaw = atan2(2*(q0*q3 + q1*q2),1 - 2*(q2*q2 + q3*q3))*180/M_PI;
    return result;
}
