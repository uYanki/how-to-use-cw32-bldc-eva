
#include "main.h"

// 主流的无刷直流电机的控制方式有3种：FOC (又称为矢量变频、磁场矢量定向控制)、方波控制 (也称为梯形波控制、120°控制、6步换向控制)和正弦波控制。

#define MOTOR_POLE_PAIRS 2  // 极对数

/////

static adc_result_t adcDatBuf;

/**
 * @brief
 *
 * "High side floating supply return" 是一个术语，通常用于描述电路中的电源回路。在这个术语中，“high
 * side” 指的是电路中与高电位点相连的部分，而“floating” 指的是这个回路没有与地电位相连。因此，“high
 * side floating supply return” 意味着电源回路连接在电路的高电位点，但是没有直接连接到电路的地电位点。
 *
 * 当电机运转时，它会产生一个反向电动势，这个电动势大小与电机的转速成正比。
 * 如果我们将电机的一个引脚连接到高边浮动电源回路，然后将另一个引脚连接到地电位，那么电机的反电动势就可以通过这个浮动电源回路测量出来。
 * 具体而言，当电机运转时，它产生的反电动势会向高边浮动电源回路提供一个负电压
 * 。通过测量这个负电压，我们可以推导出电机的转速。这种测量方法通常用于电机控制和监
 * 测应用中，可以帮助我们了解电机的运转情况，以便进行调节和控制。
 *
 * 电机的反电动势（back electromotive force，简称 BEMF）
 *
 */

static RO u8 adcChSel[] = {
    bv7 | 0x0,  // (AIN0) PA00  U相反电动势
    bv7 | 0x1,  // (AIN1) PA01  V相反电动势
    bv7 | 0x5,  // (AIN5) PA05  W相反电动势
    bv7 | 0x3,  // (AIN3) PA03  母线电压
    bv7 | 0x6,  // (AIN6) PA06  电流
    bv7 | 0x7,  // (AIN7) PA07  电流
    bv7 | 0x8,  // (AIN8) PB00  电位器 RV1 (左旋增, 右旋减)
    bv7 | 0x4,  // (AIN4) PA04  NTC热敏电阻 (阻值和温度成反比)
};

#define adcDatLen ARRAY_SIZE(adcChSel)

void* _memset(void* src, u8 chr, u32 len)
{
    u8* p = src;
    while (len--) { *p++ = chr; }
    return src;
}

volatile u32 nHallTotalCnt = 0;

/////

static void LED_Configuration(void);
static void PWM_Configuration(void);
static void ADC_Configuration(void);
static void ENC_Configuration(void);

static f32 NTC_GetTemp(u16 adc);

static void switchNextPhase(u8 nNextStep, u16 nPWMOut);

int main()
{
    board_init();

    InitTick(CONFIG_SYSCLK_FREQ);

    delay1ms(100);

    LED_Configuration();
    PWM_Configuration();
    _memset(&adcDatBuf, 0, sizeof(adcDatBuf));
    ADC_Configuration();
    ENC_Configuration();
    u32 DIin = 1230;  // 偏置电流设定

    delay1ms(1);

    u16 ib_bias = adcDatBuf.Ib;

    for (;;) {
        if (g_ticks % 4 == 0) {
#if 0
            f32 Vmdc = ADC_12R_3V3(adcDatBuf.Vbus) * 40.2156f;  // (5.1k+200k) / (5.1k)
            f32 Ue = ADC_12R_3V3(adcDatBuf.Ue), Ve = ADC_12R_3V3(adcDatBuf.Ve), We = ADC_12R_3V3(adcDatBuf.We);
            f32 temp    = NTC_GetTemp(adcDatBuf.ntc);
            // f32 current = ADC_12R_3V3(adcDatBuf.Ib - adcDatBuf.Ic) / (f32)10e3 * 1e3;
            f32 current_b = ADC_12R_3V3((adcDatBuf.Ib )) / 4.3 /* A = (33K/10K+1) */ / 0.1 /* 100mR */ * 1e3;  // mA
						f32 current_c = ADC_12R_3V3((adcDatBuf.Ic )) / 4.3 /* A = (33K/10K+1) */ / 0.1 /* 100mR */ * 1e3;  // mA
            println("%f,%f,%f,%f,%d,%f,%f,%f", Ue, Ve, We, Vmdc, adcDatBuf.Vpot, current_b, current_c, temp);
#else
            GPIO_TogglePin(PIN_LED1);
            GPIO_TogglePin(PIN_LED2);
            GPIO_TogglePin(PIN_LED3);

            // println("%d,%d,%d", CW_ATIM->CH1CCRA, CW_ATIM->CH2CCRA, CW_ATIM->CH3CCRA);
#endif
        }
    }

    // ATIM_BrakeConfig(ATIM_BRAKE_POLARITY_NONINVERT, ATIM_BRAKE_FILTER_NONE);
}

