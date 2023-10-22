#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "cw32_fwlib.h"

#define KEY1 1
#define KEY2 2
#define KEY3 3

void KeyInit(void);
bool KeyIsPress(u8 idx);
bool KeyIsRelease(u8 idx);

#endif
