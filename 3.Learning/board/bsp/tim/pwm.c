#include "pwm.h"
#include "pinmap.h"

#define PWM_Period       1600

#define CONFIG_CTRL_MODE 1  // 0: 6step, 1: foc

void PwmInit(void)
{
    // GPIO
    {
        /**
         * PA8  PB13 CH1A CH1B
         * PA9  PB14 CH2A CH2B
         * PA10 PB15 CH3A CH3B
         */

        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        GPIO_InitTypeDef GPIO_InitStruct = {
            .IT    = GPIO_IT_NONE,
            .Speed = GPIO_SPEED_HIGH,
        };

        PA08_AFx_ATIMCH1A();
        PA09_AFx_ATIMCH2A();
        PA10_AFx_ATIMCH3A();

#if CONFIG_CTRL_MODE
        PB13_AFx_ATIMCH1B();
        PB14_AFx_ATIMCH2B();
        PB15_AFx_ATIMCH3B();
#endif

        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pins = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);

#if !CONFIG_CTRL_MODE
        GPIO_WritePin(MOTOR_UL_GPIO_PORT, MOTOR_UL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
        GPIO_WritePin(MOTOR_VL_GPIO_PORT, MOTOR_VL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
        GPIO_WritePin(MOTOR_WL_GPIO_PORT, MOTOR_WL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
#endif
    }
    // TIM
    {
        // 载波频率 freq = 64M (sysclk) / 2 (CENTER_ALIG) / 1600 (PWM_Period) = 20kHz

        ATIM_InitTypeDef ATIM_InitStruct = {
            .BufferState        = DISABLE,
            .ClockSelect        = ATIM_CLOCK_PCLK,
            .CounterAlignedMode = ATIM_COUNT_MODE_CENTER_ALIGN,
            .CounterOPMode      = ATIM_OP_MODE_REPETITIVE,
            .Prescaler          = ATIM_Prescaler_DIV1,
            .ReloadValue        = PWM_Period - 1,
            .RepetitionCounter  = 0,
        };

        ATIM_OCInitTypeDef ATIM_OCInitStruct = {
            .BufferState       = DISABLE,
            .OCDMAState        = DISABLE,
            .OCInterruptSelect = ATIM_OC_IT_NONE,
            .OCInterruptState  = DISABLE,
            .OCMode            = ATIM_OCMODE_PWM2,
        };

        __RCC_ATIM_CLK_ENABLE();

        ATIM_Init(&ATIM_InitStruct);

        // 输出极性是否翻转看具体芯片哦！！
        ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_NONINVERT;
        ATIM_OC1AInit(&ATIM_OCInitStruct);
        ATIM_OC2AInit(&ATIM_OCInitStruct);
        ATIM_OC3AInit(&ATIM_OCInitStruct);

#if CONFIG_CTRL_MODE
        ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_INVERT;
        ATIM_OC1BInit(&ATIM_OCInitStruct);
        ATIM_OC2BInit(&ATIM_OCInitStruct);
        ATIM_OC3BInit(&ATIM_OCInitStruct);
#endif
        motor_stop();
        // 互补输出，插入死区
        ATIM_PWMOutputConfig(OCREFA_TYPE_DOUBLE, OUTPUT_TYPE_COMP, 2);

        // ATIM_CtrlPWMOutputs(DISABLE);
        ATIM_Cmd(ENABLE);
    }
}

//

void motor_start(void)
{
    ATIM_CtrlPWMOutputs(ENABLE);
}

void motor_stop(void)
{
    ATIM_CtrlPWMOutputs(DISABLE);
    ATIM_SetCompare1A(0);
    ATIM_SetCompare2A(0);
    ATIM_SetCompare3A(0);
    GPIO_WritePin(MOTOR_UL_GPIO_PORT, MOTOR_UL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_VL_GPIO_PORT, MOTOR_VL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_WL_GPIO_PORT, MOTOR_WL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
}

typedef void (*step6_cbk_t)(u16 duty);

step6_cbk_t funclist[6] = {
    &step6_uhwl, &step6_vhul, &step6_vhwl,
    &step6_whvl, &step6_uhvl, &step6_whul};

void step6_uhvl(u16 duty)
{
    ATIM_SetCompare1A(duty);  // u
    ATIM_SetCompare2A(0);     // v
    ATIM_SetCompare3A(0);     // w

    GPIO_WritePin(MOTOR_UL_GPIO_PORT, MOTOR_UL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_VL_GPIO_PORT, MOTOR_VL_GPIO_PIN, MOTOR_LOW_SIDE_ON);
    GPIO_WritePin(MOTOR_WL_GPIO_PORT, MOTOR_WL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
}

void step6_uhwl(u16 duty)
{
    ATIM_SetCompare1A(duty);
    ATIM_SetCompare2A(0);
    ATIM_SetCompare3A(0);

    GPIO_WritePin(MOTOR_UL_GPIO_PORT, MOTOR_UL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_VL_GPIO_PORT, MOTOR_VL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_WL_GPIO_PORT, MOTOR_WL_GPIO_PIN, MOTOR_LOW_SIDE_ON);
}

void step6_vhwl(u16 duty)
{
    ATIM_SetCompare1A(0);
    ATIM_SetCompare2A(duty);
    ATIM_SetCompare3A(0);

    GPIO_WritePin(MOTOR_UL_GPIO_PORT, MOTOR_UL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_VL_GPIO_PORT, MOTOR_VL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_WL_GPIO_PORT, MOTOR_WL_GPIO_PIN, MOTOR_LOW_SIDE_ON);
}

void step6_vhul(u16 duty)
{
    ATIM_SetCompare1A(0);
    ATIM_SetCompare2A(duty);
    ATIM_SetCompare3A(0);

    GPIO_WritePin(MOTOR_UL_GPIO_PORT, MOTOR_UL_GPIO_PIN, MOTOR_LOW_SIDE_ON);
    GPIO_WritePin(MOTOR_VL_GPIO_PORT, MOTOR_VL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_WL_GPIO_PORT, MOTOR_WL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
}

void step6_whul(u16 duty)
{
    ATIM_SetCompare1A(0);
    ATIM_SetCompare2A(0);
    ATIM_SetCompare3A(duty);

    GPIO_WritePin(MOTOR_UL_GPIO_PORT, MOTOR_UL_GPIO_PIN, MOTOR_LOW_SIDE_ON);
    GPIO_WritePin(MOTOR_VL_GPIO_PORT, MOTOR_VL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_WL_GPIO_PORT, MOTOR_WL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
}

void step6_whvl(u16 duty)
{
    ATIM_SetCompare1A(0);
    ATIM_SetCompare2A(0);
    ATIM_SetCompare3A(duty);

    GPIO_WritePin(MOTOR_UL_GPIO_PORT, MOTOR_UL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
    GPIO_WritePin(MOTOR_VL_GPIO_PORT, MOTOR_VL_GPIO_PIN, MOTOR_LOW_SIDE_ON);
    GPIO_WritePin(MOTOR_WL_GPIO_PORT, MOTOR_WL_GPIO_PIN, MOTOR_LOW_SIDE_OFF);
}

typedef enum {
    EDGE_NONE,
    EDGE_RISING,
    EDGE_FALLING,
} EdgeChk_e;

typedef struct {
    void* pCur;
    void* pLast;
    u8    nSize;
    EdgeChk_e (*pfnCbk)(void*, void*);
} EdgeChk_t;

#define DeclareEdgeChk(type) \
                             \
    EdgeChk_e type##EdgeChk(EdgeChk_t* pEdge);

#define DefineEdgeChk(type)                   \
                                              \
    EdgeChk_e type##EdgeChk(EdgeChk_t* pEdge) \
    {                                         \
        type last = *(type*)(pEdge->pLast);   \
        type cur  = *(type*)(pEdge->pCur);    \
                                              \
        EdgeChk_e ret;                        \
                                              \
        if (last > cur)                       \
        {                                     \
            ret = EDGE_FALLING;               \
        }                                     \
        else if (last < cur)                  \
        {                                     \
            ret = EDGE_RISING;                \
        }                                     \
        else                                  \
        {                                     \
            ret = EDGE_NONE;                  \
        }                                     \
                                              \
        *(type*)(pEdge->pLast) = cur;         \
    }

DeclareEdgeChk(f32);
DeclareEdgeChk(u32);
DeclareEdgeChk(u16);
DeclareEdgeChk(u8);
DeclareEdgeChk(s32);
DeclareEdgeChk(s16);
DeclareEdgeChk(s8);

DefineEdgeChk(f32);
DefineEdgeChk(u32);
DefineEdgeChk(u16);
DefineEdgeChk(u8);
DefineEdgeChk(s32);
DefineEdgeChk(s16);
DefineEdgeChk(s8);

EdgeChk_e GetUemfEdge(EdgeChk_e pEdge)
{
    // 0->1, 1->0, 计算电平持续时间
    return u8EdgeChk(pEdge);
}

RO u8 hall_table_cw[6]  = {6, 2, 3, 1, 5, 4};  // 顺时针旋转表
RO u8 hall_table_ccw[6] = {5, 1, 3, 2, 6, 4};  // 逆时针旋转表

/**
 * @brief 方向检测
 * @param [in] prev previous hall state
 * @param [in] cur  current hall state
 * @return direction
 */
direction_e HallDirChk(u8 prev, u8 cur)
{
    static RO u8 pTblCW[12]  = {0x62, 0x23, 0x31, 0x15, 0x54, 0x46, 0x63, 0x21, 0x35, 0x14, 0x56, 0x42};
    static RO u8 pTblCCW[12] = {0x45, 0x51, 0x13, 0x32, 0x26, 0x64, 0x41, 0x53, 0x12, 0x36, 0x24, 0x65};

    u8 match = ((prev & 0x0F) << 4) | (cur 0x0F);

    for (u8 i = 0; i < ARRAY_SIZE(pTblCW); ++i)
    {
        if (match == pTblCW[i])
        {
            return DIR_CCW;
        }
    }

    for (u8 i = 0; i < ARRAY_SIZE(pTblCCW); ++i)
    {
        if (match == pTblCCW[i])
        {
            return DIR_CCW;
        }
    }

    return DIR_NONE;
}