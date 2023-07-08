#include "board.h"

#define delay(n) FirmwareDelay(100 * (n))

//

void     adc_init(void);
uint16_t adc_read(void);
void     uart_init(uint32_t baudrate, uint32_t clkfreq);

//

int main()
{
    board_init();
    uart_init(9600, CONFIG_SYSCLK_FREQ);
    adc_init();

    for (;;) {
        println("%d", adc_read());
        delay(1000);
    }
}

void adc_init(void)
{
    {
        __RCC_ADC_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();
        PB00_ANALOG_ENABLE();
    }
    {
        ADC_SingleChTypeDef ADC_SingleInitStruct;

        ADC_SingleInitStruct.ADC_Chmux = ADC_ExInputCH8;  // PB0 - CH8

        ADC_SingleInitStruct.ADC_DiscardEn = ADC_DiscardNull;

        ADC_SingleInitStruct.ADC_InitStruct.ADC_OpMode     = ADC_SingleChOneMode;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_ClkDiv     = ADC_Clk_Div16;  // PCLK
        ADC_SingleInitStruct.ADC_InitStruct.ADC_SampleTime = ADC_SampTime10Clk;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_VrefSel    = ADC_Vref_VDDA;  // Vref Src
        ADC_SingleInitStruct.ADC_InitStruct.ADC_InBufEn    = ADC_BufEnable;  // 跟随器
        ADC_SingleInitStruct.ADC_InitStruct.ADC_TsEn       = ADC_TsDisable;  // 内部温度传感器
        ADC_SingleInitStruct.ADC_InitStruct.ADC_DMAEn      = ADC_DmaDisable;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_Align      = ADC_AlignRight;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_AccEn      = ADC_AccDisable;

        ADC_SingleInitStruct.ADC_WdtStruct.ADC_WdtAll = ADC_WdtDisable;

        ADC_SingleChOneModeCfg(&ADC_SingleInitStruct);
        ADC_Enable();
    }
}

uint16_t adc_read(void)  // blocking
{
    ADC_SoftwareStartConvCmd(ENABLE);
    while (ADC_GetITStatus(ADC_IT_EOC) == RESET) {}
    ADC_ClearITPendingBit(ADC_IT_EOC);
    return ADC_GetConversionValue();
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
        USART_InitStructure.USART_Mode                = USART_Mode_Tx;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

        USART_Init(CW_UART1, &USART_InitStructure);
    }
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
