#include "hallEnc.h"
#include "pinmap.h"

// https://blog.csdn.net/csol1607408930/article/details/129410150

vu16 u16HallAngle = 0;
vu32 u32HallCount = 0;

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

    u16HallAngle = HallEncGetAngle();

    GTIM_Cmd(ENC_TIM_PORT, ENABLE);
}

void ENC_TIM_IRQHandler(void)
{
    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC1);
    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC2);
    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC3);

    u32HallCount++;  // 用于测速
    u16HallAngle = HallEncGetAngle();

    // printf("%d\n", HallEncGetAngle());
}

u16 HallEncGetAngle(void)
{
    static RO u16 steps[6] = {1, 3, 2, 5, 0, 4};

    u8 code = 0;

    code = (((u8)GPIO_ReadPin(ENC_HA_GPIO_PORT, ENC_HA_GPIO_PIN) << 0u) |
            ((u8)GPIO_ReadPin(ENC_HB_GPIO_PORT, ENC_HB_GPIO_PIN) << 1u) |
            ((u8)GPIO_ReadPin(ENC_HC_GPIO_PORT, ENC_HC_GPIO_PIN) << 2u));

    if (code == 0 || code == 7)
    {
        return U16_MAX;  // TBD
    }

#if 1  // 0~360
    return 60 * steps[code - 1];
#else  // 0~65535
    return 65535 * steps[code - 1] / 6;
    // 角度标幺化
    // 0 -> 0
    // 30 -> 5461
    // 60 -> 10922
    // 90 -> 16384
    // 120 -> 21845
    // 150 -> 27306
    // 180 -> 32767
    // 210 -> 38229
    // 240 -> 43690
    // 260 -> 49152
    // 300 -> 54612
    // 330 -> 60075
#endif
}
