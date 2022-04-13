#include "quaternion.h"
#include <logdef.h>
#include <esp_timer.h>

#define M_PIf 3.14159265358979323846f
#define degrees_to_radians(degrees) ((degrees) * M_PIf / 180.0f)
#define radians_to_degrees(rad) ((rad) * 180.0f / M_PIf)


// basic constructor
Quaternion::Quaternion(float _a, float _b, float _c, float _d)
{
    a = _a;
    b = _b;
    c = _c;
    d = _d;
}

Quaternion::Quaternion(const float angle, const vector_ijk& axis)
{
    float fac = std::sin(0.5 * angle);

    a = std::cos(0.5 * angle);
    b = fac * axis.a;
    c = fac * axis.b;
    d = fac * axis.c;
}

float Quaternion::getAngle() const
{
    return 2.f * std::acos(a);
}

// should be the cross product
Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
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
	return( get_normalized( qr ) );
}

// Normalize quaternion to magnitude of 1
Quaternion Quaternion::get_normalized(Quaternion q1)
{
    float len = sqrt(q1.a*q1.a + q1.b*q1.b + q1.c*q1.c + q1.d*q1.d);
    Quaternion q2( q1.a/len, q1.b/len, q1.c/len, q1.d/len );
    // ESP_LOGI(FNAME,"Q1: a=%.3f b=%.3f c=%.3f d=%.3f  Q2: a=%.3f b=%.3f c=%.3f d=%.3f obs:%f", q1.a, q1.b, q1.c, q1.d, q2.a, q2.b, q2.c, q2.d, one_by_sqrt );
    return q2;
}
Quaternion Quaternion::normalize()
{
    float len = sqrt(a*a + b*b + c*c + d*d);
    a = a/len;
    b = b/len;
    c = c/len;
    d =d/len;
    return *this;
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
vector_ijk Quaternion::operator*(const vector_ijk& vec) const
{
    float a00 = a * a;
    float a01 = a * b;
    float a02 = a * c;
    float a03 = a * d;
    float a11 = b * b;
    float a12 = b * c;
    float a13 = b * d;
    float a22 = c * c;
    float a23 = c * d;
    float a33 = d * d;
    vector_ijk result;
    result.a = vec.a * (+a00 + a11 - a22 - a33)
        + 2.0 * (a12 * vec.b + a13 * vec.c + a02 * vec.c - a03 * vec.b);
    result.b = vec.b * (+a00 - a11 + a22 - a33)
        + 2.0 * (a12 * vec.a + a23 * vec.c + a03 * vec.a - a01 * vec.c);
    result.c = vec.c * (+a00 - a11 - a22 + a33)
        + 2.0 * (a13 * vec.a + a23 * vec.b - a02 * vec.a + a01 * vec.b);
    return result;
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

Quaternion Quaternion::AlignVectors(const vector_ijk &start, const vector_ijk &dest)
{
	vector_ijk from = start;
    from.normalize_f();
	vector_ijk to = dest;
    to.normalize_f();

	float cosTheta = from.dot_product(to);
	vector_ijk rotationAxis;

	if (cosTheta < -1 + 0.001f){
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = from.cross(vector_ijk(0.0f, 0.0f, 1.0f));
		if (rotationAxis.get_norm2() < 0.01 ) // bad luck, they were parallel, try again!
			rotationAxis = from.cross(vector_ijk(1.0f, 0.0f, 0.0f));

		rotationAxis.normalize_f();
		return Quaternion(degrees_to_radians(180.0f), rotationAxis);
	}

	rotationAxis = from.cross(to);

	float s = sqrt( (1.f+cosTheta)*2.f );

	return Quaternion(
		s * 0.5f, 
		rotationAxis.a / s,
		rotationAxis.b / s,
		rotationAxis.c / s
	);

}

// Grad
float Compass_atan2( float y, float x )
{
    float result = radians_to_degrees(atan2(y, x));
    
    // As left turn means plus, euler angles come with 0° for north, -90° for east, -+180 degree for south and for 90° west
    // compass rose goes vice versa, so east is 90° means we need to invert
    if ( std::signbit(y) ) {
        result += 360.f;
    }

    return result;
} 

#ifdef Quaternionen_Test
void Quaternion::quaternionen_test()
{
    vector_ijk v1(1,0,0);
    vector_ijk v2(0,0,1), vt(1,2,3), v3;

    // v to v setup
    int64_t t0 = esp_timer_get_time();
    Quaternion q = Quaternion::AlignVectors(v1,v2);
    int64_t t1 = esp_timer_get_time();
    ESP_LOGI(FNAME,"Align v1 to v2: (%f,%f,%f) - (%f,%f,%f)", v1.a, v1.b, v1.c, v2.a, v2.b, v2.c );
    ESP_LOGI(FNAME,"Q: %lld - %f %f %f %f a:%f", t1-t0, q.a, q.b, q.c, q.d, radians_to_degrees(q.getAngle()) );

    // rotate
    v3 = rotate_vector(v1, q);
    ESP_LOGI(FNAME,"rotate v1 -> v2: %f %f %f", v3.a, v3.b, v3.c );

    // slerp
    Quaternion q2 = Quaternion::AlignVectors(v1,vector_ijk(0.707106781, 0, 0.707106781));
    ESP_LOGI(FNAME,"Q: %f %f %f %f a:%f", q2.a, q2.b, q2.c, q2.d, radians_to_degrees(q2.getAngle()) );
    Quaternion qs = slerp(q, q2, 1.f);
    ESP_LOGI(FNAME,"slerp: %f %f %f %f a:%f", qs.a, qs.b, qs.c, qs.d, radians_to_degrees(qs.getAngle()) );

    // compass atan2
    ESP_LOGI(FNAME,"Check compass atan2");
    ESP_LOGI(FNAME,"  0? %f", Compass_atan2(0, 0));
    ESP_LOGI(FNAME,"  0: %f", Compass_atan2(0, 1));
    ESP_LOGI(FNAME," 45: %f", Compass_atan2(1, 1));
    ESP_LOGI(FNAME," 45: %f", Compass_atan2(0.6, 0.6)); // norm?
    ESP_LOGI(FNAME," 90: %f", Compass_atan2(1., 0.));
    ESP_LOGI(FNAME,"135: %f", Compass_atan2(0.6, -0.6));
    ESP_LOGI(FNAME,"180: %f", Compass_atan2(0., -1.));
    ESP_LOGI(FNAME,"180: %f", Compass_atan2(-0.00001, -1.));
    ESP_LOGI(FNAME,"225: %f", Compass_atan2(-0.6, -0.6));
    ESP_LOGI(FNAME,"270: %f", Compass_atan2(-1., 0.));
    ESP_LOGI(FNAME,"315: %f", Compass_atan2(-0.6, 0.6));
    // Comp rotate
    v3 = rotate_vector(vt, q);
    ESP_LOGI(FNAME,"rotate vt: %f %f %f", v3.a, v3.b, v3.c );
    v3 = q * vt;
    ESP_LOGI(FNAME,"op*    vt: %f %f %f", v3.a, v3.b, v3.c );

}

#endif
