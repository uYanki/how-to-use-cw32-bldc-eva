#include "pwmctrl.h"

#include "tim.h"
#include "foc.h"

extern TIM_HandleTypeDef htim1;
#define TIM_SV htim1  // space vertor

void PWM_Start(void)
{
    uint16_t duty = ParaTbl.u16PwmDutyMax >> 1;
    PWM_SetDuty(duty, duty, duty);

    HAL_TIM_PWM_Start(&TIM_SV, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM_SV, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&TIM_SV, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&TIM_SV, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&TIM_SV, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&TIM_SV, TIM_CHANNEL_3);
}

void PWM_Stop(void)
{
    HAL_TIM_PWM_Stop(&TIM_SV, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&TIM_SV, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&TIM_SV, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&TIM_SV, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&TIM_SV, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&TIM_SV, TIM_CHANNEL_3);
}

void PWM_SetDuty(uint16_t DutyA, uint16_t DutyB, uint16_t DutyC)
{
    __HAL_TIM_SetCompare(&TIM_SV, TIM_CHANNEL_1, DutyA);
    __HAL_TIM_SetCompare(&TIM_SV, TIM_CHANNEL_2, DutyB);
    __HAL_TIM_SetCompare(&TIM_SV, TIM_CHANNEL_3, DutyC);
}
