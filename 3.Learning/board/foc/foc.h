
#ifndef __MOTDRV_FOC_H__
#define __MOTDRV_FOC_H__

#include "usdk.h"

typedef struct {
    f32 phase_a;  // phase-a stator
    f32 phase_b;  // phase-b stator
    f32 phase_c;  // phase-c stator

    f32 alpha;  // stationary d-axis stator
    f32 beta;   // stationary q-axis stator
} clarke_t;

typedef struct {
    f32 alpha;  // stationary d-axis stator
    f32 beta;   // stationary q-axis stator
    f32 theta;  // rotating angle (electrical angle)

    f32 d;  // rotating d-axis stator variable
    f32 q;  // rotating q-axis stator variable

    f32 sin;  // sine(theta)
    f32 cos;  // cosine(theta)
} park_t;

typedef struct {
    f32 Ualpha;  // alpha-axis phase voltage
    f32 Ubeta;   // beta-axis phase voltage

    f32 Ta;  // phase-a switching function
    f32 Tb;  // phase-b switching function
    f32 Tc;  // phase-c switching function

    u8 Sector;  // Space vector sector
} svgen_t;

void park(park_t* v);
void ipark(park_t* v);

void clarke(clarke_t* v);
void iclarke(clarke_t* v);

void svgen(svgen_t* v);

f32 angle_gen(f32 MechTheta, f32 PolePairs);

//--------

// sign function
#define _sign(a)                   (((a) < 0) ? -1 : ((a) > 0))
#define _round(x)                  ((x) >= 0 ? (long)((x) + 0.5) : (long)((x)-0.5))
#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
// #define _sqrt(a)                   (_sqrt_fast(a))
#define _isset(a)                  ((a) != (NOT_SET))
#define NOT_SET                    -12345
// utility defines
#define _2_SQRT3                   1.1547005f
#define _SQRT3                     1.7320508f
#define _1_SQRT3                   0.5773502f
#define _SQRT3_2                   0.8660254f
#define _SQRT2                     1.4142135f
#define _120_D2R                   2.0943951f
#define _PI                        3.1415926f
#define _PI_2                      1.5707963f
#define _PI_3                      1.047197f
#define _2PI                       6.283185f
#define _3PI_2                     4.712389f
#define _PI_6                      0.523599f

// dq current structure
typedef struct {
    float d;
    float q;
} DQCurrent_s;
// phase current structure
typedef struct {
    float a;
    float b;
    float c;
} PhaseCurrent_s;
// dq voltage structs
typedef struct {
    float d;
    float q;
} DQVoltage_s;

float _sin(float a);
float _cos(float a);
float _normalizeAngle(float angle);
float _electricalAngle(float shaft_angle, int pole_pairs);
float _sqrt_fast(float number);  //_sqrtApprox

#endif
