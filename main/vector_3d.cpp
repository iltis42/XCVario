#include "vector_3d.h"
#include <stdint.h>

vector_ijk vector_3d_initialize(float a, float b, float c)
{
    vector_ijk v;
    v.a = a;
    v.b = b;
    v.c = c;
    return v;
}

vector_ijk vector_3d_sum(vector_ijk v1, vector_ijk v2)
{
    vector_ijk v;
    v.a = v1.a + v2.a;
    v.b = v1.b + v2.b;
    v.c = v1.c + v2.c;
    return v;
}

vector_ijk vector_3d_difference(vector_ijk v1, vector_ijk v2)
{
    vector_ijk v;
    v.a = v1.a - v2.a;
    v.b = v1.b - v2.b;
    v.c = v1.c - v2.c;
    return v;
}

float vector_3d_dot_product(vector_ijk v1, vector_ijk v2)
{
    return (v1.a*v2.a + v1.b*v2.b + v1.c*v2.c);

}


vector_ijk vector_3d_cross_product(vector_ijk v1, vector_ijk v2)
{
    vector_ijk v;
    v.a = v1.b*v2.c - v1.c*v2.b;
    v.b = v1.c*v2.a - v1.a*v2.c;
    v.c = v1.a*v2.b - v1.b*v2.a;
    return v;
}

vector_ijk vector_3d_normalize(vector_ijk v1)
{
    vector_ijk v2;
    float one_by_sqrt;
    one_by_sqrt = InvSqrt(v1.a*v1.a + v1.b*v1.b + v1.c*v1.c);
    v2.a = v1.a*one_by_sqrt;
    v2.b = v1.b*one_by_sqrt;
    v2.c = v1.c*one_by_sqrt;
    return v2;
}

vector_ijk vector_3d_scale(vector_ijk v1, float scale)
{
    vector_ijk v2;
    v2.a = v1.a*scale;
    v2.b = v1.b*scale;
    v2.c = v1.c*scale;
    return v2;
}

float InvSqrt(float x)
{
   uint32_t i = 0x5F1F1412 - (*(uint32_t*)&x >> 1);
   float tmp = *(float*)&i;
   return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
}
