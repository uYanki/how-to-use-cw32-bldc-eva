#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "cw32_fwlib.h"

void AdcInit(void);
u16  AdcRead(void);
f32  AdConv(u16 advalue);

#endif
