#include "hallEnc.h"
#include "pinmap.h"

#define MotorPolePairs 2

// https://blog.csdn.net/csol1607408930/article/details/129410150

vu8  u8HallState  = 0;
vu16 u16ElecAngle = 0;
vu32 u32HallCount = 0;  // 用于测速

#define PWM_Period 1600

void HallEncInit(void)
{
    // GPIO
    {
        GPIO_InitTypeDef GPIO_InitStruct = {
            .Mode  = GPIO_MODE_INPUT_PULLUP,
            .IT    = GPIO_IT_NONE,
            .Speed = GPIO_SPEED_HIGH,
        };

        ENC_HA_GPIO_CLKEN(ENC_HA_GPIO_CLK, ENABLE);
        ENC_HB_GPIO_CLKEN(ENC_HB_GPIO_CLK, ENABLE);
        ENC_HC_GPIO_CLKEN(ENC_HC_GPIO_CLK, ENABLE);

        ENC_HA_GPIO_AF();
        ENC_HB_GPIO_AF();
        ENC_HC_GPIO_AF();

        GPIO_InitStruct.Pins = ENC_HA_GPIO_PIN;  // HA
        GPIO_Init(ENC_HA_GPIO_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = ENC_HB_GPIO_PIN;  // HB
        GPIO_Init(ENC_HB_GPIO_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = ENC_HC_GPIO_PIN;  // HC
        GPIO_Init(ENC_HC_GPIO_PORT, &GPIO_InitStruct);
    }
    // TIM
    {
        GTIM_InitTypeDef GTIM_InitStruct = {
            .Mode           = GTIM_MODE_TIME,
            .OneShotMode    = GTIM_COUNT_CONTINUE,
            .Prescaler      = GTIM_PRESCALER_DIV1,
            .ReloadValue    = 0xFF,
            .ToggleOutState = DISABLE,
        };

        GTIM_ICInitTypeDef GTIM_ICInitStruct = {
            .ICFilter   = GTIM_CHx_FILTER_PCLK_N2,
            .ICInvert   = GTIM_CHx_INVERT_OFF,
            .ICPolarity = GTIM_ICPolarity_BothEdge,
        };

        ENC_TIM_CLKEN(ENC_TIM_CLK, ENABLE);

        GTIM_TimeBaseInit(ENC_TIM_PORT, &GTIM_InitStruct);

        GTIM_ICInitStruct.CHx = GTIM_CHANNEL1;
        GTIM_ICInit(ENC_TIM_PORT, &GTIM_ICInitStruct);
        GTIM_ICInitStruct.CHx = GTIM_CHANNEL2;
        GTIM_ICInit(ENC_TIM_PORT, &GTIM_ICInitStruct);
        GTIM_ICInitStruct.CHx = GTIM_CHANNEL3;
        GTIM_ICInit(ENC_TIM_PORT, &GTIM_ICInitStruct);
    }
    // NVIC
    {
        GTIM_ITConfig(ENC_TIM_PORT, GTIM_IT_CC1 | GTIM_IT_CC2 | GTIM_IT_CC3, ENABLE);

        __disable_irq();
        NVIC_EnableIRQ(ENC_TIM_IRQn);
        __enable_irq();
    }

    HallEncUpdate();

    GTIM_Cmd(ENC_TIM_PORT, ENABLE);
}

void ENC_TIM_IRQHandler(void)
{
    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC1);
    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC2);
    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC3);

    u32HallCount++;
    HallEncUpdate();
}

bool HallEncUpdate(void)
{
    // 霍尔真值为5时, 表示转子d轴与定子alpha轴重合, 即电角度为0
    static RO u8 steps[] = {5, 3, 4, 1, 0, 2};

    u8 state = 0;

    state = (((u8)GPIO_ReadPin(ENC_HA_GPIO_PORT, ENC_HA_GPIO_PIN) << 2u) |
             ((u8)GPIO_ReadPin(ENC_HB_GPIO_PORT, ENC_HB_GPIO_PIN) << 1u) |
             ((u8)GPIO_ReadPin(ENC_HC_GPIO_PORT, ENC_HC_GPIO_PIN) << 0u));

    if (state == 0 || state == 7)
    {
        return false;
    }

    u8HallState = state;

#if 1  // 0~360
    u16ElecAngle = 60 * steps[state - 1];
#else  // 0~360 => 0~65535
    // 角度标幺化
    u16ElecAngle = 65535 * steps[state - 1] / 6;
#endif

    // printf("%5d,%5d,%5d\n", u16ElecAngle, u8HallState, u32HallCount);

    return true;
}

