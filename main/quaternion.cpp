#include "quaternion.h"
#include <logdef.h>
#include <esp_timer.h>



// basic constructor
Quaternion::Quaternion(float _a, float _b, float _c, float _d)
{
    a = _a;
    b = _b;
    c = _c;
    d = _d;
}

// rotate radian angle around axis
Quaternion::Quaternion(const float angle, const vector_ijk& axis)
{
    float fac = std::sin(0.5 * angle);

    a = std::cos(0.5 * angle);
    b = fac * axis.a;
    c = fac * axis.b;
    d = fac * axis.c;
}
// radian
float Quaternion::getAngle() const
{
    return 2.f * std::acos(a);
}

// radions and a normalized vector
float Quaternion::getAngleAndAxis(vector_ijk& axis) const
{
    float angle = getAngle();
    float sinphi2 = fabs(angle) > 1e-7 ? 1.0f / sin(0.5f * angle) : 0.0;

    // null rotation -> return null vector
    axis.a = b;
    axis.b = c;
    axis.c = d;
    axis *= sinphi2;
    // ESP_LOGI(FNAME,"naxisv: %.3f", axis.get_norm() );

    return angle;
}


// concatenatin of two quaternions
Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
    // q = q1*q2 -- IMPORTANT: read the "*" operator as "after"
    //                         q1*q2 is first rotating q2, then q1 (!)
    Quaternion q( (q1.a*q2.a) - (q1.b*q2.b) - (q1.c*q2.c) - (q1.d*q2.d),
                  (q1.a*q2.b) + (q1.b*q2.a) + (q1.c*q2.d) - (q1.d*q2.c),
                  (q1.a*q2.c) - (q1.b*q2.d) + (q1.c*q2.a) + (q1.d*q2.b),
                  (q1.a*q2.d) + (q1.b*q2.c) - (q1.c*q2.b) + (q1.d*q2.a) );
    return q;
}

// Spherically interpolates between quaternions q1 and q2 by ratio lambda.
// The parameter lambda is clamped to the range [0, 1].
// Use this to create a rotation which smoothly interpolates between the first
// quaternion q1 to the second quaternion q2, based on the value of the parameter lambda.
// If the value of the parameter is close to 0, the output will be close to q1,
// if it is close to 1, the output will be close to q2.

Quaternion slerp(Quaternion q1, Quaternion q2, double lambda)
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
	return qr.normalize();
}

// Get a normalized version of quaternion
Quaternion Quaternion::get_normalized() const
{
    float len = sqrt(a*a + b*b + c*c + d*d);
    Quaternion q2( a/len, b/len, c/len, d/len );
    // ESP_LOGI(FNAME,"Q1: a=%.3f b=%.3f c=%.3f d=%.3f  Q2: a=%.3f b=%.3f c=%.3f d=%.3f", a, b, c, d, q2.a, q2.b, q2.c, q2.d );
    return q2;
}
// Normalize quaternion
Quaternion& Quaternion::normalize()
{
    float len = sqrt(a*a + b*b + c*c + d*d);
    a = a/len;
    b = b/len;
    c = c/len;
    d =d/len;
    return *this;
}

// conjugat quaternion
Quaternion& Quaternion::conjugate()
{
    b = -b;
    c = -c;
    d = -d;
    return *this;
}

// return the conjugated quaternion
Quaternion Quaternion::get_conjugate() const
{
    Quaternion q2( a, -b, -c, -d );
    return q2;
}

