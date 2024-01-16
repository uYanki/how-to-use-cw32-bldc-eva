#include "vbus.h"

uint16_t VBUS_GetVoltage(uint16_t adconv)  // unit: 0.1v
{
    return adconv * 19 / 106;
}
