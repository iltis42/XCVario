/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "GaugeTrig.h"

#include <math.h>
#include <float.h>

//
// fast trigenometric helpers in 0.5 degree steps
// esp32 takes 1000~3000 cycles to approximate a sinus, tan, atan, and log do not perform as well
// for all discrete gauge math the precision is just fine, not for other purposes (!)
//

// sinus table with 0.5 degree resolution
static const float sin_table_0_5deg[181] = {
    0.000000f, 0.008727f, 0.017452f, 0.026177f, 0.034899f, 0.043619f, 0.052336f, 0.061049f,
    0.069756f, 0.078459f, 0.087156f, 0.095846f, 0.104528f, 0.113203f, 0.121869f, 0.130526f,
    0.139173f, 0.147809f, 0.156434f, 0.165048f, 0.173648f, 0.182236f, 0.190809f, 0.199368f,
    0.207912f, 0.216440f, 0.224951f, 0.233445f, 0.241922f, 0.250380f, 0.258819f, 0.267238f,
    0.275637f, 0.284015f, 0.292372f, 0.300706f, 0.309017f, 0.317305f, 0.325568f, 0.333807f,
    0.342020f, 0.350207f, 0.358368f, 0.366501f, 0.374607f, 0.382683f, 0.390731f, 0.398749f,
    0.406737f, 0.414693f, 0.422618f, 0.430511f, 0.438371f, 0.446198f, 0.453990f, 0.461749f,
    0.469472f, 0.477159f, 0.484810f, 0.492424f, 0.500000f, 0.507538f, 0.515038f, 0.522499f,
    0.529919f, 0.537300f, 0.544639f, 0.551937f, 0.559193f, 0.566406f, 0.573576f, 0.580703f,
    0.587785f, 0.594823f, 0.601815f, 0.608762f, 0.615661f, 0.622514f, 0.629320f, 0.636078f,
    0.642788f, 0.649448f, 0.656059f, 0.662620f, 0.669131f, 0.675590f, 0.681998f, 0.688355f,
    0.694658f, 0.700909f, 0.707107f, 0.713250f, 0.719340f, 0.725374f, 0.731354f, 0.737277f,
    0.743145f, 0.748956f, 0.754710f, 0.760406f, 0.766044f, 0.771625f, 0.777146f, 0.782608f,
    0.788011f, 0.793353f, 0.798636f, 0.803857f, 0.809017f, 0.814116f, 0.819152f, 0.824126f,
    0.829038f, 0.833886f, 0.838671f, 0.843391f, 0.848048f, 0.852640f, 0.857167f, 0.861629f,
    0.866025f, 0.870356f, 0.874620f, 0.878817f, 0.882948f, 0.887011f, 0.891007f, 0.894934f,
    0.898794f, 0.902585f, 0.906308f, 0.909961f, 0.913545f, 0.917060f, 0.920505f, 0.923880f,
    0.927184f, 0.930418f, 0.933580f, 0.936672f, 0.939693f, 0.942641f, 0.945519f, 0.948324f,
    0.951057f, 0.953717f, 0.956305f, 0.958820f, 0.961262f, 0.963630f, 0.965926f, 0.968148f,
    0.970296f, 0.972370f, 0.974370f, 0.976296f, 0.978148f, 0.979924f, 0.981627f, 0.983255f,
    0.984807f, 0.986285f, 0.987688f, 0.989016f, 0.990268f, 0.991445f, 0.992546f, 0.993571f,
    0.994522f, 0.995396f, 0.996195f, 0.996917f, 0.997564f, 0.998135f, 0.998630f, 0.999048f,
    0.999391f, 0.999657f, 0.999848f, 0.999962f, 1.000000f
};

float fast_sin_deg(float angle) {
    angle = fmodf(angle, 360.0f);
    if (angle < 0) angle += 360.0f;

    bool negative = false;
    if (angle > 180.0f) {
        angle -= 180.0f;
        negative = true;
    }

    if (angle > 90.0f)
        angle = 180.0f - angle;

    int index = (int)(angle * 2.0f + 0.5f); // 0.5° resolution → Index = Grad × 2

    float result = sin_table_0_5deg[index];
    return negative ? -result : result;
}

float fast_cos_deg(float angle) {
    return fast_sin_deg(angle + 90.0f);
}

