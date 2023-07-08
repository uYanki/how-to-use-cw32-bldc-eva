#include "board.h"

void rcc_init(void)
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

#endif
}

void board_init(void)
{
    rcc_init();
}

#if CONFIG_REDIRECT_PRINTF

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE* f)
#endif

PUTCHAR_PROTOTYPE
{
    if (ch == '\n') {
        USART_SendData_8bit(CONFIG_REDIRECT_CWUART, '\r');
        while (USART_GetFlagStatus(CONFIG_REDIRECT_CWUART, USART_FLAG_TXE) == RESET) {}
    }

    USART_SendData_8bit(CONFIG_REDIRECT_CWUART, (uint8_t)ch);
    while (USART_GetFlagStatus(CONFIG_REDIRECT_CWUART, USART_FLAG_TXE) == RESET) {}

    return ch;
}

#endif