// rotate vector v by quaternion
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
        + 2.0f * (a12 * vec.b + a13 * vec.c + a02 * vec.c - a03 * vec.b);
    result.b = vec.b * (+a00 - a11 + a22 - a33)
        + 2.0f * (a12 * vec.a + a23 * vec.c + a03 * vec.a - a01 * vec.c);
    result.c = vec.c * (+a00 - a11 - a22 + a33)
        + 2.0f * (a13 * vec.a + a23 * vec.b - a02 * vec.a + a01 * vec.b);
    return result;
}
vector_d Quaternion::operator*(const vector_d& vec) const
{
    double a00 = a * a;
    double a01 = a * b;
    double a02 = a * c;
    double a03 = a * d;
    double a11 = b * b;
    double a12 = b * c;
    double a13 = b * d;
    double a22 = c * c;
    double a23 = c * d;
    double a33 = d * d;
    vector_d result;
    result.a = vec.a * (+a00 + a11 - a22 - a33)
        + 2.0 * (a12 * vec.b + a13 * vec.c + a02 * vec.c - a03 * vec.b);
    result.b = vec.b * (+a00 - a11 + a22 - a33)
        + 2.0 * (a12 * vec.a + a23 * vec.c + a03 * vec.a - a01 * vec.c);
    result.c = vec.c * (+a00 - a11 - a22 + a33)
        + 2.0 * (a13 * vec.a + a23 * vec.b - a02 * vec.a + a01 * vec.b);
    return result;
}

// return euler angles roll, pitch, yaw in radian
EulerAngles Quaternion::toEulerRad() const
{
    EulerAngles result;

    // roll
    result.a = atan2(2.f*(a*b + c*d),1.f - 2.f*(b*b + c*c));

    // pitch
    result.b = (-(float)(M_PI)/2.f + 2.f* atan2(sqrt(1.f+ 2.f*(a*c - b*d)), sqrt(1- 2*(a*c - b*d))));
    // or asin(2*(a*c - d*b));

    // yaw
    if (d==0)
        result.c = 0.0f;
    else
        result.c = atan2(2.f*(a*d + b*c),1.f - 2.f*(c*c + d*d));
    return result;
}

