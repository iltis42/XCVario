#include <math.h>
#pragma once


class vector_ijk
{
public:
	vector_ijk() { a=0; b=0; c=0; };
	vector_ijk( const vector_ijk &o ) { a=o.a; b=o.b; c=o.c; };
	vector_ijk(float a, float b, float c);
	vector_ijk(vector_ijk&&) = default; // Allow the move optimization
	vector_ijk& operator=(const vector_ijk&) = default;


	void sum(const vector_ijk v2);
	void difference(vector_ijk v2);
	float dot_product(const vector_ijk v2);
	void cross_product(const vector_ijk v2 );
	vector_ijk cross(const vector_ijk &v2 ) const;
	void normalize();
	void normalize_f();
	float get_norm2();
	void scale(float scale);
	// tbd: private
	float a;
	float b;
	float c;
};
