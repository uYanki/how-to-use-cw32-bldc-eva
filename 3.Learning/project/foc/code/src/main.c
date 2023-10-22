#include "board.h"

int main()
{
    BoardInit();

    while (1)
    {
        static tick_t t = 0;

        if (DelayNonBlockUS(t, 2e6))
        {
            LedTgl(LED1);
            printf("hello\n");
            t = HAL_GetTick();
        }

    }
}

void BoardInit(void)
{
    SysclkInit();
    DelayInit();

    USART_InitTypeDef USART_InitStruct = {
        .USART_BaudRate            = 115200,
        .USART_Over                = USART_Over_16,
        .USART_Source              = USART_Source_PCLK,
        .USART_UclkFreq            = CONFIG_SYSCLK_FREQ,
        .USART_StartBit            = USART_StartBit_FE,
        .USART_StopBits            = USART_StopBits_1,
        .USART_Parity              = USART_Parity_No,
        .USART_Mode                = USART_Mode_Tx | USART_Mode_Rx,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    };
    UartInit();
    UartConfig(&USART_InitStruct);

    LedInit();
    KeyInit();
    BeepInit();

    NtcInit();
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
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */