#include "foc.h"

/**
 * @brief Park Transformation
 * @param [in] alpha, beta
 * @param [out] d, q
 */
void park(park_t* v)
{
    v->d = v->beta * v->sin + v->alpha * v->cos;
    v->q = v->beta * v->cos - v->alpha * v->sin;
}

/**
 * @brief Inverse Park Transformation
 * @param [in] d, q
 * @param [out] alpha, beta
 */
void ipark(park_t* v)
{
    v->alpha = v->d * v->cos - v->q * v->sin;
    v->beta  = v->d * v->sin + v->q * v->cos;
}

/**
 * @brief Clarke Transformation
 * @param [in] phase_a, phase_b, phase_c
 * @param [out] alpha, beta
 */
void clarke(clarke_t* v)
{
    v->alpha = v->phase_a;
    v->beta  = (v->phase_a + v->phase_b * 2) * M_INVSQRT3;
}

/**
 * @brief Inverse Clarke Transformation
 * @param [in] alpha, beta
 * @param [out] phase_a, phase_b, phase_c
 */
void iclarke(clarke_t* v)
{
    v->phase_a = v->alpha;
    v->phase_b = +0.5f * (M_SQRT3 * v->beta - v->alpha);
    v->phase_c = -0.5f * (M_SQRT3 * v->beta + v->alpha);
}

/**
 * @brief Space Vector PWM Generator
 *
 * @param [in] Ualpha, Ubeta
 * @param [out] Ta, Tb, Tc, Sector
 */
void svgen(svgen_t* v)
{
    f32 X = v->Ubeta;
    f32 Y = (v->Ubeta + M_SQRT3 * v->Ualpha) * 0.5f;
    f32 Z = Y - X;

    v->Sector = 3;

    if (Y > 0)
    {
        v->Sector -= 1;
    }
    if (Z > 0)
    {
        v->Sector -= 1;
    }
    if (X < 0)
    {
        v->Sector = 7 - v->Sector;
    }

    switch (v->Sector)
    {
        case 1:
        case 4: {
            v->Ta = Y;
            v->Tb = X - Z;
            v->Tc = -Y;
            break;
        }
        case 2:
        case 5: {
            v->Ta = Z + Y;
            v->Tb = X;
            v->Tc = -X;
            break;
        }
        default:
        case 3:
        case 6: {
            v->Ta = Z;
            v->Tb = -Z;
            v->Tc = -(X + Y);
            break;
        }
    }
}

/**
 * @brief
 *
 * @param MechTheta Motor Electrical angle
 * @param PolePairs Number of pole pairs
 *
 * @return Motor Mechanical Angle
 */
f32 angle_gen(f32 MechTheta, f32 PolePairs)
{
    f32 ElecTheta = PolePairs * MechTheta;

    while (ElecTheta >= 6.28f)
    {
        ElecTheta -= 6.28f;
    }

    return ElecTheta;
}

#if 0

void foc_tst()
{
#if 1

    if (keyflg & BV(0))
    {
        mechAngle += 0.002;

        if (mechAngle > 6.28)
        {
            mechAngle = 0;
        }
    }
    else
    {
        mechAngle -= 0.002;

        if (mechAngle < 0)
        {
            mechAngle = 6.28;
        }
    }

    elecAngle = mechAngle * polepairs;
#else

    if (keyflg & BV(0))
    {
        elecAngle += 0.01;
    }
    else
    {
        elecAngle -= 0.01;
    }

#endif

    _park.sin = sinf(elecAngle);
    _park.cos = cosf(elecAngle);

    ipark(&_park);

    _svpwm.Ualpha = _park.alpha;
    _svpwm.Ubeta  = _park.beta;

    svgen(&_svpwm);

    DUTY_A = _svpwm.Ta;
    DUTY_B = _svpwm.Tb;
    DUTY_C = _svpwm.Tc;
}

#endif

//---

#include "math.h"

/***************************************************************************/
// int array instead of float array
// 4x200 points per 360 deg
// 2x storage save (int 2Byte float 4 Byte )
// sin*10000

