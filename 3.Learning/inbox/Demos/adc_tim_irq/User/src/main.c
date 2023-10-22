#include "board.h"
#include <stdio.h>

void uart_init(uint32_t baudrate, uint32_t clkfreq);
void adc_tim_init(void);

float32_t ntc_conv(uint16_t adc);

//

int main()
{
    board_init();
    uart_init(9600, CONFIG_SYSCLK_FREQ);
    adc_tim_init();

    for (;;) {
    }
}

void uart_init(uint32_t baudrate, uint32_t clkfreq)
{
    __RCC_UART1_CLK_ENABLE();
    __RCC_GPIOB_CLK_ENABLE();

    {
        GPIO_InitTypeDef GPIO_InitStructure;

        PB08_AFx_UART1TXD();

        GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStructure.Pins  = GPIO_PIN_8;  // tx
        GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;

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

void adc_tim_init(void)
{
    {
        __RCC_BTIM_CLK_ENABLE();
        __RCC_ADC_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        __disable_irq();
        NVIC_EnableIRQ(BTIM1_IRQn);
        NVIC_EnableIRQ(ADC_IRQn);
        __enable_irq();

        PA04_ANALOG_ENABLE();
    }
    {
        ADC_SingleChTypeDef ADC_SingleInitStruct;

        ADC_SingleInitStruct.ADC_Chmux                     = ADC_ExInputCH4;
        ADC_SingleInitStruct.ADC_DiscardEn                 = ADC_DiscardNull;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_AccEn      = ADC_AccDisable;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_Align      = ADC_AlignRight;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_ClkDiv     = ADC_Clk_Div16;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_DMAEn      = ADC_DmaDisable;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_InBufEn    = ADC_BufEnable;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_OpMode     = ADC_SingleChOneMode;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_SampleTime = ADC_SampTime10Clk;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_TsEn       = ADC_TsDisable;
        ADC_SingleInitStruct.ADC_InitStruct.ADC_VrefSel    = ADC_Vref_VDDA;
        ADC_SingleInitStruct.ADC_WdtStruct.ADC_WdtAll      = ADC_WdtDisable;

        ADC_SingleChOneModeCfg(&ADC_SingleInitStruct);
        ADC_ITConfig(ADC_IT_EOC, ENABLE);
        ADC_Enable();
    }
    {
        BTIM_TimeBaseInitTypeDef BTIM_InitStruct;

        BTIM_InitStruct.BTIM_Mode   = BTIM_Mode_TIMER;
        BTIM_InitStruct.BTIM_OPMode = BTIM_OPMode_Repetitive;
        // 1MHz -> 1us
#if CONFIG_SYSCLK_FREQ == 8000000
        BTIM_InitStruct.BTIM_Prescaler = BTIM_PRS_DIV8;
#elif CONFIG_SYSCLK_FREQ == 64000000
        BTIM_InitStruct.BTIM_Prescaler = BTIM_PRS_DIV64;
#endif
        BTIM_InitStruct.BTIM_Period = 50000;  // 50ms

        BTIM_TimeBaseInit(CW_BTIM1, &BTIM_InitStruct);
        BTIM_ITConfig(CW_BTIM1, BTIM_IT_OV, ENABLE);
        BTIM_Cmd(CW_BTIM1, ENABLE);
    }
}

float32_t ntc_conv(uint16_t adc)
{
    static uint16_t map[98] = {28017, 26826, 25697, 24629, 23618, 22660, 21752, 20892, 20075, 19299, 18560, 18482, 18149, 17632, 16992, 16280, 15535, 14787, 14055, 13354, 12690, 12068, 11490, 10954, 10458, 10000, 9576, 9184, 8819, 8478, 8160, 7861, 7579, 7311, 7056, 6813, 6581, 6357, 6142, 5934, 5734, 5541, 5353, 5173, 4998, 4829, 4665, 4507, 4355, 4208, 4065, 3927, 3794, 3664, 3538, 3415, 3294, 3175, 3058, 2941, 2825, 2776, 2718, 2652, 2582, 2508, 2432, 2356, 2280, 2207, 2135, 2066, 2000, 1938, 1879, 1823, 1770, 1720, 1673, 1628, 1586, 1546, 1508, 1471, 1435, 1401, 1367, 1334, 1301, 1268, 1236, 1204, 1171, 1139, 1107, 1074, 1042, 1010};

    uint8_t  max = 97, min = 0, mid;
    float    mV  = adc / 4096.f * 3300.f;  // map to [0,3300] mV
    uint16_t val = (mV / (5 - mV / 1000)) * 10;

    while (1) {
        mid = (max + min) / 2;
        (map[mid] < val) ? (max = mid) : (min = mid);
        if ((max - min) <= 1) break;
    }

    float32_t temp = min;
    if (max != min) {
        temp += (float32_t)(map[min] - val) / (map[min] - map[max]);
    }

    return temp;
}

void BTIM1_IRQHandler(void)
{
    if (BTIM_GetITStatus(CW_BTIM1, BTIM_IT_OV)) {
        BTIM_ClearITPendingBit(CW_BTIM1, BTIM_IT_OV);
        ADC_SoftwareStartConvCmd(ENABLE);
    }
}

void ADC_IRQHandler(void)
{
    if (ADC_GetITStatus(ADC_IT_EOC)) {
        ADC_ClearITPendingBit(ADC_IT_EOC);
        println("%.1f", ntc_conv(ADC_GetConversionValue()));
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
