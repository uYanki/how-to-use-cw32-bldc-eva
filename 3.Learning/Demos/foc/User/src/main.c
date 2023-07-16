#include "board.h"

#include "math.h"

#define delay(n)       FirmwareDelay(100 * (n))

/**
 * 6xPWM: ATIM
 * U+ PA8
 * V+:PA9
 * W+:PA10
 *
 * U-:PB13
 * V-:PB14
 * W-:PB15
 *
 * 霍尔
 * HA:PA15
 * HB:PB3
 * HC:PA2
 *
 * 电位器调速 PB0
 *
 * 上管调制，下管IO，
 * 上管高电平，下管低电平驱动
 *
 */

#define MotorPoles     2  // 电机极数

#define min(a, b)      (((a) < (b)) ? (a) : (b))
#define max(a, b)      (((a) > (b)) ? (a) : (b))

#define clamp(a, v, b) max(a, min(v, b))

float fVolMin = 0, fVolMax = 10, fVolPWR = 12;

uint32_t ticks = 0;

//

void tim_hall_init(void);

#define PWMReloadValue 3200

const unsigned int PWM_EN_TAB[6] = {0x001, 0x001, 0x010, 0x010, 0x100, 0x100};  //	6²½PWMÊä³öÊ¹ÄÜ¿ØÖÆ	AB,AC,BC,BA,CA,CB

bool     bMotorRun   = true;
uint8_t  nNextStep   = 0;
uint32_t nPulseCount = 0;
uint16_t nADCin = 220, nPWMout = 0;

float32_t zero_electricalAngle = 0;

#define _PI  3.1415926
#define _2PI (_PI * 2)

void adc_init(void);

void pwm_init(void);
void pwm_set(uint8_t step, uint16_t val);

void uart_init(uint32_t baudrate, uint32_t clkfreq);

void btim_init(void);

#define PWM_AL_OFF() GPIO_WritePin(CW_GPIOB, GPIO_PIN_13, GPIO_Pin_SET)
#define PWM_BL_OFF() GPIO_WritePin(CW_GPIOB, GPIO_PIN_14, GPIO_Pin_SET)
#define PWM_CL_OFF() GPIO_WritePin(CW_GPIOB, GPIO_PIN_15, GPIO_Pin_SET)

#define PWM_AL_ON()  GPIO_WritePin(CW_GPIOB, GPIO_PIN_13, GPIO_Pin_RESET)
#define PWM_BL_ON()  GPIO_WritePin(CW_GPIOB, GPIO_PIN_14, GPIO_Pin_RESET)
#define PWM_CL_ON()  GPIO_WritePin(CW_GPIOB, GPIO_PIN_15, GPIO_Pin_RESET)

float32_t calcElectricalAngle(float32_t fShaftAngle, int iPolePairs)
{
    // 电角度 = 机械角度(轴角度) * 极对数
    return fShaftAngle * iPolePairs;
}

float32_t normalizeAngle(float angle)
{
    // map to [0,2pi]
    float32_t a = fmod(angle, _2PI);
    return a >= 0 ? a : (a + _2PI);
}

void configPWM(float32_t Ua, float32_t Ub, float32_t Uc)
{
    float32_t duty_a, duty_b, duty_c;

    duty_a = clamp(fVolMin, Ua, fVolMax) / fVolPWR;
    duty_b = clamp(fVolMin, Ub, fVolMax) / fVolPWR;
    duty_c = clamp(fVolMin, Uc, fVolMax) / fVolPWR;

    // println("%.2f,%.2f,%.2f", duty_a, duty_b, duty_c);

    ATIM_SetCompare1A(PWMReloadValue * duty_a);
    ATIM_SetCompare2A(PWMReloadValue * duty_b);
    ATIM_SetCompare3A(PWMReloadValue * duty_c);

    PWM_AL_ON();
    PWM_BL_ON();
    PWM_CL_ON();
}

void setPhaseVoltage(float32_t Uq, float32_t Ud, float32_t electricalAngle)
{
    electricalAngle  = normalizeAngle(electricalAngle + zero_electricalAngle);
    // 帕克逆变化
    float32_t Ualpha = Ud * cos(electricalAngle) - Uq * sin(electricalAngle);
    float32_t Ubeta  = Ud * sin(electricalAngle) + Uq * cos(electricalAngle);
    // 克拉克逆变换
    float32_t Ua     = Ualpha;
    float32_t Ub     = (sqrt(3) * Ubeta - Ualpha) / 2;
    float32_t Uc     = -(Ualpha + sqrt(3) * Ubeta) / 2;

    Ua += fVolPWR / 2;
    Ub += fVolPWR / 2;
    Uc += fVolPWR / 2;

    configPWM(Ua, Ub, Uc);
}

float32_t fShaftAngle = 0;

