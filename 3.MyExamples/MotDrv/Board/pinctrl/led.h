#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "main.h"

#define LED1_ON()  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define LED1_OFF() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED1_TGL() HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)

#define LED2_ON()  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
#define LED2_OFF() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_TGL() HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin)

#define LED3_ON()  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET)
#define LED3_OFF() HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET)
#define LED3_TGL() HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin)

#endif
