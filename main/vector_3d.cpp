#include "vector_3d.h"
#include <cstdint>
#include <limits>
#include <cassert>

vector_ijk::vector_ijk(float _a, float _b, float _c)
{
    a = _a;
    b = _b;
    c = _c;
}

void vector_ijk::sum(const vector_ijk v2)
{
    a = a + v2.a;
    b = b + v2.b;
    c = c + v2.c;
}

void vector_ijk::difference(const vector_ijk v2)
{
    a = a - v2.a;
    b = b - v2.b;
    c = c - v2.c;
}

float vector_ijk::dot_product(const vector_ijk v2)
{
    return (a*v2.a + b*v2.b + c*v2.c);
}


void vector_ijk::cross_product(const vector_ijk v2)
{
    a = b*v2.c - c*v2.b;
    b = c*v2.a - a*v2.c;
    c = a*v2.b - b*v2.a;
}

vector_ijk vector_ijk::cross(const vector_ijk &v2) const
{
    vector_ijk tmp;
    tmp.a = b*v2.c - c*v2.b;
    tmp.b = c*v2.a - a*v2.c;
    tmp.c = a*v2.b - b*v2.a;
    return tmp;
}

void vector_ijk::normalize()
{
    float one_by_sqrt;
    one_by_sqrt = 1/sqrt(a*a + b*b + c*c);
    a = a*one_by_sqrt;
    b = b*one_by_sqrt;
    c = c*one_by_sqrt;
}

void vector_ijk::normalize_f()
{
    float len = sqrt(a*a + b*b + c*c);
    a = a/len;
    b = b/len;
    c = c/len;
}

float vector_ijk::get_norm2()
{
    return a*a + b*b + c*c;
}

void vector_ijk::scale(float scale)
{
    a = a*scale;
    b = b*scale;
    c = c*scale;
}

