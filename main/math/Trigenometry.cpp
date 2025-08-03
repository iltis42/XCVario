/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "Trigenometry.h"

#include <cmath>
#include <cfloat>

// O:=1813c == 1,85µs (240MHz)
int normalizeDeg(int angle)
{
    angle = angle % 360;
    if (angle < 0)
        angle += 360;
    return angle;
}

int angleDiffDeg(int a1, int a2)
{
    return normalizeDeg(a1 - a2);
}


// Operation	    Rough Cycle Cost	Notes
// (int)a           ~1–5 cycles	        Hardware FP convert, extremely fast
// floorf(a)	    ~3802           	Implements proper rounding toward –∞
// fmodf(a, 360.0f)	~3801       	    Software modulus, heavy overhead
// sin(a)           ~14226
// log2f(a)         ~12763
// tanf(a)          ~47403
// tan(a)           ~42657
// atan2(100., a)   ~14064
// atan2f(100., a)  ~14064


// O():=1360c
float fast_floorf(float x) {
    int i = (int)x;  // truncates toward 0
    return (x < 0.0f && x != (float)i) ? (float)(i - 1) : (float)i;
}


//
// fast trigenometric helpers in 0.5 degree steps
// esp32 takes 1000~3000 cycles to approximate a sinus, tan, atan, and log do not perform as well
// for all discrete gauge math the precision is just fine, not for other purposes (!)
//

// sinus table with 0.5 degree resolution
static const float sin_table_0_5deg[182] = {
    0.000000000f,   0.008726535f,   0.017452406f,   0.026176948f,   0.034899497f,   0.043619387f,   0.052335956f,   0.061048540f,
    0.069756474f,   0.078459096f,   0.087155743f,   0.095845753f,   0.104528463f,   0.113203214f,   0.121869343f,   0.130526192f,
    0.139173101f,   0.147809411f,   0.156434465f,   0.165047606f,   0.173648178f,   0.182235525f,   0.190808995f,   0.199367934f,
    0.207911691f,   0.216439614f,   0.224951054f,   0.233445364f,   0.241921896f,   0.250380004f,   0.258819045f,   0.267238376f,
    0.275637356f,   0.284015345f,   0.292371705f,   0.300705800f,   0.309016994f,   0.317304656f,   0.325568154f,   0.333806859f,
    0.342020143f,   0.350207381f,   0.358367950f,   0.366501227f,   0.374606593f,   0.382683432f,   0.390731128f,   0.398749069f,
    0.406736643f,   0.414693243f,   0.422618262f,   0.430511097f,   0.438371147f,   0.446197813f,   0.453990500f,   0.461748613f,
    0.469471563f,   0.477158760f,   0.484809620f,   0.492423560f,   0.500000000f,   0.507538363f,   0.515038075f,   0.522498565f,
    0.529919264f,   0.537299608f,   0.544639035f,   0.551936985f,   0.559192903f,   0.566406237f,   0.573576436f,   0.580702956f,
    0.587785252f,   0.594822787f,   0.601815023f,   0.608761429f,   0.615661475f,   0.622514637f,   0.629320391f,   0.636078220f,
    0.642787610f,   0.649448048f,   0.656059029f,   0.662620048f,   0.669130606f,   0.675590208f,   0.681998360f,   0.688354576f,
    0.694658370f,   0.700909264f,   0.707106781f,   0.713250449f,   0.719339800f,   0.725374371f,   0.731353702f,   0.737277337f,
    0.743144825f,   0.748955721f,   0.754709580f,   0.760405966f,   0.766044443f,   0.771624583f,   0.777145961f,   0.782608157f,
    0.788010754f,   0.793353340f,   0.798635510f,   0.803856861f,   0.809016994f,   0.814115518f,   0.819152044f,   0.824126189f,
    0.829037573f,   0.833885822f,   0.838670568f,   0.843391446f,   0.848048096f,   0.852640164f,   0.857167301f,   0.861629160f,
    0.866025404f,   0.870355696f,   0.874619707f,   0.878817113f,   0.882947593f,   0.887010833f,   0.891006524f,   0.894934362f,
    0.898794046f,   0.902585284f,   0.906307787f,   0.909961271f,   0.913545458f,   0.917060074f,   0.920504853f,   0.923879533f,
    0.927183855f,   0.930417568f,   0.933580426f,   0.936672189f,   0.939692621f,   0.942641491f,   0.945518576f,   0.948323655f,
    0.951056516f,   0.953716951f,   0.956304756f,   0.958819735f,   0.961261696f,   0.963630453f,   0.965925826f,   0.968147640f,
    0.970295726f,   0.972369920f,   0.974370065f,   0.976296007f,   0.978147601f,   0.979924705f,   0.981627183f,   0.983254908f,
    0.984807753f,   0.986285602f,   0.987688341f,   0.989015863f,   0.990268069f,   0.991444861f,   0.992546152f,   0.993571856f,
    0.994521895f,   0.995396198f,   0.996194698f,   0.996917334f,   0.997564050f,   0.998134798f,   0.998629535f,   0.999048222f,
    0.999390827f,   0.999657325f,   0.999847695f,   0.999961923f,  1.0f, 1.0f
};

