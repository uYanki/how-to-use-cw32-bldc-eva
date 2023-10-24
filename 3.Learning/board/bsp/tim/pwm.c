#include "pwm.h"
#include "pinmap.h"

#define PWM_Period 1600

void PwmInit(void)
{
    // GPIO
    {
        /**
         * PA8  PB13 CH1A CH1B
         * PA9  PB14 CH2A CH2B
         * PA10 PB15 CH3A CH3B
         */

        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        GPIO_InitTypeDef GPIO_InitStruct = {
            .IT    = GPIO_IT_NONE,
            .Speed = GPIO_SPEED_HIGH,
        };

        PA08_AFx_ATIMCH1A();
        PA09_AFx_ATIMCH2A();
        PA10_AFx_ATIMCH3A();
        PB13_AFx_ATIMCH1B();
        PB14_AFx_ATIMCH2B();
        PB15_AFx_ATIMCH3B();

        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pins = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    }
    // TIM
    {
        // 载波频率 freq = 64M (sysclk) / 2 (CENTER_ALIG) / 1600 (PWM_Period) = 20kHz

        ATIM_InitTypeDef ATIM_InitStruct = {
            .BufferState        = DISABLE,
            .ClockSelect        = ATIM_CLOCK_PCLK,
            .CounterAlignedMode = ATIM_COUNT_MODE_CENTER_ALIGN,
            .CounterOPMode      = ATIM_OP_MODE_REPETITIVE,
            .Prescaler          = ATIM_Prescaler_DIV1,
            .ReloadValue        = PWM_Period - 1,
            .RepetitionCounter  = 0,
        };

        ATIM_OCInitTypeDef ATIM_OCInitStruct = {
            .BufferState       = DISABLE,
            .OCDMAState        = DISABLE,
            .OCInterruptSelect = ATIM_OC_IT_NONE,
            .OCInterruptState  = DISABLE,
            .OCMode            = ATIM_OCMODE_PWM2,
        };

        __RCC_ATIM_CLK_ENABLE();

        ATIM_Init(&ATIM_InitStruct);

        // 输出极性是否翻转看具体芯片哦！！

        ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_NONINVERT;
        ATIM_OC1AInit(&ATIM_OCInitStruct);
        ATIM_OC2AInit(&ATIM_OCInitStruct);
        ATIM_OC3AInit(&ATIM_OCInitStruct);

        ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_INVERT;
        ATIM_OC1BInit(&ATIM_OCInitStruct);
        ATIM_OC2BInit(&ATIM_OCInitStruct);
        ATIM_OC3BInit(&ATIM_OCInitStruct);

        ATIM_SetCompare1A(PWM_Period);
        ATIM_SetCompare2A(PWM_Period);
        ATIM_SetCompare3A(PWM_Period);

        // 互补输出，插入死区
        ATIM_PWMOutputConfig(OCREFA_TYPE_DOUBLE, OUTPUT_TYPE_COMP, 2);

        ATIM_CtrlPWMOutputs(ENABLE);
        ATIM_Cmd(ENABLE);
    }
}