#include "foc/foc.h"
#include "math.h"
#include "board.h"
static u8 iiiii = 0;
#define voltage_power_supply 11.2
float Ta, Tb, Tc;
void  setPhaseVoltage(float Uq, float Ud, float angle_el)
{
    float    Uout;
    uint32_t sector;
    float    T0, T1, T2;

    if (Ud) /* 只设置了u */
    {
        // 平法根(3-4% 误差)
        Uout     = _sqrt_fast(Ud * Ud + Uq * Uq) / voltage_power_supply;
        // 将角度转换到【0-2PI】,使用查表法必要操作
        angle_el = _normalizeAngle(angle_el + atan2(Uq, Ud)); /* 标准化角度 */
    }
    else
    {
        // 只需要Uq-不需要平方根和反正切函数
        Uout     = Uq / voltage_power_supply;
        angle_el = _normalizeAngle(angle_el + _PI_2); /* 标准化角度 */
    }

    if (Uout > 0.577f)
    {
        Uout = 0.577f; /* 溢出检测 */
    }
    if (Uout < -0.577f)
    {
        Uout = -0.577f;
    }

    sector = (angle_el / _PI_3) + 1.0f; /* 扇区计算 */
    T1     = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uout;
    T2     = _SQRT3 * _sin(angle_el - (sector - 1.0f) * _PI_3) * Uout;
    T0     = 1.0f - T1 - T2;

    /* 根据扇区计算出需要的占空比svpwm */
    switch (sector)
    {
        case 1:
            Ta = T1 + T2 + T0 / 2;
            Tb = T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 2:
            Ta = T1 + T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 3:
            Ta = T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T2 + T0 / 2;
            break;
        case 4:
            Ta = T0 / 2;
            Tb = T1 + T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 5:
            Ta = T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 6:
            Ta = T1 + T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T0 / 2;
            break;
        default:  // possible error state
            Ta = 0;
            Tb = 0;
            Tc = 0;
    }

    f32 duty[6][3] = {
        {Ta, Tb, Tc},
        {Ta, Tc, Tb},
        {Tb, Ta, Tc},
        {Tb, Tc, Ta},
        {Tc, Ta, Tb},
        {Tc, Tb, Ta},
    };

    u8 n = 0;

    ATIM_SetCompare1A(duty[n][0] * PWM_Period);
    ATIM_SetCompare2A(duty[n][1] * PWM_Period);
    ATIM_SetCompare3A(duty[n][2] * PWM_Period);
}

#define voltage_limit 6.8  //  6.8  // V，最大值需小于12/1.732=6.9。

void MotorRun(void)
{
#if 1

    // 开环

    static f32 f32MechAngle = 0.0f;

    f32MechAngle += 0.003;

    if (f32MechAngle >= 6.28)
    {
        f32MechAngle -= 6.28;
    }

    f32 f32ElecAngle = angle_gen(f32MechAngle, MotorPolePairs);

#else

    // 霍尔闭环 (需要超前90°)
    // 电角度不对会导致出力不对

    f32 f32ElecAngle = 6.28 * (u16ElecAngle + 90) / 360;

#endif

    // 转子 q 轴控制转速
    // 转子 d 轴控制力矩

    setPhaseVoltage(voltage_limit, 0, f32ElecAngle);

    return;

    // foc

    park_t _park = {
        .sin   = sinf(f32ElecAngle),
        .cos   = cosf(f32ElecAngle),
        .theta = f32ElecAngle,
        .d     = 0,
        .q     = 5,
    };

    ipark(&_park);

    svgen_t _svpwm = {
        .Ualpha = _park.alpha,
        .Ubeta  = _park.beta,
    };

    svgen(&_svpwm);
}
