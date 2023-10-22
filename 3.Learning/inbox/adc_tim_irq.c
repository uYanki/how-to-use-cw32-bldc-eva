#include "board.h"
#include <stdio.h>

void uart_init(uint32_t baudrate, uint32_t clkfreq);
void adc_tim_init(void);

float32_t ntc_conv(uint16_t adc);

//

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

void BTIM1_IRQHandler(void)
{
    if (BTIM_GetITStatus(CW_BTIM1, BTIM_IT_OV))
    {
        BTIM_ClearITPendingBit(CW_BTIM1, BTIM_IT_OV);
        ADC_SoftwareStartConvCmd(ENABLE); // ADC 有个外部触发的, 可能不需要该中断
    }
}

void ADC_IRQHandler(void)
{
    if (ADC_GetITStatus(ADC_IT_EOC))
    {
        ADC_ClearITPendingBit(ADC_IT_EOC);
        println("%.1f", ntc_conv(ADC_GetConversionValue()));
    }
}
