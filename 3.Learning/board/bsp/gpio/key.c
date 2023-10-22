#include "key.h"
#include "pinmap.h"

void KeyInit(void)
{
      GPIO_InitTypeDef GPIO_InitStruct = {
        .Mode  = GPIO_MODE_INPUT_PULLUP,
        .IT    = GPIO_IT_NONE,
        .Speed = GPIO_SPEED_HIGH,
    };

#if KEY_NBR >= 1u
    GPIO_InitStruct.Pins = KEY1_GPIO_PIN;  // KEY1
    KEY1_GPIO_CLKEN(KEY1_GPIO_CLK, ENABLE);
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStruct);
#endif
#if KEY_NBR >= 2u
    GPIO_InitStruct.Pins = KEY2_GPIO_PIN;  // KEY2
    KEY2_GPIO_CLKEN(KEY2_GPIO_CLK, ENABLE);
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStruct);
#endif
#if KEY_NBR >= 3u
    GPIO_InitStruct.Pins = KEY3_GPIO_PIN;  // KEY3
    KEY3_GPIO_CLKEN(KEY3_GPIO_CLK, ENABLE);
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStruct);
#endif
}

bool KeyIsPress(u8 idx)
{
    switch (idx)
    {
#if KEY_NBR >= 1u
        case KEY1: return GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_Pin_RESET;
#endif
#if KEY_NBR >= 2u
        case KEY2: return GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == GPIO_Pin_RESET;
#endif
#if KEY_NBR >= 3u
        case KEY3: return GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == GPIO_Pin_RESET;
#endif
        default: return false;
    }
}

bool KeyIsRelease(u8 idx)
{
    switch (idx)
    {
#if KEY_NBR >= 1u
        case KEY1: return GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_Pin_SET;
#endif
#if KEY_NBR >= 2u
        case KEY2: return GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == GPIO_Pin_SET;
#endif
#if KEY_NBR >= 3u
        case KEY3: return GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == GPIO_Pin_SET;
#endif
        default: return false;
    }
}
