#include "hallEnc.h"
#include "pinmap.h"

void HallEncInit(void)
{
    // GPIO
    {
        GPIO_InitTypeDef GPIO_InitStruct = {
            .Mode  = GPIO_MODE_OUTPUT_PP,
            .IT    = GPIO_IT_NONE,
            .Speed = GPIO_SPEED_HIGH,
        };

        ENC_HA_GPIO_CLKEN(ENC_HA_GPIO_CLK, ENABLE);
        ENC_HB_GPIO_CLKEN(ENC_HB_GPIO_CLK, ENABLE);
        ENC_HC_GPIO_CLKEN(ENC_HC_GPIO_CLK, ENABLE);

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

        ENC_HC_GPIO_CLKEN(ENC_HC_GPIO_CLK, ENABLE);

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

    GTIM_Cmd(ENC_TIM_PORT, ENABLE);
}

void ENC_TIM_IRQHandler(void)
{
    static RO u16 steps[6] = {1, 3, 2, 5, 0, 4};

    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC1);
    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC2);
    GTIM_ClearITPendingBit(ENC_TIM_PORT, GTIM_IT_CC3);

    //    ++nHallTotalCnt;

    printf("%d,%d,%d,%d,", ENC_TIM_PORT->CNT, ENC_TIM_PORT->CCR1, ENC_TIM_PORT->CCR2, ENC_TIM_PORT->CCR3)

        u8 x = 0;

    if (PA15_GETVALUE())
    {
        x |= 0x1;
    }
    if (PB03_GETVALUE())
    {
        x |= 0x2;
    }
    if (PA02_GETVALUE())
    {
        x |= 0x4;
    }

    // steps[x - 1]
    if (x > 0 && x < 7)
    {
        // nCurDeg = steps[x - 1] * 60;
        printf("%d", steps[x - 1] * 60);
    }

    printf("\n");
}
