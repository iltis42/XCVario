/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/


#include <cmath>
#include <stdint.h>

bool floatEqual(float a, float b, float eps)
{
    return fabsf(a - b) <= eps * fmaxf(1.0f, fmaxf(fabsf(a), fabsf(b)));
}

// equality tested 0.999996 == 1.0f .. 1.000009 == 1.0f
bool floatEqualFast(float a, float b)
{
    // reinterpret as integers (safe on ESP32 little endian)
    uint32_t ai = *(uint32_t*)&a;
    uint32_t bi = *(uint32_t*)&b;

    // Handle sign bit uniformly
    ai = (ai & 0x80000000U) ? 0x80000000U - ai : ai;
    bi = (bi & 0x80000000U) ? 0x80000000U - bi : bi;

    uint32_t diff = (ai > bi) ? ai - bi : bi - ai;

    // Convert relative tolerance into bit distance
    // relTol ≈ 1e-5 → ~80 ULPs for float near 1.0f
    constexpr uint32_t maxDiff = 80;

    return diff <= maxDiff;
}


bool floatEqualFastAbs(float a, float b, float tol)
{
    float diff = a - b;
    return (diff < tol && diff > -tol);
}
