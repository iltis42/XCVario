#include <math.h>
#pragma once

float InvSqrt(float x);

class vector_ijk
{
public:
	vector_ijk() { a=0; b=0; c=0; };
	vector_ijk( const vector_ijk &o ) { a=o.a; b=o.b; c=o.c; };
	vector_ijk(float a, float b, float c);
	void sum(const vector_ijk v2);
	void difference(vector_ijk v2);
	float dot_product(const vector_ijk v2);
	void cross_product(const vector_ijk v2 );
	vector_ijk cross(const vector_ijk &v2 );
	void normalize();
	float get_norm2();
	void scale(float scale);
	// tbd: private
	float a;
	float b;
	float c;
};
