#include "pot.h"
#include "pinmap.h"

void PotInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pins  = POT_GPIO_PIN,
        .Mode  = GPIO_MODE_ANALOG,
        .Speed = GPIO_SPEED_HIGH,
    };

    POT_GPIO_CLKEN(POT_GPIO_CLK, ENABLE);
    GPIO_Init(POT_GPIO_PORT, &GPIO_InitStruct);
}

f32 PotConv(u16 advalue)
{
    return 3.3f * (f32)advalue / 4096.f;
}
