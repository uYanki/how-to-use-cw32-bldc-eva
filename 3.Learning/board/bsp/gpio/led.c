#include "led.h"
#include "pinmap.h"

void LedInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Mode  = GPIO_MODE_OUTPUT_PP,
        .IT    = GPIO_IT_NONE,
        .Speed = GPIO_SPEED_HIGH,
    };

#if LED_NBR >= 1u
    GPIO_InitStruct.Pins = LED1_GPIO_PIN;  // LED1
    LED1_GPIO_CLKEN(LED1_GPIO_CLK, ENABLE);
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
    LedOff(LED1);
#endif
#if LED_NBR >= 2u
    GPIO_InitStruct.Pins = LED2_GPIO_PIN;  // LED2
    LED2_GPIO_CLKEN(LED2_GPIO_CLK, ENABLE);
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);
    LedOff(LED2);
#endif
#if LED_NBR >= 3u
    GPIO_InitStruct.Pins = LED3_GPIO_PIN;  // LED3
    LED3_GPIO_CLKEN(LED3_GPIO_CLK, ENABLE);
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);
    LedOff(LED3);
#endif
}

void LedOn(u8 idx)
{
    switch (idx)
    {
#if LED_NBR >= 1u
        case LED1: GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_Pin_RESET); break;
#endif
#if LED_NBR >= 2u
        case LED2: GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_Pin_RESET); break;
#endif
#if LED_NBR >= 3u
        case LED3: GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_Pin_RESET); break;
#endif
        default: break;
    }
}

void LedOff(u8 idx)
{
    switch (idx)
    {
#if LED_NBR >= 1u
        case LED1: GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_Pin_SET); break;
#endif
#if LED_NBR >= 2u
        case LED2: GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_Pin_SET); break;
#endif
#if LED_NBR >= 3u
        case LED3: GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_Pin_SET); break;
#endif
        default: break;
    }
}

void LedTgl(u8 idx)
{
    switch (idx)
    {
#if LED_NBR >= 1u
        case LED1: GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN); break;
#endif
#if LED_NBR >= 2u
        case LED2: GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN); break;
#endif
#if LED_NBR >= 3u
        case LED3: GPIO_TogglePin(LED3_GPIO_PORT, LED3_GPIO_PIN); break;
#endif
        default: break;
    }
}