// O():=8178c
// peak error for x.25 and x.75 angles is about 5µ rad
float fast_sin_deg(float angle) {
    // equals to angle = fmodf(angle, 360.0f); (3800c)
    angle = angle - 360.0f * fast_floorf(angle / 360.0f); // 0 .. 360.0 (2736c)
    
    bool negative = false;
    if (angle > 180.0f) {
        angle -= 180.0f;
        negative = true;
    }

    if (angle > 90.0f)
        angle = 180.0f - angle;

    // index
    angle *= 2.0f; // 0.5° resolution ==> Index = Grad × 2; [0 .. 180)
    int index = static_cast<int>(angle);
    float fraction = angle - static_cast<float>(index); // 0 .. 1

    float result = sin_table_0_5deg[index] + fraction * (sin_table_0_5deg[index+1] - sin_table_0_5deg[index]);
    return negative ? -result : result;
}

float fast_cos_deg(float angle) {
    return fast_sin_deg(angle + 90.0f);
}

float fast_sin_rad(float rad) {
    return fast_sin_deg(rad * (180.0f / My_PIf));
}

float fast_cos_rad(float rad) {
    return fast_cos_deg(rad * (180.0f / My_PIf));
}

// O():=3191c
float fast_log2f(float x) {
    union { float f; uint32_t i; } vx = { x };
    uint32_t ix = vx.i;

    // extract exponent and normalize mantissa
    int exponent = ((ix >> 23) & 0xFF) - 127;
    ix = (ix & 0x007FFFFF) | 0x3f800000;  // normalize mantissa
    vx.i = ix;
    float m = vx.f;

    // Polynomial approximation of log2(m) around 1.0
    // This 4th-degree approximation is accurate within ~0.001
    float y = m - 1.0f;
    float log2_m = y * (1.34656f + y * (-0.33674f + y * (0.07477f - y * 0.02424f)));

    return exponent + log2_m;
}


// fast approximation of atan(x) for x in [0, 1] !!! (0-45°)
float fast_atan(float x) {
    return x / (1.0f + 0.28f * x * x);
}

// O():=5484c
float fast_atan2(float y, float x) {
    const float ONEQTR_PI = M_PI_4;        // π/4
    const float THRQTR_PI = 3.0f * M_PI_4; // 3π/4
    float abs_y = fabsf(y) + 1e-10f;       // avoid division by zero
    float r, angle;

    if (x < 0.0f) {
        r = (x + abs_y) / (abs_y - x);
        angle = THRQTR_PI;
    } else {
        r = (x - abs_y) / (x + abs_y);
        angle = ONEQTR_PI;
    }
    angle += (0.1963f * r * r - 0.9817f) * r;
    return (y < 0.0f) ? -angle : angle;
}


constexpr float TAN_STEP_DEG = 0.5f;
constexpr int TAN_TABLE_SIZE = 181;


