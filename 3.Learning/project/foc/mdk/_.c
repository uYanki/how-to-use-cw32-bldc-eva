@ @-0, 0 + 1, 604 @ @

#include "main.h"
#include "motor.h"

// ��������ˢֱ������Ŀ��Ʒ�ʽ��3�֣�FOC (�ֳ�Ϊʸ����Ƶ���ų�ʸ���������)���������� (Ҳ��Ϊ���β����ơ�120����ơ�6���������)�����Ҳ����ơ�

#define MOTOR_POLE_PAIRS 2  // ������

    /////

    static adc_result_t adcDatBuf;

u8 cur_section;

/**
 * @brief
 *
 * "High side floating supply return" ��һ�����ͨ������������·�еĵ�Դ��·������������У���high
 * side�� ָ���ǵ�·����ߵ�λ�������Ĳ��֣�����floating�� ָ���������·û����ص�λ��������ˣ���high
 * side floating supply return�� ��ζ�ŵ�Դ��·�����ڵ�·�ĸߵ�λ�㣬����û��ֱ�����ӵ���·�ĵص�λ�㡣
 *
 * �������תʱ���������һ������綯�ƣ�����綯�ƴ�С������ת�ٳ����ȡ�
 * ������ǽ������һ���������ӵ��߱߸�����Դ��·��Ȼ����һ���������ӵ��ص�λ����ô����ķ��綯�ƾͿ���ͨ�����������Դ��·����������
 * ������ԣ��������תʱ���������ķ��綯�ƻ���߱߸�����Դ��·�ṩһ������ѹ
 * ��ͨ�������������ѹ�����ǿ����Ƶ��������ת�١����ֲ�������ͨ�����ڵ�����ƺͼ�
 * ��Ӧ���У����԰��������˽�������ת������Ա���е��ںͿ��ơ�
 *
 * ����ķ��綯�ƣ�back electromotive force����� BEMF��
 *
 */

static RO u8 adcChSel[] = {
    bv7 | 0x0,  // (AIN0) PA00  U�෴�綯��
    bv7 | 0x1,  // (AIN1) PA01  V�෴�綯��
    bv7 | 0x5,  // (AIN5) PA05  W�෴�綯��
    bv7 | 0x3,  // (AIN3) PA03  ĸ�ߵ�ѹ
    bv7 | 0x6,  // (AIN6) PA06  ����
    bv7 | 0x7,  // (AIN7) PA07  ����
    bv7 | 0x8,  // (AIN8) PB00  ��λ�� RV1 (������, ������)
    bv7 | 0x4,  // (AIN4) PA04  NTC�������� (��ֵ���¶ȳɷ���)
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
static void KEY_Configuration(void);

static f32 NTC_GetTemp(u16 adc);

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

    delay1ms(1);

    Bool_e bRunState = 0;
    for (;;)
    {
        bRunState = adcDatBuf.Vpot > 1000;
        GPIO_WritePin(PIN_LED3, bRunState);  // ����ָʾ��
        if (bRunState)
        {
        }
        else
        {
            if (g_ticks & 1)
            {
                // FOC2();

                static u16 angle = 0;
                sine_wave_drive((angle += 10) % 3600, 4000);
            }
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

    if (g_ticks % TIMES == 0)
    {  // time interval
        static u32 nHallLast = 0;

        u32 nHallCur = nHallTotalCnt;
        s16 nDelta   = nHallCur - nHallLast;

        if (nDelta > 0)
        {
            /// @note 12: ת1Ȧ��������������12��
            u16 rpm = nDelta * (60000.f / CONFIG_DEBUG_TICKER_PERIOD_N_MS / TIMES) / 12;
            println("rpm:%d", rpm);
        }

        nHallLast = nHallCur;
    }

#undef TIMES
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

        PB13_AFx_ATIMCH1B();  // UL
        PB14_AFx_ATIMCH2B();  // VL
        PB15_AFx_ATIMCH3B();  // WL
    }
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStruct.Pins = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;  // �Ϲ�
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
        GPIO_WritePin(CW_GPIOA, GPIO_InitStruct.Pins, GPIO_Pin_RESET);  // OFF

        GPIO_InitStruct.Pins = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;  // �¹�
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
        // GPIO_WritePin(CW_GPIOB, GPIO_InitStruct.Pins, GPIO_Pin_SET);     // OFF
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
        ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_CENTER_ALIGN;
        ATIM_InitStruct.BufferState        = DISABLE;
        ATIM_InitStruct.ClockSelect        = ATIM_CLOCK_PCLK;
        ATIM_InitStruct.RepetitionCounter  = ATIM_OP_MODE_REPETITIVE;
        ATIM_InitStruct.UnderFlowMask      = DISABLE;
        ATIM_InitStruct.OverFlowMask       = DISABLE;
        ATIM_InitStruct.Prescaler          = ATIM_Prescaler_DIV16;
        ATIM_InitStruct.ReloadValue        = 4200;

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
        ATIM_OC1BInit(&ATIM_OCInitStruct);
        ATIM_OC2BInit(&ATIM_OCInitStruct);
        ATIM_OC3BInit(&ATIM_OCInitStruct);

        ATIM_PWMOutputConfig(OCREFA_TYPE_DOUBLE, OUTPUT_TYPE_COMP, 1);

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
     * @note ADC.CR0 ( Control Register , ���ƼĴ��� )
     *
     * - BIT[7]: ADC ÿ��ת�����ʱ���� DMA
     *
     * - BIT[3:0]: ͨ��ѡ��
     *
     *          0000��AIN0????1000��AIN8
     *          0001��AIN1????1001��AIN9
     *          0010��AIN2????1010��AIN10
     *          0011��AIN3????1011��AIN11
     *          0100��AIN4????1100��AIN12
     *          0101��AIN5????1101��VDDA/3
     *          0110��AIN6????1110��TS �����¶ȴ�����
     *          0111��AIN7????1111��1.2V �ں˵�ѹ��׼Դ
     *
     * @note ʹ�� DMA �� ADC ͨ�������л�
     *
     *      ԭ��: ����ת��ͨ��ֻ�� 4 �� ( ADC_SerialCh... ), ������
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
            // ���˲������

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
            // �л�����ͨ��

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
            // ��������

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
    if (DMA_GetITStatus(DMA_IT_TC1))
    {
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
