#include "vector_3d.h"
#include <cstdint>


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

vector_ijk vector_ijk::cross(const vector_ijk &v2)
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
    one_by_sqrt = InvSqrt(a*a + b*b + c*c);
    a = a*one_by_sqrt;
    b = b*one_by_sqrt;
    c = c*one_by_sqrt;
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

float InvSqrt(float x)
{
   uint32_t i = 0x5F1F1412 - (*(uint32_t*)&x >> 1);
   float tmp = *(float*)&i;
   return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
}
