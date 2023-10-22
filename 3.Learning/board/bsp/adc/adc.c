#include "adc.h"
#include "pinmap.h"

// PA0~7 -> AIN0~7, PA0~3 -> AIN8~12,
static RO u8 adcChSel[] = {
    bv7 | 0x0,  // AIN0 PA00 U
    bv7 | 0x1,  // AIN1 PA01 V
    bv7 | 0x5,  // AIN5 PA05 W
    bv7 | 0x3,  // AIN3 PA03 HVBUS
    bv7 | 0x6,  // AIN6 PA06 IBIS
    bv7 | 0x7,  // AIN7 PA07 ICIV
    bv7 | 0x8,  // AIN8 PB00 RV1
    bv7 | 0x4,  // AIN4 PA04 NTC
};

void AdcInit(void)
{
    {
        __RCC_ADC_CLK_ENABLE();

        ADC_SingleChTypeDef ADC_SingleInitStruct;

        ADC_SingleInitStruct.ADC_Chmux = HVBUS_ADC_CH;  // channel

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

u16 AdcRead(void)  // blocking
{
    ADC_SoftwareStartConvCmd(ENABLE);
    while (ADC_GetITStatus(ADC_IT_EOC) == RESET) {}
    ADC_ClearITPendingBit(ADC_IT_EOC);
    return ADC_GetConversionValue();
}

f32 AdConv(u16 advalue)
{
    return (f32)advalue / 4096.f * 3.3f;
}