void BTIM1_IRQCallBack()
{
    // calc speed

#if CONFIG_DEBUG_TICKER_PERIOD_N_MS <= 20
#define TIMES 50
#elif CONFIG_DEBUG_TICKER_PERIOD_N_MS <= 40
#define TIMES 25
#else
#define TIMES 1
#endif

    if (g_ticks % TIMES == 0) {  // time interval
        static u32 nHallLast = 0;

        u32 nHallCur = nHallTotalCnt;
        s16 nDelta   = nHallCur - nHallLast;

        if (nDelta > 0) {
            /// @note 12: 转1圈霍尔编码器计数12次
            u16 rpm = nDelta * (60000.f / CONFIG_DEBUG_TICKER_PERIOD_N_MS / TIMES) / 12;
            println("rpm:%d", rpm);
        }

        nHallLast = nHallCur;
    }

#undef TIMES
}

static f32 NTC_GetTemp(u16 adc)
{
    static RO u16 tbl[98] = {28017, 26826, 25697, 24629, 23618, 22660, 21752, 20892, 20075, 19299, 18560, 18482, 18149, 17632, 16992, 16280, 15535, 14787, 14055, 13354, 12690, 12068, 11490, 10954, 10458, 10000, 9576, 9184, 8819, 8478, 8160, 7861, 7579, 7311, 7056, 6813, 6581, 6357, 6142, 5934, 5734, 5541, 5353, 5173, 4998, 4829, 4665, 4507, 4355, 4208, 4065, 3927, 3794, 3664, 3538, 3415, 3294, 3175, 3058, 2941, 2825, 2776, 2718, 2652, 2582, 2508, 2432, 2356, 2280, 2207, 2135, 2066, 2000, 1938, 1879, 1823, 1770, 1720, 1673, 1628, 1586, 1546, 1508, 1471, 1435, 1401, 1367, 1334, 1301, 1268, 1236, 1204, 1171, 1139, 1107, 1074, 1042, 1010};

    u16 min = 0, max = ARRAY_SIZE(tbl) - 1, mid;

    f32 t = adc / 4095.f * 3300.f, ret;

    t /= (5 - t / 1000);

    ret = t * 10;

    while (1) {
        mid = (max + min) / 2;
        (tbl[mid] < ret) ? (max = mid) : (min = mid);
        if ((max - min) <= 1) break;
    }

    if (max == min)
        ret = min;
    else {
        ret = min + (tbl[min] - ret) / (tbl[min] - tbl[max]);
    }

    return ret;
}

