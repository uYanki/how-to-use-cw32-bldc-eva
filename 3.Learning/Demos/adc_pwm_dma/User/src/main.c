#include "board.h"
#include <stdio.h>

void adc_pwm_init(void);

//

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

int main()
{
    board_init();
    adc_pwm_init();
    uart_init(9600, CONFIG_SYSCLK_FREQ);

    for (;;) {
        // read adc
        ADC_SoftwareStartConvCmd(ENABLE);
        while (ADC_GetITStatus(ADC_IT_EOC) == RESET) {}
        FirmwareDelay(100000);
        // DMA will stop when CNT [bit3,0] reduces to 0
        CW_DMACHANNEL1->CNT = bv16 | 0x1;
        DMA_Cmd(CW_DMACHANNEL1, ENABLE);
    }
}

void adc_pwm_init(void)
{
    {
        __RCC_ADC_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();
        __RCC_ATIM_CLK_ENABLE();
        __RCC_DMA_CLK_ENABLE();

        PB00_ANALOG_ENABLE();
        PB11_AFx_ATIMCH3A();
    }
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pins  = GPIO_PIN_11;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    }
    {
        ADC_SingleChTypeDef ADC_SingleInitStruct;

        ADC_SingleInitStruct.ADC_Chmux = ADC_ExInputCH8;  // PB0 - CH8

        ADC_SingleInitStruct.ADC_DiscardEn                 = ADC_DiscardNull;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_OpMode     = ADC_SingleChOneMode;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_ClkDiv     = ADC_Clk_Div16;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_SampleTime = ADC_SampTime10Clk;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_VrefSel    = ADC_Vref_VDDA;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_InBufEn    = ADC_BufEnable;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_TsEn       = ADC_TsDisable;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_DMAEn      = ADC_DmaEnable;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_Align      = ADC_AlignRight;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_AccEn      = ADC_AccDisable;
        ADC_SingleInitStruct.ADC_WdtStruct.ADC_WdtAll      = ADC_WdtDisable;

        ADC_SingleChOneModeCfg(&ADC_SingleInitStruct);
        ADC_Enable();
    }
    {
        ATIM_InitTypeDef ATIM_InitStruct;

        // clksrc = 1MHz (1us)
#if CONFIG_SYSCLK_FREQ == 8000000
        ATIM_InitStruct.Prescaler = ATIM_Prescaler_DIV8;
#elif CONFIG_SYSCLK_FREQ == 64000000
        ATIM_InitStruct.Prescaler = ATIM_Prescaler_DIV64;
#endif

        ATIM_InitStruct.CounterDirection   = ATIM_COUNTING_UP;
        ATIM_InitStruct.CounterOPMode      = ATIM_OP_MODE_REPETITIVE;  // 连续
        ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_EDGE_ALIGN;
        ATIM_InitStruct.BufferState        = DISABLE;
        ATIM_InitStruct.ClockSelect        = ATIM_CLOCK_PCLK;
        ATIM_InitStruct.ReloadValue        = 4096 - 1;
        ATIM_InitStruct.RepetitionCounter  = 0;
        ATIM_InitStruct.UnderFlowMask      = DISABLE;
        ATIM_InitStruct.OverFlowMask       = DISABLE;

        ATIM_Init(&ATIM_InitStruct);
        ATIM_Cmd(ENABLE);
    }
    {
        ATIM_OCInitTypeDef ATIM_OCInitStruct;

        ATIM_OCInitStruct.BufferState       = ENABLE;
        ATIM_OCInitStruct.OCDMAState        = DISABLE;
        ATIM_OCInitStruct.OCInterruptSelect = ATIM_OC_IT_NONE;
        ATIM_OCInitStruct.OCInterruptState  = DISABLE;
        ATIM_OCInitStruct.OCMode            = ATIM_OCMODE_PWM1;
        ATIM_OCInitStruct.OCPolarity        = ATIM_OCPOLARITY_NONINVERT;

        ATIM_OC3AInit(&ATIM_OCInitStruct);
        ATIM_CH3Config(ATIM_CHxA_CIE, ENABLE);

        ATIM_SetCompare3A(0);
        ATIM_CtrlPWMOutputs(ENABLE);
    }
    {
        DMA_InitTypeDef DMA_InitStruct;

        DMA_InitStruct.DMA_Mode          = DMA_MODE_BLOCK;
        DMA_InitStruct.DMA_TransferWidth = DMA_TRANSFER_WIDTH_16BIT;
        DMA_InitStruct.DMA_SrcInc        = DMA_SrcAddress_Fix;
        DMA_InitStruct.DMA_DstInc        = DMA_DstAddress_Fix;
        DMA_InitStruct.TrigMode          = DMA_HardTrig;
        DMA_InitStruct.HardTrigSource    = DMA_HardTrig_ADC_TRANSCOMPLETE;
        DMA_InitStruct.DMA_TransferCnt   = 1;  // 传输次数
        DMA_InitStruct.DMA_SrcAddress    = (uint32_t) & (CW_ADC->RESULT0);
        DMA_InitStruct.DMA_DstAddress    = (uint32_t) & (CW_ATIM->CH3CCRA);

        DMA_Init(CW_DMACHANNEL1, &DMA_InitStruct);
        DMA_Cmd(CW_DMACHANNEL1, ENABLE);
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
