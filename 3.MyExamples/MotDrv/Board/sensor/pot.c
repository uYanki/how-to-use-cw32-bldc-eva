#include "pot.h"

uint16_t POT1_GetVoltage(uint16_t adconv)  // unit: mv
{
    return adconv * 3300 / 4096;
}

uint16_t POT2_GetVoltage(uint16_t adconv)  // unit: mv
{
    return adconv * 3300 / 4096;
}