static void ENC_Configuration(void)
{
    {
        __RCC_GTIM2_CLK_ENABLE();
        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        PA15_AFx_GTIM2CH1();  // HA
        PB03_AFx_GTIM2CH2();  // HB
        PA02_AFx_GTIM2CH3();  // HC
    }
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Mode  = GPIO_MODE_INPUT_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStruct.Pins = GPIO_PIN_15 | GPIO_PIN_2;
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_3;
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    }
    {
        GTIM_InitTypeDef   GTIM_InitStruct;
        GTIM_ICInitTypeDef GTIM_ICInitStruct;

        GTIM_InitStruct.Mode           = GTIM_MODE_TIME;
        GTIM_InitStruct.OneShotMode    = GTIM_COUNT_CONTINUE;
        GTIM_InitStruct.Prescaler      = GTIM_PRESCALER_DIV1;
        GTIM_InitStruct.ReloadValue    = 0xFF;
        GTIM_InitStruct.ToggleOutState = DISABLE;
        GTIM_TimeBaseInit(CW_GTIM2, &GTIM_InitStruct);

        GTIM_ICInitStruct.ICFilter   = GTIM_CHx_FILTER_PCLK_N2;
        GTIM_ICInitStruct.ICInvert   = GTIM_CHx_INVERT_OFF;
        GTIM_ICInitStruct.ICPolarity = GTIM_ICPolarity_BothEdge;

        GTIM_ICInitStruct.CHx = GTIM_CHANNEL1;
        GTIM_ICInit(CW_GTIM2, &GTIM_ICInitStruct);
        GTIM_ICInitStruct.CHx = GTIM_CHANNEL2;
        GTIM_ICInit(CW_GTIM2, &GTIM_ICInitStruct);
        GTIM_ICInitStruct.CHx = GTIM_CHANNEL3;
        GTIM_ICInit(CW_GTIM2, &GTIM_ICInitStruct);
    }
    {
        GTIM_ITConfig(CW_GTIM2, GTIM_IT_CC1 | GTIM_IT_CC2 | GTIM_IT_CC3, ENABLE);

        __disable_irq();
        NVIC_EnableIRQ(GTIM2_IRQn);
        __enable_irq();
    }

    GTIM_Cmd(CW_GTIM2, ENABLE);
}

void GTIM2_IRQHandler(void)
{
    static RO u8 steps[6] = {1, 3, 2, 5, 0, 4};

    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC1);
    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC2);
    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC3);

    ++nHallTotalCnt;

    u8 x = 0;
    if (PA15_GETVALUE()) x |= 0x1;
    if (PB03_GETVALUE()) x |= 0x2;
    if (PA02_GETVALUE()) x |= 0x4;
    switchNextPhase(steps[x - 1], (f32)adcDatBuf.Vpot / 4095 * 100);
}

static void switchNextPhase(u8 nNextStep, u16 nPWMOut)
{
    ///< 方波变换: 6步换向

#define CH1ASW(state)   GPIO_WritePin(CW_GPIOA, GPIO_PIN_8, state ? GPIO_Pin_SET : GPIO_Pin_RESET)
#define CH2ASW(state)   GPIO_WritePin(CW_GPIOA, GPIO_PIN_9, state ? GPIO_Pin_SET : GPIO_Pin_RESET)
#define CH3ASW(state)   GPIO_WritePin(CW_GPIOA, GPIO_PIN_10, state ? GPIO_Pin_SET : GPIO_Pin_RESET)

    // set off : GPIO_Pin_SET, set on: GPIO_Pin_RESET
#define CH1BSW(state)   GPIO_WritePin(CW_GPIOB, GPIO_PIN_13, state ? GPIO_Pin_RESET : GPIO_Pin_SET)
#define CH2BSW(state)   GPIO_WritePin(CW_GPIOB, GPIO_PIN_14, state ? GPIO_Pin_RESET : GPIO_Pin_SET)
#define CH3BSW(state)   GPIO_WritePin(CW_GPIOB, GPIO_PIN_15, state ? GPIO_Pin_RESET : GPIO_Pin_SET)

#define CHACFG(U, V, W) ATIM_SetCompare1A(U), ATIM_SetCompare2A(V), ATIM_SetCompare3A(W);

    ATIM_CtrlPWMOutputs(DISABLE);

    /**
     *
     *          0	1	2	3	4	5
     *  CH1A 			X	X	X	X
     *  CH2A 	X	X			X	X
     *  CH3A 	X	X	X	X
     *  CH1B 	X	X	X			X
     *  CH2B 		X	X	X	X
     *  CH3B 	X			X	X	X
     *
     *
     */

    switch (nNextStep) {
        case 0:  // AB
        case 5:  // CB
        {
            CH1BSW(0);
            CH3BSW(0);
            break;
        }
        case 1:  // AC
        case 2:  // BC
        {
            CH1BSW(0);
            CH2BSW(0);
            break;
        }
        case 3:  // BA
        case 4:  // CA
        {
            CH2BSW(0);
            CH3BSW(0);
            break;
        }
    }

    switch (nNextStep) {
        case 0:  // AB
        case 1:  // AC
        {
            CHACFG(nPWMOut, 0, 0);
            break;
        }
        case 2:  // BC
        case 3:  // BA
        {
            CHACFG(0, nPWMOut, 0);
            break;
        }
        case 4:  // CA
        case 5:  // CB
        {
            CHACFG(0, 0, nPWMOut);
            break;
        }
    }

    // set on: 0
    switch (nNextStep) {
        case 0:  // AB
        case 5:  // CB
        {
            CH2BSW(1);
            break;
        }
        case 1:  // AC
        case 2:  // BC
        {
            CH3BSW(1);
            break;
        }
        case 3:  // BA
        case 4:  // CA
        {
            CH1BSW(1);
            break;
        }
    }

    ATIM_CtrlPWMOutputs(ENABLE);
}

