#pragma once

#include <cmath>


template <typename T>
class vector_3d
{
public:
	vector_3d() = default;
	vector_3d( const vector_3d &o ) { a=o.a; b=o.b; c=o.c; };
	vector_3d(T a, T b, T c);
	vector_3d(vector_3d&&) = default; // Allow the move optimization
	vector_3d<T>& operator=(const vector_3d<T>&) = default;

	// indexed access
	T& operator[](int i) { return (&a)[i]; }

	// API
	void set(const T p1, const T p2, const T p3) { a=p1, b=p2, c=p3; }
	vector_3d<T>& operator+=(const vector_3d<T>& v2);
	vector_3d<T>& operator+(const vector_3d<T>& v2) const;
	vector_3d<T>& operator-=(const vector_3d<T>& v2);
	vector_3d<T>& operator-(const vector_3d<T>& v2) const;
	vector_3d<T>& operator*=(const T s2);
	vector_3d<T>& operator/=(const T s2);
	T dot(const vector_3d<T>& v2);
	// void cross_product(const vector_3d v2 );
	vector_3d<T> cross(const vector_3d<T> &v2 ) const;
	T get_norm() const { return sqrt(a*a + b*b + c*c); }
	T get_norm2() const { return a*a + b*b + c*c; }
	void normalize();
	// tbd: private
	T a;
	T b;
	T c;
};

template <typename T>
vector_3d<T>& operator+(const vector_3d<T> v1, const vector_3d<T> v2);


typedef vector_3d<float> vector_ijk;
typedef vector_3d<double> vector_d;