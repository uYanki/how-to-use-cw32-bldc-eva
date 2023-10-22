#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "cw32_fwlib.h"

#define LED1 1
#define LED2 2
#define LED3 3

void LedInit(void);
void LedOn(u8 idx);
void LedOff(u8 idx);
void LedTgl(u8 idx);

#endif