static void LED_Configuration(void)
{
    {
        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();
        __RCC_GPIOC_CLK_ENABLE();
    }
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStruct.Pins = GPIO_PIN_13;  // LED1
        GPIO_Init(CW_GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_11;  // LED2
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_11;  // LED3
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    }
}

static void PWM_Configuration(void)
{
    {
        __RCC_ATIM_CLK_ENABLE();
        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        PA08_AFx_ATIMCH1A();  // UH
        PA09_AFx_ATIMCH2A();  // VH
        PA10_AFx_ATIMCH3A();  // WH

        PB13_AFx_GPIO();      // UL
        PB14_AFx_GPIO();      // VL
        PB15_AFx_GPIO();      // WL
    }
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStruct.Pins = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;    // 上管
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
        GPIO_WritePin(CW_GPIOA, GPIO_InitStruct.Pins, GPIO_Pin_RESET);   // OFF

        GPIO_InitStruct.Pins = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;  // 下管
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
        GPIO_WritePin(CW_GPIOB, GPIO_InitStruct.Pins, GPIO_Pin_SET);     // OFF
    }
    {
        ATIM_InitTypeDef ATIM_InitStruct;

        /**
         * @note FREQ = ( CONFIG_SYSCLK_FREQ / ATIM_Prescaler ) / ( ReloadValue + 1 )
         *
         *    -->  freq = ( 64e6 / 64 ) / 1e2 = 1e4 = 10khz
         */

        ATIM_InitStruct.CounterDirection   = ATIM_COUNTING_UP;
        ATIM_InitStruct.CounterOPMode      = ATIM_OP_MODE_REPETITIVE;
        ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_EDGE_ALIGN;
        ATIM_InitStruct.BufferState        = DISABLE;
        ATIM_InitStruct.ClockSelect        = ATIM_CLOCK_PCLK;
        ATIM_InitStruct.RepetitionCounter  = ATIM_OP_MODE_REPETITIVE;
        ATIM_InitStruct.UnderFlowMask      = DISABLE;
        ATIM_InitStruct.OverFlowMask       = DISABLE;
        ATIM_InitStruct.Prescaler          = ATIM_Prescaler_DIV64;
        ATIM_InitStruct.ReloadValue        = 100 - 1;

        ATIM_Init(&ATIM_InitStruct);
    }
    {
        ATIM_OCInitTypeDef ATIM_OCInitStruct;

        ATIM_OCInitStruct.OCPolarity        = ATIM_OCPOLARITY_NONINVERT;
        ATIM_OCInitStruct.OCMode            = ATIM_OCMODE_PWM1;
        ATIM_OCInitStruct.OCInterruptState  = DISABLE;
        ATIM_OCInitStruct.OCInterruptSelect = ATIM_OC_IT_NONE;
        ATIM_OCInitStruct.OCDMAState        = DISABLE;
        ATIM_OCInitStruct.BufferState       = DISABLE;

        ATIM_OC1AInit(&ATIM_OCInitStruct);
        ATIM_OC2AInit(&ATIM_OCInitStruct);
        ATIM_OC3AInit(&ATIM_OCInitStruct);

        ATIM_PWMOutputConfig(OCREFA_TYPE_SINGLE, OUTPUT_TYPE_ALONE, -1);
        // 该模式的 Deadtime 必须为 -1, 不然电机不转

        ATIM_SetCompare1A(0);  // U
        ATIM_SetCompare2A(0);  // V
        ATIM_SetCompare3A(0);  // W

        ATIM_CtrlPWMOutputs(ENABLE);
    }

    ATIM_Cmd(ENABLE);
}

