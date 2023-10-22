#include "board.h"

#if CONFIG_USING_DEBUG_TICKER

static void BTIM_Init(void)
{
    {
        __RCC_BTIM_CLK_ENABLE();

        __disable_irq();
        NVIC_EnableIRQ(BTIM1_IRQn);
        __enable_irq();
    }
    {
        BTIM_TimeBaseInitTypeDef BTIM_TimeBaseInitStruct;

        BTIM_TimeBaseInitStruct.BTIM_Mode   = BTIM_Mode_TIMER;
        BTIM_TimeBaseInitStruct.BTIM_OPMode = BTIM_OPMode_Repetitive;

#if CONFIG_DEBUG_TICKER_PERIOD_N_MS > 0

#if CONFIG_SYSCLK_FREQ == 8000000
        BTIM_OCInitStruct.BTIM_Prescaler = BTIM_PRS_DIV64;
#elif CONFIG_SYSCLK_FREQ == 64000000
        BTIM_TimeBaseInitStruct.BTIM_Prescaler = BTIM_PRS_DIV512;
#endif

        BTIM_TimeBaseInitStruct.BTIM_Period = CONFIG_DEBUG_TICKER_PERIOD_N_MS * 125 - 1;

#elif CONFIG_DEBUG_TICKER_PERIOD_N_US > 0

#if CONFIG_SYSCLK_FREQ == 8000000
        BTIM_OCInitStruct.BTIM_Prescaler = BTIM_PRS_DIV8;
#elif CONFIG_SYSCLK_FREQ == 64000000
        BTIM_TimeBaseInitStruct.BTIM_Prescaler = BTIM_PRS_DIV64;
#endif

        BTIM_TimeBaseInitStruct.BTIM_Period = CONFIG_DEBUG_TICKER_PERIOD_N_US - 1;

#else

#error "undefine ticker period"

#endif

        BTIM_TimeBaseInit(CW_BTIM1, &BTIM_TimeBaseInitStruct);
        BTIM_ITConfig(CW_BTIM1, BTIM_IT_OV, ENABLE);
    }

    BTIM_Cmd(CW_BTIM1, ENABLE);
}

volatile u32 g_ticks = 0;

void WEAK BTIM1_IRQCallBack() {}

void BTIM1_IRQHandler(void)
{
    if (BTIM_GetITStatus(CW_BTIM1, BTIM_IT_OV))
    {
        BTIM_ClearITPendingBit(CW_BTIM1, BTIM_IT_OV);

        ++g_ticks;
        BTIM1_IRQCallBack();
    }
}

#else

#endif
