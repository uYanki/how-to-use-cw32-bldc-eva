#include "adc.h"
#include "pinmap.h"

#define ADC_REG_CHSEL          CW_ADC->CR1
#define ADC_REG_RESULT         CW_ADC->RESULT0
#define ADC_REG_CONV           CW_ADC->START

// 转换结果
#define ADC_RES_DMA_CH         CW_DMACHANNEL4
#define ADC_RES_DMA_BUFADDR    AdcResBuf
#define ADC_RES_DMA_BUFSIZE    ARRAY_SIZE(AdcResBuf)
#define ADC_RES_DMA_IRQn       DMACH45_IRQn
#define ADC_RES_DMA_IRQHandler DMACH45_IRQHandler
#define ADC_RES_DMA_IT_TC      DMA_IT_TC4

// 待转换通道
#define ADC_SRC_DMA_CH         CW_DMACHANNEL5
#define ADC_SRC_DMA_BUFADDR    AdcChSel
#define ADC_SRC_DMA_BUFSIZE    ARRAY_SIZE(AdcChSel)

// 触发转换
#define ADC_CONV_DMA_CH        CW_DMACHANNEL3

#if 0
            f32 Vmdc = ADC_12R_3V3(adcDatBuf.Vbus) * 40.2156f;  // (5.1k+200k) / (5.1k)
            f32 Ue = ADC_12R_3V3(adcDatBuf.Ue), Ve = ADC_12R_3V3(adcDatBuf.Ve), We = ADC_12R_3V3(adcDatBuf.We);
            f32 temp    = NTC_GetTemp(adcDatBuf.ntc);
            // f32 current = ADC_12R_3V3(adcDatBuf.Ib - adcDatBuf.Ic) / (f32)10e3 * 1e3;
            f32 current_b = ADC_12R_3V3((adcDatBuf.Ib )) / 4.3 /* A = (33K/10K+1) */ / 0.1 /* 100mR */ * 1e3;  // mA
						f32 current_c = ADC_12R_3V3((adcDatBuf.Ic )) / 4.3 /* A = (33K/10K+1) */ / 0.1 /* 100mR */ * 1e3;  // mA
            println("%f,%f,%f,%f,%d,%f,%f,%f", Ue, Ve, We, Vmdc, adcDatBuf.Vpot, current_b, current_c, temp);
#else

// bv7: 转换完成后触发DMA
static RO u8 AdcChSel[] = {
    bv7 | ADC_SqrCh0,  // AIN0 PA00 U
    bv7 | ADC_SqrCh1,  // AIN1 PA01 V
    bv7 | ADC_SqrCh5,  // AIN5 PA05 W
    bv7 | ADC_SqrCh3,  // AIN3 PA03 HVBUS
    bv7 | ADC_SqrCh6,  // AIN6 PA06 IBIS
    bv7 | ADC_SqrCh7,  // AIN7 PA07 ICIV
    bv7 | ADC_SqrCh4,  // AIN4 PA04 NTC
    bv7 | ADC_SqrCh8,  // AIN8 PB00 RV1
    // bv7 | ADC_SqrTs,
    // bv7 | ADC_SqrVddaDiV3,
    // bv7 | ADC_SqrVref1P2,
};

#define HVBUS_COEFF   (40.2157f)  // 单位 V, 计算公式 (5.1k+200k) / (5.1k)
#define CURRENT_COEFF (2.32558f)  // 单位 A, 放大倍数 A = (33K/10K+1), 采样电阻 Rshunt = 100mR, 计算公式 1/4.3/0.1

u16 AdcResBuf[ARRAY_SIZE(AdcChSel)] = {0};

