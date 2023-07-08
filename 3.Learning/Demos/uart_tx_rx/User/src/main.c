#include "board.h"
#include <stdio.h>

//

uint8_t uart1_rxcnt = 0;
uint8_t uart1_rxflg = 0;

#define uart1_rxcap 64
char uart1_rxbuf[uart1_rxcap + 1] = {0};

//

void uart_init(uint32_t baudrate, uint32_t clkfreq);

//

int main()
{
    board_init();
    uart_init(9600, CONFIG_SYSCLK_FREQ);

    println("hello world");

    for (;;) {
        if (uart1_rxflg) {
            uart1_rxflg = 0;
            printf(uart1_rxbuf);
        }
    }
}

void uart_init(uint32_t baudrate, uint32_t clkfreq)
{
    {
#if 1
        __RCC_UART1_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();
#else
        RCC_APBPeriphClk_Enable2(RCC_APB2_PERIPH_UART1, ENABLE);
        RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_GPIOB, ENABLE);
#endif
    }
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        PB08_AFx_UART1TXD();
        PB09_AFx_UART1RXD();

        GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStructure.Pins = GPIO_PIN_8;  // tx
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Init(CW_GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.Pins = GPIO_PIN_9;  // rx
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
        GPIO_Init(CW_GPIOB, &GPIO_InitStructure);
    }
    {
        USART_InitTypeDef USART_InitStructure;

        USART_InitStructure.USART_BaudRate            = baudrate;
        USART_InitStructure.USART_Over                = USART_Over_16;
        USART_InitStructure.USART_Source              = USART_Source_PCLK;
        USART_InitStructure.USART_UclkFreq            = clkfreq;
        USART_InitStructure.USART_StartBit            = USART_StartBit_FE;
        USART_InitStructure.USART_StopBits            = USART_StopBits_1;
        USART_InitStructure.USART_Parity              = USART_Parity_No;
        USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

        USART_Init(CW_UART1, &USART_InitStructure);
    }
    {
        USART_ITConfig(CW_UART1, USART_IT_RC, ENABLE);
        NVIC_SetPriority(UART1_IRQn, 0);
        NVIC_EnableIRQ(UART1_IRQn);
    }
}

void UART1_IRQHandler(void)
{
    char ch;

    if (USART_GetFlagStatus(CW_UART1, USART_IT_RC)) {
        USART_ClearITPendingBit(CW_UART1, USART_IT_RC);
        ch = USART_ReceiveData(CW_UART1);
#if 0
        putchar(ch);
#else
        uart1_rxbuf[uart1_rxcnt++] = ch;

        if ((ch == '\n') ||
            (uart1_rxcnt == uart1_rxcap)) {
            uart1_rxbuf[uart1_rxcnt] = '\0';
            uart1_rxcnt              = 0;
            uart1_rxflg              = 1;
        }
    }
#endif
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @return None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