const int sine_array[200] = {0, 79, 158, 237, 316, 395, 473, 552, 631, 710, 789, 867, 946, 1024, 1103, 1181, 1260, 1338, 1416, 1494, 1572, 1650, 1728, 1806, 1883, 1961, 2038, 2115, 2192, 2269, 2346, 2423, 2499, 2575, 2652, 2728, 2804, 2879, 2955, 3030, 3105, 3180, 3255, 3329, 3404, 3478, 3552, 3625, 3699, 3772, 3845, 3918, 3990, 4063, 4135, 4206, 4278, 4349, 4420, 4491, 4561, 4631, 4701, 4770, 4840, 4909, 4977, 5046, 5113, 5181, 5249, 5316, 5382, 5449, 5515, 5580, 5646, 5711, 5775, 5839, 5903, 5967, 6030, 6093, 6155, 6217, 6279, 6340, 6401, 6461, 6521, 6581, 6640, 6699, 6758, 6815, 6873, 6930, 6987, 7043, 7099, 7154, 7209, 7264, 7318, 7371, 7424, 7477, 7529, 7581, 7632, 7683, 7733, 7783, 7832, 7881, 7930, 7977, 8025, 8072, 8118, 8164, 8209, 8254, 8298, 8342, 8385, 8428, 8470, 8512, 8553, 8594, 8634, 8673, 8712, 8751, 8789, 8826, 8863, 8899, 8935, 8970, 9005, 9039, 9072, 9105, 9138, 9169, 9201, 9231, 9261, 9291, 9320, 9348, 9376, 9403, 9429, 9455, 9481, 9506, 9530, 9554, 9577, 9599, 9621, 9642, 9663, 9683, 9702, 9721, 9739, 9757, 9774, 9790, 9806, 9821, 9836, 9850, 9863, 9876, 9888, 9899, 9910, 9920, 9930, 9939, 9947, 9955, 9962, 9969, 9975, 9980, 9985, 9989, 9992, 9995, 9997, 9999, 10000, 10000};

// ~40us (float array)
// ~50us (int array)
// precision +-0.005
// it has to receive an angle in between 0 and 2PI
/**
 * @brief 查表法计算sin
 * @param angle
 */
float _sin(float a)
{
    if (a < _PI_2)
    {
        // return sine_array[(int)(199.0*( a / (_PI/2.0)))];
        // return sine_array[(int)(126.6873* a)];           							// 浮点数数组优化
        return 0.0001f * sine_array[_round(126.6873f * a)];  // 整形数组
    }
    else if (a < _PI)
    {
        // return sine_array[(int)(199.0*(1.0 - (a-_PI/2.0) / (_PI/2.0)))];
        // return sine_array[398 - (int)(126.6873*a)];         		 		  // 浮点数数组优化
        return 0.0001f * sine_array[398 - _round(126.6873f * a)];  // 整形数组
    }
    else if (a < _3PI_2)
    {
        // return -sine_array[(int)(199.0*((a - _PI) / (_PI/2.0)))];
        // return -sine_array[-398 + (int)(126.6873*a)];           			// 浮点数数组优化
        return -0.0001f * sine_array[-398 + _round(126.6873f * a)];  // 整形数组
    }
    else
    {
        // return -sine_array[(int)(199.0*(1.0 - (a - 3*_PI/2) / (_PI/2.0)))];
        // return -sine_array[796 - (int)(126.6873*a)];           					// 浮点数数组优化
        return -0.0001f * sine_array[796 - _round(126.6873f * a)];  // 整形数组
    }
}
/**
 * @brief 查表法计算cos
 * @param angle
 */
float _cos(float a)
{
    float a_sin = a + _PI_2;
    a_sin       = a_sin > _2PI ? a_sin - _2PI : a_sin;
    return _sin(a_sin);
}
/**
 * @brief 标准化角度 【0，2PI】
 * @param void
 */
float _normalizeAngle(float angle)
{
    while (angle >= _2PI)
    {
        angle -= _2PI;
    }

    while (angle <= -_2PI)
    {
        angle += _2PI;
    }

    float a = angle;  // fmod(angle, _2PI);
    return a >= 0 ? a : (a + _2PI);
}
/**
 * @brief 获取电机电角度
 * @param 轴角度（当前机械角度） ；极对数
 */
float _electricalAngle(float shaft_angle, int pole_pairs)
{
    return (shaft_angle * pole_pairs);
}

//// https://reprap.org/forum/read.php?147,219210
//// https://en.wikipedia.org/wiki/Fast_inverse_square_root
///**
// * @brief 平方根近似函数
// * @param angle
// */
float _sqrt_fast(float number)
{  // low in fat
    long  i;
    float y;
    // float x;
    // const float f = 1.5F; // better precision
    // x = number * 0.5F;
    y = number;
    i = *(long*)&y;
    i = 0x5f375a86 - (i >> 1);
    y = *(float*)&i;
    // y = y * ( f - ( x * y * y ) ); // better precision
    return number * y;
}
