#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "main.h"

#define KEY1_IS_PRESS() (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
#define KEY2_IS_PRESS() (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
#define KEY3_IS_PRESS() (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)

#endif