/* 0.5° step: index = deg × 2
   0   → tan_table[0]   … tan( 0.0°)
   89° → tan_table[178] … tan(89.0°)
   89.5° → tan_table[179]
   90°  → tan_table[180] (∞)
*/
const float tan_table[TAN_TABLE_SIZE] = {
    0.00000000f,   0.00872687f,   0.01745506f,   0.02618592f,   0.03492077f,   0.04366094f,   0.05240778f,   0.06116262f,
    0.06992681f,   0.07870171f,   0.08748866f,   0.09628905f,   0.10510424f,   0.11393561f,   0.12278456f,   0.13165250f,
    0.14054083f,   0.14945100f,   0.15838444f,   0.16734261f,   0.17632698f,   0.18533904f,   0.19438031f,   0.20345230f,
    0.21255656f,   0.22169466f,   0.23086819f,   0.24007876f,   0.24932800f,   0.25861758f,   0.26794919f,   0.27732454f,
    0.28674539f,   0.29621349f,   0.30573068f,   0.31529879f,   0.32491970f,   0.33459532f,   0.34432761f,   0.35411857f,
    0.36397023f,   0.37388468f,   0.38386404f,   0.39391048f,   0.40402623f,   0.41421356f,   0.42447482f,   0.43481237f,
    0.44522869f,   0.45572626f,   0.46630766f,   0.47697553f,   0.48773259f,   0.49858161f,   0.50952545f,   0.52056705f,
    0.53170943f,   0.54295570f,   0.55430905f,   0.56577278f,   0.57735027f,   0.58904502f,   0.60086062f,   0.61280079f,
    0.62486935f,   0.63707026f,   0.64940759f,   0.66188556f,   0.67450852f,   0.68728096f,   0.70020754f,   0.71329307f,
    0.72654253f,   0.73996108f,   0.75355405f,   0.76732699f,   0.78128563f,   0.79543592f,   0.80978403f,   0.82433639f,
    0.83909963f,   0.85408069f,   0.86928674f,   0.88472526f,   0.90040404f,   0.91633117f,   0.93251509f,   0.94896457f,
    0.96568877f,   0.98269726f,   1.00000000f,   1.01760739f,   1.03553031f,   1.05378013f,   1.07236871f,   1.09130850f,
    1.11061251f,   1.13029439f,   1.15036841f,   1.17084957f,   1.19175359f,   1.21309700f,   1.23489716f,   1.25717230f,
    1.27994163f,   1.30322537f,   1.32704482f,   1.35142244f,   1.37638192f,   1.40194829f,   1.42814801f,   1.45500903f,
    1.48256097f,   1.51083519f,   1.53986496f,   1.56968558f,   1.60033453f,   1.63185169f,   1.66427948f,   1.69766312f,
    1.73205081f,   1.76749402f,   1.80404776f,   1.84177089f,   1.88072647f,   1.92098213f,   1.96261051f,   2.00568971f,
    2.05030384f,   2.09654360f,   2.14450692f,   2.19429973f,   2.24603677f,   2.29984255f,   2.35585237f,   2.41421356f,
    2.47508685f,   2.53864790f,   2.60508906f,   2.67462149f,   2.74747742f,   2.82391289f,   2.90421088f,   2.98868496f,
    3.07768354f,   3.17159480f,   3.27085262f,   3.37594342f,   3.48741444f,   3.60588351f,   3.73205081f,   3.86671309f,
    4.01078093f,   4.16529977f,   4.33147587f,   4.51070850f,   4.70463011f,   4.91515703f,   5.14455402f,   5.39551717f,
    5.67128182f,   5.97576436f,   6.31375151f,   6.69115624f,   7.11536972f,   7.59575411f,   8.14434643f,   8.77688736f,
    9.51436445f,   10.38539708f,   11.43005230f,   12.70620474f,   14.30066626f,   16.34985548f,   19.08113669f,   22.90376555f,
    28.63625328f,   38.18845930f,   57.28996163f,   114.58865013f,   INFINITY
};


// O():=7729c
// peak error around 90°, 270°, ...
float fast_tan_deg(float deg)
{
    // Wrap angle to 0–180°, then use tan(θ) = –tan(θ–180°)
    bool negative = false;
    deg = deg - 360.0f * fast_floorf(deg / 360.0f); // 0 .. 360.0
    if (deg > 180.f) { deg -= 180.f; negative = true; }

    // Reflect >90° into first quadrant (tan is symmetric)
    if (deg > 90.f)  deg = 180.f - deg;

    // Index calculation
    int idx = (int)(deg / TAN_STEP_DEG);
    if (idx >= TAN_TABLE_SIZE - 1)   // 89.5° or above
        return negative ? -INFINITY : INFINITY;

    float frac = (deg - idx * TAN_STEP_DEG) / TAN_STEP_DEG;

    // Linear interpolation between idx and idx+1
    float t0 = tan_table[idx];
    float t1 = tan_table[idx + 1];
    float value = t0 + frac * (t1 - t0);
    return negative ? -value : value;
}

float fast_tan_rad(float rad) {
    return fast_tan_deg(rad * (180.0f / My_PIf));
}

int count_digits(unsigned int n) {
    if (n < 10)        return 1;
    else if (n < 100)  return 2;
    else if (n < 1000) return 3;
    else if (n < 10000) return 4;
    else if (n < 100000) return 5;
    else if (n < 1000000) return 6;
    else if (n < 10000000) return 7;
    else if (n < 100000000) return 8;
    else if (n < 1000000000) return 9;
    else return 10; // max for 32-bit unsigned
}

