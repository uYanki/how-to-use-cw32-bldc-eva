#ifndef __MOTDRV_PWM_H__
#define __MOTDRV_PWM_H__

#include "defs.h"

void PWM_Start(void);
void PWM_Stop(void);
void PWM_SetDuty(uint16_t DutyA, uint16_t DutyB, uint16_t DutyC);

#endif
