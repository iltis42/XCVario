#ifdef __cplusplus
extern "C" {
#endif

#ifndef VECTOR_3D_INCLUDED
#define VECTOR_3D_INCLUDED

#include <math.h>

typedef struct vector_ijk {

    float a;
    float b;
    float c;

    // v = ai + bj + ck

} vector_ijk;

vector_ijk vector_3d_initialize(float a, float b, float c);
vector_ijk vector_3d_sum(vector_ijk v1, vector_ijk v2);
vector_ijk vector_3d_difference(vector_ijk v1, vector_ijk v2);
float vector_3d_dot_product(vector_ijk v1, vector_ijk v2);
vector_ijk vector_3d_cross_product(vector_ijk v1, vector_ijk v2);
vector_ijk vector_3d_normalize(vector_ijk v);
vector_ijk vector_3d_scale(vector_ijk v1, float scale);
float InvSqrt(float x);

#endif

#ifdef __cplusplus
}
#endif
