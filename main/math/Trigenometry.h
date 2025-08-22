/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <cstdint>

// general trigenometric helper
constexpr double My_PId = 3.14159265358979323846;
constexpr float My_PIf = static_cast<float>(My_PId);
constexpr float PI2f = 2.0f * static_cast<float>(My_PId);

constexpr double deg2rad(double degrees) { return degrees * (My_PId / 180.0); }
constexpr float deg2rad(float degrees)   { return degrees * (My_PIf / 180.0); }
constexpr double rad2deg(double rad)     { return rad * 180.0 / My_PId; }
constexpr float rad2deg(float rad)       { return rad * 180.0 / My_PIf; }

// integer angle math
int normalizeDeg(int angle);
int angleDiffDeg(int a1, int a2);

inline int fast_roundf_to_int(float a) {
    return (int)((a >= 0.0f) ? (a + 0.5f) : (a - 0.5f));
}

float fast_floorf(float x);

// fast gauge routines with reduced precision
float fast_sin_deg(float angle);
float fast_cos_deg(float angle);
float fast_sin_rad(float rad);
float fast_cos_rad(float rad);
float fast_sin_idx(int16_t idx);
float fast_cos_idx(int idx);

float fast_log2f(float x);

float fast_atan(float x);
float fast_atan2(float y, float x);
float fast_tan_deg(float deg);
float fast_tan_rad(float rad);

int count_digits(unsigned int n);

