#include "board.h"

static void RCC_Init(void);
static void UART_Init(void);
static void BTIM_Init(void);

void board_init(void)
{
    RCC_Init();
    UART_Init();
    BTIM_Init();
}

static void RCC_Init(void)
{
    /**
     * @note
     *  HSI -> Inside
     *  HSE -> External
     */

#if CONFIG_SYSCLK_FREQ == 8000000UL  // 8Mhz

    // enable HSI
    RCC_HSI_Enable(RCC_HSIOSC_DIV6);

#elif CONFIG_SYSCLK_FREQ == 64000000UL  // 64MHz

    // enable HSI
    RCC_HSI_Enable(RCC_HSIOSC_DIV6);

    // set division factor
    RCC_HCLKPRS_Config(RCC_HCLK_DIV1);
    RCC_PCLKPRS_Config(RCC_PCLK_DIV1);

    // PLL clksrc = HSI, 8M x 8 = 64MHz
    RCC_PLL_Enable(RCC_PLLSOURCE_HSI, 8000000, 8);

#if 0
    RCC_PLL_OUT();  // PC13 output PLL clock
#endif

    __RCC_FLASH_CLK_ENABLE();
    // 24 < HCLK ≤ 48: FLASH_Latency_2
    // 48 < HCLK: FLASH_Latency_3
    FLASH_SetLatency(FLASH_Latency_3);  // FLASH 读等待周期

    // switch SysClk to PLL
    RCC_SysClk_Switch(RCC_SYSCLKSRC_PLL);
    RCC_SystemCoreClockUpdate(64000000);  // 64M

#else

#error "unknown sysclk"

#endif
}

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

static void BTIM_Init(void);

#endif  // CONFIG_USING_DEBUG_TICKER