float32_t openloop(float32_t targetVelocity)
{
    setPhaseVoltage(fVolMax, 0, calcElectricalAngle(normalizeAngle(fShaftAngle += .1), MotorPoles));

    return 0;

    static uint32_t us = 0;

    float32_t dt = (fShaftAngle - us) * 1e-6f;

    if (dt <= 0 || dt > 0.5f) dt = 1e-3f;

    // fShaftAngle  = normalizeAngle(fShaftAngle + targetVelocity * dt);

    fShaftAngle = normalizeAngle(fShaftAngle += .5);

    float32_t Uq = fVolMax;
    setPhaseVoltage(Uq, 0, calcElectricalAngle(fShaftAngle, MotorPoles));
    us = fShaftAngle;

    return Uq;
}

int main()
{
    board_init();
    uart_init(115200, CONFIG_SYSCLK_FREQ);
    // adc_init();
    // tim_hall_init();
    pwm_init();
    btim_init();

    for (;;) {
        // read ADC by DMA channel
        CW_DMACHANNEL1->CNT = bv16 | 1;
        DMA_Cmd(CW_DMACHANNEL1, ENABLE);
        ADC_SoftwareStartConvCmd(ENABLE);

        openloop(5);
    }
}

void pwm_init(void)
{
    {
        __RCC_ATIM_CLK_ENABLE();
        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        PA08_AFx_ATIMCH1A();
        PA09_AFx_ATIMCH2A();
        PA10_AFx_ATIMCH3A();
    }
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pins  = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pins  = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        PWM_AL_OFF();
        PWM_BL_OFF();
        PWM_CL_OFF();
    }
    {
        ATIM_InitTypeDef   ATIM_InitStruct;
        ATIM_OCInitTypeDef ATIM_OCInitStruct;

        // TIM ( freq = 20000Hz )

        ATIM_InitStruct.BufferState        = DISABLE;
        ATIM_InitStruct.ClockSelect        = ATIM_CLOCK_PCLK;
        ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_EDGE_ALIGN;
        ATIM_InitStruct.CounterDirection   = ATIM_COUNTING_UP;
        ATIM_InitStruct.CounterOPMode      = ATIM_OP_MODE_REPETITIVE;
        ATIM_InitStruct.Prescaler          = ATIM_Prescaler_DIV1;
        ATIM_InitStruct.ReloadValue        = PWMReloadValue;  // 64e6 Hz / 32e2 = 2e4 Hz
        ATIM_InitStruct.RepetitionCounter  = 0;
        ATIM_InitStruct.OverFlowMask       = DISABLE;
        ATIM_InitStruct.UnderFlowMask      = DISABLE;

        ATIM_Init(&ATIM_InitStruct);

        // PWM

        ATIM_OCInitStruct.BufferState       = DISABLE;
        ATIM_OCInitStruct.OCDMAState        = DISABLE;
        ATIM_OCInitStruct.OCInterruptSelect = ATIM_OC_IT_UP_COUNTER;
        ATIM_OCInitStruct.OCInterruptState  = ENABLE;
        ATIM_OCInitStruct.OCMode            = ATIM_OCMODE_PWM1;
        ATIM_OCInitStruct.OCPolarity        = ATIM_OCPOLARITY_NONINVERT;

        ATIM_OC1AInit(&ATIM_OCInitStruct);
        ATIM_OC2AInit(&ATIM_OCInitStruct);
        ATIM_OC3AInit(&ATIM_OCInitStruct);

        ATIM_SetCompare1A(0);
        ATIM_SetCompare2A(0);
        ATIM_SetCompare3A(0);

        ATIM_PWMOutputConfig(OCREFA_TYPE_SINGLE, OUTPUT_TYPE_COMP, 0);  // 死区互补
        ATIM_CtrlPWMOutputs(ENABLE);
        ATIM_Cmd(ENABLE);
    }
}

void pwm_set(uint8_t step, uint16_t val)
{
    PWM_AL_OFF();
    PWM_BL_OFF();
    PWM_CL_OFF();

    if (bMotorRun) {
        // 上桥
        switch (step) {
            case 0:  // AB
            case 1:  // AC
            {
                CW_ATIM->CH1CCRA = val;
                CW_ATIM->CH2CCRA = 0;
                CW_ATIM->CH3CCRA = 0;
                break;
            }
            case 2:  // BC
            case 3:  // BA
            {
                CW_ATIM->CH1CCRA = 0;
                CW_ATIM->CH2CCRA = val;
                CW_ATIM->CH3CCRA = 0;
                break;
            }
            case 4:  // CA
            case 5:  // CB
            {
                CW_ATIM->CH1CCRA = 0;
                CW_ATIM->CH2CCRA = 0;
                CW_ATIM->CH3CCRA = val;
                break;
            }
        }

        // 下桥
        switch (step) {
            case 0:  // AB
            case 5:  // CB
            {
                PWM_AL_OFF();
                PWM_CL_OFF();
                PWM_BL_ON();
                break;
            }
            case 1:  // AC
            case 2:  // BC
            {
                PWM_AL_OFF();
                PWM_BL_OFF();
                PWM_CL_ON();
                break;
            }
            case 3:  // BA
            case 4:  // CA
            {
                PWM_BL_OFF();
                PWM_CL_OFF();
                PWM_AL_ON();
                break;
            }
        }
        ATIM_CtrlPWMOutputs(ENABLE);
    } else {
        CW_ATIM->CH1CCRA = CW_ATIM->CH2CCRA = CW_ATIM->CH3CCRA = 0;
        ATIM_CtrlPWMOutputs(DISABLE);
    }
}

