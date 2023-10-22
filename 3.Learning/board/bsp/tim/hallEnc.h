#ifndef __BSP_HALL_ENC_H__
#define __BSP_HALL_ENC_H__

#include "cw32_fwlib.h"

extern vu16 u16HallAngle;
extern vu32 u32HallCount;

void HallEncInit(void);
u16  HallEncGetAngle(void);

#endif