// Creat a rotation through two vectors, aligning the first to the second
Quaternion Quaternion::AlignVectors(const vector_ijk &start, const vector_ijk &dest)
{
	vector_ijk from = start;
    from.normalize();
	vector_ijk to = dest;
    to.normalize();

	float cosTheta = from.dot(to);
	vector_ijk rotationAxis;

	if (cosTheta < -1 + 0.001f){
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = from.cross(vector_ijk(0.0f, 0.0f, 1.0f));
		if (rotationAxis.get_norm2() < 0.01 ) // bad luck, they were parallel, try again!
			rotationAxis = from.cross(vector_ijk(1.0f, 0.0f, 0.0f));

		rotationAxis.normalize();
		return Quaternion(deg2rad(180.0f), rotationAxis);
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

Quaternion Quaternion::fromRotationMatrix(const vector_d &X, const vector_d &Y)
{
    vector_d mat[3];
    mat[0] = X;
    mat[1] = Y;
    mat[2] = X.cross(Y);
    // ESP_LOGI(FNAME, "Z: %f,%f,%f", mat[2].a, mat[2].b, mat[2].c);

    Quaternion result;
    const double trace = mat[0].a + mat[1].b + mat[2].c;
    // ESP_LOGI(FNAME, "trace: %f", trace);

    // check the diagonal
    if ( trace > 0.0 ) {
        double s = std::sqrt(trace + 1.0);
        result.a = s / 2.0;
        s = 0.5 / s;

        result.b = (mat[1][2] - mat[2][1]) * s;
        result.c = (mat[2][0] - mat[0][2]) * s;
        result.d = (mat[0][1] - mat[1][0]) * s;
    } else {
        // find largest diag. element with index i
        int i = 0;
        if ( mat[1][1] > mat[0][0] ) {
            i = 1;
        }
        if ( mat[2][2] > mat[i][i] ) {
            i = 2;
        }
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;

        double s = std::sqrt((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0);
        vector_d v;
        v[i] = s * 0.5;
        s = 0.5 / s;

        result.a = (mat[j][k] - mat[k][j]) * s;
        v[j] = (mat[i][j] + mat[j][i]) * s;
        v[k] = (mat[i][k] + mat[k][i]) * s;
        result.b = v[0]; result.c = v[1]; result.d = v[2];
    }
    return result;
}

// https://ahrs.readthedocs.io/en/latest/filters/aqua.html
// accel_par in m/(sec*sec)
Quaternion Quaternion::fromAccelerometer(const vector_ijk& accel_par)
{
    // Normalize
    vector_ijk an = accel_par;
    an.normalize();
	//ESP_LOGI(FNAME,"ax=%.3f ay=%.3f az=%.3f", an.an, an.b, an.c);
    // There is a singularity at an.c == -1
    if ( an.c < -0.999f ) {
        float half_cos = sqrt(0.5f*(1.0f - an.c));
        float temp = .5f / half_cos;
        Quaternion orientation( -an.b*temp, half_cos, 0.0, an.a*temp );
        //ESP_LOGI(FNAME,"Quat: %.3f %.3f %.3f %.3f", orientation.a, orientation.b, orientation.c, orientation.d );
        return orientation;
    } else {
        float half_cos = sqrt(0.5f*(1.0f + an.c));
        float temp = .5f / half_cos;
        Quaternion orientation( half_cos, -an.b*temp, an.a*temp, 0.0 );
        //ESP_LOGI(FNAME,"Quat: %.3f %.3f %.3f %.3f", orientation.a, orientation.b, orientation.c, orientation.d );
        return orientation;
    }
}

// omega in radians per second: dtime in seconds
Quaternion Quaternion::fromGyro(const vector_ijk& omega, float dtime)
{
    // ESP_LOGI(FNAME,"Quat: %.3f %.3f %.3f", omega.a, omega.b, omega.c );
    float alpha = 0.5*dtime;
    float a,b,c,d;
    b = alpha*(omega.a);
    c = alpha*(omega.b);
    d = alpha*(omega.c);
    a = 1.f - 0.5f*(b*b+c*c+d*d);
    Quaternion result(a,b,c,d);
    result.normalize();
    // ESP_LOGI(FNAME,"Quat1: %.3f %.3f %.3f %.3f", result.a, result.b, result.c, result.d );

    // Often found in literature and identic for small angles.
    // // omega=(alpha,beta,gamma)
    // // theta = ||omega||*dt; //length of angular velocity vector
    // float norm = omega.normalize(); // normalized orientation of angular velocity vector
    // float theta_05 = norm * 0.5 * dtime;
    // Quaternion result2(cos(theta_05), omega.a * sin(theta_05), omega.b * sin(theta_05), omega.c * sin(theta_05));
    // result = result2;
    // ESP_LOGI(FNAME,"Quat2: %.3f %.3f %.3f %.3f", result.a, result.b, result.c, result.d );
    return result;
}

// Grad
float Compass_atan2( float y, float x )
{
    float result = rad2deg(atan2(y, x));
    
    // As left turn means plus, euler angles come with 0° for north, -90° for east, -+180 degree for south and for 90° west
    // compass rose goes vice versa, so east is 90° means we need to invert
    if ( std::signbit(y) ) {
        result += 360.f;
    }

    return result;
} 

#ifdef Quaternionen_Test

// Test quaternion performance against a matrix mapping
class Matrix
{
public:
    Matrix() = default;
    Matrix(const vector_ijk& x, const vector_ijk& y) {
        vector_ijk z = x.cross(y);
        m[0][0] = x.a; m[1][0] = x.a; m[2][0] = x.a;
        m[0][1] = y.b; m[1][1] = y.b; m[2][1] = y.b;
        m[0][2] = z.c; m[1][2] = z.c; m[2][2] = z.c;
    }
    vector_ijk operator*(vector_ijk& v2) {
        vector_ijk r;
        // not correct, just for a performance comparison
        for ( int row = 0; row < 3; ++row ) {
            for ( int col = 0; col < 3; ++col ) {
                r[row] += m[row][col] * v2[row];
            }
        }
        return r;
    }

    float m[3][3];
};

void Quaternion::quaternionen_test()
{
    vector_ijk v1(1,0,0),
        v2(0,0,1),
        vt(1,2,3),
        x_axes(1,0,0),
        y_axes(0,1,0),
        z_axes(0,0,1),
        v3;

    // v to v setup
    int64_t t0 = esp_timer_get_time();
    Quaternion q = Quaternion::AlignVectors(v1,v2);
    int64_t t1 = esp_timer_get_time();
    ESP_LOGI(FNAME,"Setup");
    ESP_LOGI(FNAME,"Align v1 to v2: (%f,%f,%f) - (%f,%f,%f)", v1.a, v1.b, v1.c, v2.a, v2.b, v2.c );
    ESP_LOGI(FNAME,"%-4lldusec: %f %f %f %f a:%f", t1-t0, q.a, q.b, q.c, q.d, rad2deg(q.getAngle()) );
    // explicit setup, rotate around y
    Quaternion qex = Quaternion(deg2rad(-90.f), y_axes);
    ESP_LOGI(FNAME,"equal to: %f %f %f %f a:%f", qex.a, qex.b, qex.c, qex.d, rad2deg(qex.getAngle()) );

    // rotate
    t0 = esp_timer_get_time();
    v3 = q * v1;
    t1 = esp_timer_get_time();
    ESP_LOGI(FNAME,"Mapping (%lldusec)", t1-t0);
    ESP_LOGI(FNAME,"rotate v1 -> v2: %f %f %f", v3.a, v3.b, v3.c );
    // compare to matrix mapping
    Matrix m(v2, y_axes);
    t0 = esp_timer_get_time();
    v3 = m * v1;
    t1 = esp_timer_get_time();
    ESP_LOGI(FNAME,"Matrixing (%lldusec)", t1-t0);
    ESP_LOGI(FNAME,"rotate v1 -> v2: %f %f %f", v3.a, v3.b, v3.c );

    // Zero rotation
    q = Quaternion(1,0,0,0);
    v3 = q * v1;
    ESP_LOGI(FNAME,"rotate yero v1 -> v1: %f %f %f", v3.a, v3.b, v3.c );

    // slerp
    ESP_LOGI(FNAME,"Slerp: (v1+v2)/2, v2");
    Quaternion q2 = Quaternion::AlignVectors(v1,vector_ijk(0.707106781, 0, 0.707106781));
    ESP_LOGI(FNAME,"Q: %f %f %f %f a:%f", q2.a, q2.b, q2.c, q2.d, rad2deg(q2.getAngle()) );
    Quaternion qs = slerp(q, q2, 1.f);
    ESP_LOGI(FNAME,"-> (45°+90°)/2");
    ESP_LOGI(FNAME,"slerp: %f %f %f %f a:%f", qs.a, qs.b, qs.c, qs.d, rad2deg(qs.getAngle()) );

    // toEuler
    q = Quaternion(1,0,0,0);
    EulerAngles e = rad2deg(q.toEulerRad());
    ESP_LOGI( FNAME,"Euler zero r/p/y %.4f/%.4f/%.4f", e.Roll(), e.Pitch(), e.Yaw());
    // 45° around X
    q = Quaternion::AlignVectors(vector_ijk(0,0,1), vector_ijk(0,-1,1));
    e = rad2deg(q.toEulerRad());
    ESP_LOGI( FNAME,"Euler +45X r/p/y %.4f/%.4f/%.4f", e.Roll(), e.Pitch(), e.Yaw());

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


    // fromRotationMatrix
    ESP_LOGI(FNAME, "Test import of 3x3 matrix");
    q = fromRotationMatrix(vector_d(1,0,0), vector_d(0,1,0));
    ESP_LOGI(FNAME, "Benign case: 1,0,0/0,1,0 - %f %f %f %f", q.a, q.b, q.c, q.d);
    ESP_LOGI(FNAME, "Rotate 90°/Z");
    q = fromRotationMatrix(vector_d(0,1,0), vector_d(-1,0,0));
    ESP_LOGI(FNAME, "Quaternion : %f %f %f %f a:%f", q.a, q.b, q.c, q.d, rad2deg(q.getAngle()) );
    // explicit setup, rotate around z
    qex = Quaternion(deg2rad(90.f), z_axes);
    ESP_LOGI(FNAME, "check equal: %f %f %f %f a:%f", qex.a, qex.b, qex.c, qex.d, rad2deg(qex.getAngle()) );

    ESP_LOGI(FNAME, "Rotate 90°/X");
    q2 = fromRotationMatrix(vector_d(1,0,0), vector_d(0,0,1));
    ESP_LOGI(FNAME, "Quaternion : %f %f %f %f a:%f", q2.a, q2.b, q2.c, q2.d, rad2deg(q2.getAngle()) );
    // explicit setup, rotate around z
    qex = Quaternion(deg2rad(90.f), x_axes);
    ESP_LOGI(FNAME, "check equal: %f %f %f %f a:%f", qex.a, qex.b, qex.c, qex.d, rad2deg(qex.getAngle()) );

    ESP_LOGI(FNAME, "Concat Rotate 90°/Z and Rotate 90°/X");
    q = q * q2; // concatenate
    ESP_LOGI(FNAME, "Quaternion : %f %f %f %f a:%f", q2.a, q2.b, q2.c, q2.d, rad2deg(q2.getAngle()) );
    v1 = q * x_axes;
    ESP_LOGI(FNAME, "image of x-axes: %f %f %f", v1.a, v1.b, v1.c );
    v1 = q * y_axes;
    ESP_LOGI(FNAME, "image of y-axes: %f %f %f", v1.a, v1.b, v1.c );
    v1 = q * vector_ijk(5,5,5);
    ESP_LOGI(FNAME, "image of 5,5,5: %f %f %f", v1.a, v1.b, v1.c );
    // concatenate
    qex = fromRotationMatrix(vector_d(0,1,0), vector_d(0,0,1));
    ESP_LOGI(FNAME, "check equal: %f %f %f %f a:%f", qex.a, qex.b, qex.c, qex.d, rad2deg(qex.getAngle()) );
    v1 = qex * x_axes;
    ESP_LOGI(FNAME, "image of x-axes: %f %f %f", v1.a, v1.b, v1.c );
    v1 = qex * y_axes;
    ESP_LOGI(FNAME, "image of y-axes: %f %f %f", v1.a, v1.b, v1.c );
    v1 = qex * vector_ijk(5,5,5);
    ESP_LOGI(FNAME, "image of 5,5,5: %f %f %f", v1.a, v1.b, v1.c );

    // fromAccelerometer
    ESP_LOGI(FNAME, "Test accelerometer vector conversion");
    q = fromAccelerometer(vector_ijk(0,0,1));
    EulerAngles euler = rad2deg(q.toEulerRad());
    ESP_LOGI(FNAME, "Check zero case: R/P/Y: %f %f %f", euler.Roll(), euler.Pitch(), euler.Yaw());
    q = fromAccelerometer(vector_ijk(0,1,1));
    euler = rad2deg(q.toEulerRad());
    ESP_LOGI(FNAME, "Check Roll-45°X case: R/P/Y: %f %f %f", euler.Roll(), euler.Pitch(), euler.Yaw());

    // fromGyro
    ESP_LOGI(FNAME, "Test gyro vector conversion");
    q = fromGyro(vector_ijk(deg2rad(30),0,0), 0.5f);
    euler = rad2deg(q.toEulerRad());
    ESP_LOGI(FNAME, "Check X:30°/s,0.5s: X/Y/Z: %f %f %f", euler.Roll(), euler.Pitch(), euler.Yaw());
    q = fromGyro(vector_ijk(0,deg2rad(-30),0), 0.5f);
    euler = rad2deg(q.toEulerRad());
    ESP_LOGI(FNAME, "Check Y:-30°/s,0.5s: X/Y/Z: %f %f %f", euler.Roll(), euler.Pitch(), euler.Yaw());
    q = fromGyro(vector_ijk(0,0,deg2rad(30)), 0.5f);
    euler = rad2deg(q.toEulerRad());
    ESP_LOGI(FNAME, "Check Z:30°/s,0.5s: X/Y/Z: %f %f %f", euler.Roll(), euler.Pitch(), euler.Yaw());
    q = fromGyro(vector_ijk(deg2rad(30),deg2rad(30),deg2rad(30)), 0.5f);
    euler = rad2deg(q.toEulerRad());
    ESP_LOGI(FNAME, "Check All:30°/s,0.5s: X/Y/Z: %f %f %f", euler.Roll(), euler.Pitch(), euler.Yaw());
}

#endif
