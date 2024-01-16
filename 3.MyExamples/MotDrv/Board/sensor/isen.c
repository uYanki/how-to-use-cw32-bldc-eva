#include "isen.h"

#define AMP_GAIN 11.0  // (1+10/1)

// 806 = 3.3e6 / 4096 // (unit: uv)

// 偏置电压 1.65v

// input: 减去基准电压后的AD值
int16_t ISEN_GetPhaseCurrent(uint32_t adconv)  // unit: mA
{
    int amp_gain = AMP_GAIN;

    int shunt_conductance = 5;  // mOh, current sensing resistor

    int     amp_out_volt = 806 * adconv;                    // mV
    int     shunt_volt   = amp_out_volt / amp_gain;         // mV
    int16_t current      = shunt_volt / shunt_conductance;  // unit mA

    return current;
}