float fast_sin_rad(float rad) {
    return fast_sin_deg(rad * (180.0f / M_PI));
}

float fast_cos_rad(float rad) {
    return fast_cos_deg(rad * (180.0f / M_PI));
}

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


// fast approximation of atan(x) for x in [0, 1]
float fast_atan(float x) {
    return x / (1.0f + 0.28f * x * x);
}

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


   // for FLT_MAX if you prefer it to INFINITY

/* 0.5° step: index = deg × 2
   0   → tan_table[0]   … tan( 0.0°)
   89° → tan_table[178] … tan(89.0°)
   89.5° → tan_table[179]
   90°  → tan_table[180] (∞)
*/
const float tan_table[181] = {
    0.000000f,  0.008727f,  0.017455f,  0.026186f,  0.034921f,  0.043661f,  0.052408f,  0.061163f,
    0.069927f,  0.078702f,  0.087489f,  0.096289f,  0.105104f,  0.113936f,  0.122785f,  0.131652f,
    0.140541f,  0.149451f,  0.158384f,  0.167343f,  0.176327f,  0.185339f,  0.194380f,  0.203452f,
    0.212557f,  0.221695f,  0.230868f,  0.240079f,  0.249328f,  0.258618f,  0.267949f,  0.277325f,
    0.286745f,  0.296213f,  0.305731f,  0.315299f,  0.324920f,  0.334595f,  0.344328f,  0.354119f,
    0.363970f,  0.373885f,  0.383864f,  0.393910f,  0.404026f,  0.414214f,  0.424475f,  0.434812f,
    0.445229f,  0.455726f,  0.466308f,  0.476976f,  0.487733f,  0.498582f,  0.509525f,  0.520567f,
    0.531709f,  0.542956f,  0.554309f,  0.565773f,  0.577350f,  0.589045f,  0.600861f,  0.612801f,
    0.624869f,  0.637070f,  0.649408f,  0.661886f,  0.674509f,  0.687281f,  0.700208f,  0.713293f,
    0.726543f,  0.739961f,  0.753554f,  0.767327f,  0.781286f,  0.795436f,  0.809784f,  0.824336f,
    0.839100f,  0.854081f,  0.869287f,  0.884725f,  0.900404f,  0.916331f,  0.932515f,  0.948965f,
    0.965689f,  0.982697f,  1.000000f,  1.017607f,  1.035530f,  1.053780f,  1.072369f,  1.091309f,
    1.110613f,  1.130294f,  1.150368f,  1.170850f,  1.191754f,  1.213097f,  1.234897f,  1.257172f,
    1.279942f,  1.303225f,  1.327045f,  1.351422f,  1.376382f,  1.401948f,  1.428148f,  1.455009f,
    1.482561f,  1.510835f,  1.539865f,  1.569686f,  1.600335f,  1.631852f,  1.664279f,  1.697663f,
    1.732051f,  1.767494f,  1.804048f,  1.841771f,  1.880726f,  1.920982f,  1.962611f,  2.005690f,
    2.050304f,  2.096544f,  2.144507f,  2.194300f,  2.246037f,  2.299843f,  2.355852f,  2.414214f,
    2.475087f,  2.538648f,  2.605089f,  2.674621f,  2.747477f,  2.823913f,  2.904211f,  2.988685f,
    3.077684f,  3.171595f,  3.270853f,  3.375943f,  3.487414f,  3.605884f,  3.732051f,  3.866713f,
    4.010781f,  4.165300f,  4.331476f,  4.510709f,  4.704630f,  4.915157f,  5.144554f,  5.395517f,
    5.671282f,  5.975764f,  6.313752f,  6.691156f,  7.115370f,  7.595754f,  8.144346f,  8.776887f,
    9.514364f,  10.385397f,  11.430052f,  12.706205f,  14.300666f,  16.349855f,  19.081137f,  22.903766f,
    28.636253f,  38.188459f,  57.289962f,  114.588650f,  INFINITY
};



#define TAN_STEP_DEG   0.5f
#define TAN_TABLE_SIZE 181

float fast_tan_deg(float deg)
{
    // Wrap angle to 0–180°, then use tan(θ) = –tan(θ–180°)
    bool negative = false;
    deg = fmodf(deg, 360.f);
    if (deg < 0) deg += 360.f;
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
    return fast_tan_deg(rad * (180.0f / M_PI));
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