void AdcInit(void)
{
    // GPIO
    {
        PA00_ANALOG_ENABLE();
        PA01_ANALOG_ENABLE();
        PA05_ANALOG_ENABLE();
        PA03_ANALOG_ENABLE();
        PA04_ANALOG_ENABLE();
        PA06_ANALOG_ENABLE();
        PA07_ANALOG_ENABLE();
        PB00_ANALOG_ENABLE();
    }
    // ADC
    {
        __RCC_ADC_CLK_ENABLE();

        ADC_InitTypeDef ADC_InitStructure = {
            .ADC_OpMode     = ADC_SingleChOneMode,
            .ADC_ClkDiv     = ADC_Clk_Div8,
            .ADC_SampleTime = ADC_SampTime10Clk,
            .ADC_VrefSel    = ADC_Vref_VDDA,   // 参考电压
            .ADC_InBufEn    = ADC_BufDisable,  // 电压跟随器
            .ADC_TsEn       = ADC_TsEnable,    // 片内温度传感器
            .ADC_DMAEn      = ADC_DmaEnable,
            .ADC_Align      = ADC_AlignRight,
            .ADC_AccEn      = ADC_AccDisable,  // 转换结果不累加
        };

        ADC_Init(&ADC_InitStructure);
        ADC_Enable();
    }
    // DMA
    {
        __RCC_DMA_CLK_ENABLE();

        DMA_InitTypeDef DMA_InitStruct = {
            .DMA_Mode       = DMA_MODE_BLOCK,
            .HardTrigSource = DMA_HardTrig_ADC_TRANSCOMPLETE,
            .TrigMode       = DMA_HardTrig,
        };

        {
            // 转换结果运输

            DMA_InitStruct.DMA_DstAddress    = (u32)(&ADC_RES_DMA_BUFADDR[0]);
            DMA_InitStruct.DMA_DstInc        = DMA_DstAddress_Increase;
            DMA_InitStruct.DMA_SrcAddress    = (u32)(&ADC_REG_RESULT);
            DMA_InitStruct.DMA_SrcInc        = DMA_SrcAddress_Fix;
            DMA_InitStruct.DMA_TransferCnt   = ADC_RES_DMA_BUFSIZE;
            DMA_InitStruct.DMA_TransferWidth = DMA_TRANSFER_WIDTH_16BIT;

            DMA_Init(ADC_RES_DMA_CH, &DMA_InitStruct);
        }

        {
            // 通道选择

            ADC_REG_CHSEL = ADC_SRC_DMA_BUFADDR[0];

            DMA_InitStruct.DMA_DstAddress    = (u32)(&ADC_REG_CHSEL);
            DMA_InitStruct.DMA_DstInc        = DMA_DstAddress_Fix;
            DMA_InitStruct.DMA_SrcAddress    = (u32)(&ADC_SRC_DMA_BUFADDR[1]);
            DMA_InitStruct.DMA_SrcInc        = DMA_SrcAddress_Increase;
            DMA_InitStruct.DMA_TransferCnt   = ADC_SRC_DMA_BUFSIZE - 1;
            DMA_InitStruct.DMA_TransferWidth = DMA_TRANSFER_WIDTH_8BIT;

            DMA_Init(ADC_SRC_DMA_CH, &DMA_InitStruct);
        }

        {
            // 触发转换

            static RO u8 AdcRunConv = ADC_START_START_Msk;

            DMA_InitStruct.DMA_DstAddress    = (u32)(&ADC_REG_CONV);
            DMA_InitStruct.DMA_DstInc        = DMA_DstAddress_Fix;
            DMA_InitStruct.DMA_SrcAddress    = (u32)(&AdcRunConv);
            DMA_InitStruct.DMA_SrcInc        = DMA_SrcAddress_Fix;
            DMA_InitStruct.DMA_TransferCnt   = ADC_SRC_DMA_BUFSIZE - 1;
            DMA_InitStruct.DMA_TransferWidth = DMA_TRANSFER_WIDTH_8BIT;

            DMA_Init(ADC_CONV_DMA_CH, &DMA_InitStruct);
        }

        {
            DMA_ITConfig(ADC_RES_DMA_CH, DMA_IT_TC, ENABLE);

            __disable_irq();
            NVIC_EnableIRQ(ADC_RES_DMA_IRQn);
            __enable_irq();
        }

        DMA_Cmd(ADC_RES_DMA_CH, ENABLE);
        DMA_Cmd(ADC_SRC_DMA_CH, ENABLE);
        DMA_Cmd(ADC_CONV_DMA_CH, ENABLE);
    }

    ADC_SoftwareStartConvCmd(ENABLE);

    // EOA:多次转换完成标志
    // EOS:序列转换完成标志
    // EOC:转换完成标志
    // ADC_ITConfig(ADC_IT_EOS, ENABLE);
    // ADC_EnableIrq(ADC_INT_PRIORITY);
}

void ADC_RES_DMA_IRQHandler(void)
{
    // DMA [bit16]: REPEAT
    if (DMA_GetITStatus(ADC_RES_DMA_IT_TC))
    {
        // 清除中断标志位
        DMA_ClearITPendingBit(ADC_RES_DMA_IT_TC);
        ADC_ClearITPendingAll();

        // read result
        ADC_RES_DMA_CH->CNT     = bv16 | (ADC_RES_DMA_BUFSIZE);
        ADC_RES_DMA_CH->DSTADDR = (u32)(&ADC_RES_DMA_BUFADDR[0]);
        DMA_Cmd(ADC_RES_DMA_CH, ENABLE);

        // select channel
        ADC_REG_CHSEL           = ADC_SRC_DMA_BUFADDR[0];
        ADC_SRC_DMA_CH->CNT     = bv16 | (ADC_SRC_DMA_BUFSIZE - 1);
        ADC_SRC_DMA_CH->SRCADDR = (u32)(&ADC_SRC_DMA_BUFADDR[1]);
        DMA_Cmd(ADC_SRC_DMA_CH, ENABLE);

        // start conv
        ADC_CONV_DMA_CH->CNT = ADC_SRC_DMA_CH->CNT;
        DMA_Cmd(ADC_CONV_DMA_CH, ENABLE);

        ADC_SoftwareStartConvCmd(ENABLE);
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