void ATIM_IRQHandler(void)
{
}

static void ADC_Configuration(void)
{
    /**
     * @note ADC.CR0 ( Control Register , 控制寄存器 )
     *
     * - BIT[7]: ADC 每次转换完成时触发 DMA
     *
     * - BIT[3:0]: 通道选择
     *
     *          0000：AIN0????1000：AIN8
     *          0001：AIN1????1001：AIN9
     *          0010：AIN2????1010：AIN10
     *          0011：AIN3????1011：AIN11
     *          0100：AIN4????1100：AIN12
     *          0101：AIN5????1101：VDDA/3
     *          0110：AIN6????1110：TS 内置温度传感器
     *          0111：AIN7????1111：1.2V 内核电压基准源
     *
     * @note 使用 DMA 对 ADC 通道进行切换
     *
     *      原因: 规则转换通道只有 4 条 ( ADC_SerialCh... ), 不够用
     *
     */

    static RO u8 RUN = 1;

    {
        __RCC_ADC_CLK_ENABLE();
        __RCC_DMA_CLK_ENABLE();
        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        PA00_ANALOG_ENABLE();
        PA01_ANALOG_ENABLE();
        PA05_ANALOG_ENABLE();
        PA03_ANALOG_ENABLE();
        PA04_ANALOG_ENABLE();
        PA06_ANALOG_ENABLE();
        PA07_ANALOG_ENABLE();
        PB00_ANALOG_ENABLE();
    }

    {
        ADC_InitTypeDef ADC_InitStruct;

        ADC_InitStruct.ADC_AccEn      = ADC_AccDisable;
        ADC_InitStruct.ADC_Align      = ADC_AlignRight;
        ADC_InitStruct.ADC_ClkDiv     = ADC_Clk_Div64;
        ADC_InitStruct.ADC_DMAEn      = ADC_DmaEnable;
        ADC_InitStruct.ADC_InBufEn    = ADC_BufDisable;
        ADC_InitStruct.ADC_OpMode     = ADC_SingleChOneMode;
        ADC_InitStruct.ADC_SampleTime = ADC_SampTime10Clk;
        ADC_InitStruct.ADC_TsEn       = ADC_TsDisable;
        ADC_InitStruct.ADC_VrefSel    = ADC_Vref_VDDA;

        ADC_Init(&ADC_InitStruct);
        ADC_Enable();
    }

    {
        DMA_InitTypeDef DMA_InitStruct;

        DMA_InitStruct.DMA_Mode = DMA_MODE_BLOCK;

        {
            // 搬运采样结果

            DMA_InitStruct.TrigMode       = DMA_HardTrig;
            DMA_InitStruct.HardTrigSource = DMA_HardTrig_ADC_TRANSCOMPLETE;

            DMA_InitStruct.DMA_DstAddress = (u32)&adcDatBuf;
            DMA_InitStruct.DMA_DstInc     = DMA_DstAddress_Increase;

            DMA_InitStruct.DMA_SrcAddress    = (u32)&CW_ADC->RESULT0;
            DMA_InitStruct.DMA_SrcInc        = DMA_SrcAddress_Fix;
            DMA_InitStruct.DMA_TransferCnt   = adcDatLen;
            DMA_InitStruct.DMA_TransferWidth = DMA_TRANSFER_WIDTH_16BIT;

            DMA_Init(CW_DMACHANNEL1, &DMA_InitStruct);
            DMA_Cmd(CW_DMACHANNEL1, ENABLE);
        }
        {
            // 切换采样通道

            DMA_InitStruct.TrigMode       = DMA_HardTrig;
            DMA_InitStruct.HardTrigSource = DMA_HardTrig_ADC_TRANSCOMPLETE;

            DMA_InitStruct.DMA_DstAddress = (u32)&CW_ADC->CR1;
            DMA_InitStruct.DMA_DstInc     = DMA_DstAddress_Fix;

            CW_ADC->CR1                      = adcChSel[0];  // default channel
            DMA_InitStruct.DMA_SrcAddress    = (u32)&adcChSel[1];
            DMA_InitStruct.DMA_SrcInc        = DMA_SrcAddress_Increase;
            DMA_InitStruct.DMA_TransferCnt   = adcDatLen - 1;
            DMA_InitStruct.DMA_TransferWidth = DMA_TRANSFER_WIDTH_8BIT;

            DMA_Init(CW_DMACHANNEL2, &DMA_InitStruct);
            DMA_Cmd(CW_DMACHANNEL2, ENABLE);
        }
        {
            // 触发采样

            DMA_InitStruct.TrigMode       = DMA_HardTrig;
            DMA_InitStruct.HardTrigSource = DMA_HardTrig_ADC_TRANSCOMPLETE;
            // DMA_InitStruct.HardTrigSource = DMA_HardTrig_ATIM_CH1A2A3A4;

            DMA_InitStruct.DMA_DstAddress = (u32)&CW_ADC->START;
            DMA_InitStruct.DMA_DstInc     = DMA_DstAddress_Fix;

            DMA_InitStruct.DMA_SrcAddress    = (u32)&RUN;
            DMA_InitStruct.DMA_SrcInc        = DMA_SrcAddress_Fix;
            DMA_InitStruct.DMA_TransferCnt   = adcDatLen - 1;
            DMA_InitStruct.DMA_TransferWidth = DMA_TRANSFER_WIDTH_8BIT;

            DMA_Init(CW_DMACHANNEL3, &DMA_InitStruct);
            DMA_Cmd(CW_DMACHANNEL3, ENABLE);
        }
    }
    {
        DMA_ITConfig(CW_DMACHANNEL1, DMA_IT_TC, ENABLE);

        __disable_irq();
        NVIC_EnableIRQ(DMACH1_IRQn);
        __enable_irq();
    }

    // DMA_SWTrigCmd(CW_DMACHANNEL3);
    ADC_SoftwareStartConvCmd(ENABLE);
}

void DMACH1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA_IT_TC1)) {
        DMA_ClearITPendingBit(DMA_IT_TC1);
        ADC_ClearITPendingAll();

        // read result
        CW_DMACHANNEL1->DSTADDR = (u32)(&adcDatBuf);
        CW_DMACHANNEL1->CNT     = bv16 | adcDatLen;

        // select channel
        CW_ADC->CR1             = adcChSel[0];
        CW_DMACHANNEL2->SRCADDR = (u32)(&adcChSel[1]);
        CW_DMACHANNEL2->CNT     = bv16 | (adcDatLen - 1);

        // start conv
        CW_DMACHANNEL3->CNT = CW_DMACHANNEL2->CNT;

        DMA_Cmd(CW_DMACHANNEL1, ENABLE);
        DMA_Cmd(CW_DMACHANNEL2, ENABLE);
        DMA_Cmd(CW_DMACHANNEL3, ENABLE);

        ADC_SoftwareStartConvCmd(ENABLE);
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