// hall sensor

void tim_hall_init(void)
{
    {
        __RCC_GTIM2_CLK_ENABLE();
        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        PA15_AFx_GTIM2CH1();
        PB03_AFx_GTIM2CH2();
        PA02_AFx_GTIM2CH3();

        __disable_irq();
        NVIC_EnableIRQ(GTIM2_IRQn);
        __enable_irq();
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

        GTIM_ITConfig(CW_GTIM2, GTIM_IT_CC1 | GTIM_IT_CC2 | GTIM_IT_CC3, ENABLE);
        GTIM_Cmd(CW_GTIM2, ENABLE);
    }
}

void GTIM2_IRQHandler(void)
{
    // const uint8_t PWM_EN_TAB[6]={0x001,0x001,0x010,0x010,0x100,0x100}; // AB,AC,BC,BA,CA,CB
    static const uint8_t steps[6] = {1, 3, 2, 5, 0, 4};  // {4,0,5,2,3,1};

    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC1);
    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC2);
    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC3);

    ++nPulseCount;

    uint8_t x = 0;
    if (PA15_GETVALUE()) x |= 0x1;
    if (PB03_GETVALUE()) x |= 0x2;
    if (PA02_GETVALUE()) x |= 0x4;
    nNextStep = steps[x - 1];  // switch to next step
    println("%d", nNextStep);

    // if (bMotorRun) pwm_set(nNextStep, nPWMout = nADCin);
}

// adc sampler

void adc_init(void)
{
    {
        __RCC_ADC_CLK_ENABLE();
        __RCC_DMA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();
        PB00_ANALOG_ENABLE();
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
        DMA_InitTypeDef DMA_InitStruct;

        DMA_InitStruct.DMA_Mode          = DMA_MODE_BLOCK;
        DMA_InitStruct.DMA_TransferWidth = DMA_TRANSFER_WIDTH_16BIT;
        DMA_InitStruct.DMA_SrcInc        = DMA_SrcAddress_Fix;
        DMA_InitStruct.DMA_DstInc        = DMA_DstAddress_Fix;
        DMA_InitStruct.TrigMode          = DMA_HardTrig;
        DMA_InitStruct.HardTrigSource    = DMA_HardTrig_ADC_TRANSCOMPLETE;
        DMA_InitStruct.DMA_TransferCnt   = 7;
        DMA_InitStruct.DMA_SrcAddress    = (uint32_t) & (CW_ADC->RESULT0);
        DMA_InitStruct.DMA_DstAddress    = (uint32_t) & (nADCin);

        DMA_Init(CW_DMACHANNEL1, &DMA_InitStruct);
        DMA_Cmd(CW_DMACHANNEL1, ENABLE);
    }
}

// console

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

// timer

void btim_init(void)
{
    {
        __RCC_BTIM_CLK_ENABLE();

        __disable_irq();
        NVIC_EnableIRQ(BTIM1_IRQn);
        __enable_irq();
    }
    {
        BTIM_TimeBaseInitTypeDef BTIM_InitStruct;

        BTIM_InitStruct.BTIM_Mode   = BTIM_Mode_TIMER;
        BTIM_InitStruct.BTIM_OPMode = BTIM_OPMode_Repetitive;
#if 0
        // 15625 / ( 64e6 / 4096 ) = 1s
        BTIM_InitStruct.BTIM_Period    = 15625;
        BTIM_InitStruct.BTIM_Prescaler = BTIM_PRS_DIV4096;
#else
        // 15625 / ( 64e6 / 4096 ) = 10us
        BTIM_InitStruct.BTIM_Period    = 10;
        BTIM_InitStruct.BTIM_Prescaler = BTIM_PRS_DIV64;
#endif
        BTIM_TimeBaseInit(CW_BTIM1, &BTIM_InitStruct);

        BTIM_ITConfig(CW_BTIM1, BTIM_IT_OV, ENABLE);
        BTIM_Cmd(CW_BTIM1, ENABLE);
    }
}

void BTIM1_IRQHandler(void)
{
    if (BTIM_GetITStatus(CW_BTIM1, BTIM_IT_OV)) {
        BTIM_ClearITPendingBit(CW_BTIM1, BTIM_IT_OV);
        ++ticks;
#if 0
        uint16_t rpm = nPulseCount * 20 / MotorPoles;
        println("[%s] %d rpm", bMotorRun ? "start" : "stop", rpm);
#endif
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
