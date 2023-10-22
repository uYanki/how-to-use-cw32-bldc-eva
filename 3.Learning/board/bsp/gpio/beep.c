#include "beep.h"
#include "pinmap.h"

void BeepInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pins  = BEEP_GPIO_PIN,
        .Mode  = GPIO_MODE_OUTPUT_PP,
        .IT    = GPIO_IT_NONE,
        .Speed = GPIO_SPEED_HIGH,
    };

    BEEP_GPIO_CLKEN(BEEP_GPIO_CLK, ENABLE);
    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStruct);

    BeepOff();
}

void BeepOn(void)
{
    GPIO_WritePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_Pin_SET);
}

void BeepOff(void)
{
    GPIO_WritePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_Pin_RESET);
}

void BeepTgl(void)
{
    GPIO_TogglePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
}
