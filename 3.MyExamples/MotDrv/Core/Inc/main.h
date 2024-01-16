/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TIM_HALL htim2
#define UART_MODBUS husart1
#define PWM_PERIOD 4000
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC
#define MOT_UE_Pin GPIO_PIN_0
#define MOT_UE_GPIO_Port GPIOA
#define MOT_VE_Pin GPIO_PIN_1
#define MOT_VE_GPIO_Port GPIOA
#define ENC_HALL_C_Pin GPIO_PIN_2
#define ENC_HALL_C_GPIO_Port GPIOA
#define AD_VBUS_Pin GPIO_PIN_3
#define AD_VBUS_GPIO_Port GPIOA
#define AD_NTC_Pin GPIO_PIN_4
#define AD_NTC_GPIO_Port GPIOA
#define MOT_WE_Pin GPIO_PIN_5
#define MOT_WE_GPIO_Port GPIOA
#define AD_IBIS_Pin GPIO_PIN_6
#define AD_IBIS_GPIO_Port GPIOA
#define AD_ICIV_Pin GPIO_PIN_7
#define AD_ICIV_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_10
#define KEY3_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_11
#define LED3_GPIO_Port GPIOB
#define MOT_UL_Pin GPIO_PIN_13
#define MOT_UL_GPIO_Port GPIOB
#define MOT_VL_Pin GPIO_PIN_14
#define MOT_VL_GPIO_Port GPIOB
#define MOT_WL_Pin GPIO_PIN_15
#define MOT_WL_GPIO_Port GPIOB
#define MOT_UH_Pin GPIO_PIN_8
#define MOT_UH_GPIO_Port GPIOA
#define MOT_VH_Pin GPIO_PIN_9
#define MOT_VH_GPIO_Port GPIOA
#define MOT_WH_Pin GPIO_PIN_10
#define MOT_WH_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_11
#define LED2_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_12
#define BUZZER_GPIO_Port GPIOA
#define ENC_HALL_A_Pin GPIO_PIN_15
#define ENC_HALL_A_GPIO_Port GPIOA
#define ENC_HALL_B_Pin GPIO_PIN_3
#define ENC_HALL_B_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_4
#define KEY1_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_5
#define KEY2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */