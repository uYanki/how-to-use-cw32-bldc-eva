#ifndef __PINMAP_H__
#define __PINMAP_H__

//
// cw32_bldc_eva
//

//-----------------------------------------------------------------------------
//

#include "cw32_fwlib.h"

//-----------------------------------------------------------------------------
//

#define LED_NBR            3u
#define KEY_NBR            3u

//-----------------------------------------------------------------------------
//

///< led (digital)

#define LED1_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define LED1_GPIO_CLK      RCC_AHB_PERIPH_GPIOC
#define LED1_GPIO_PORT     CW_GPIOC
#define LED1_GPIO_PIN      GPIO_PIN_13

#define LED2_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define LED2_GPIO_CLK      RCC_AHB_PERIPH_GPIOA
#define LED2_GPIO_PORT     CW_GPIOA
#define LED2_GPIO_PIN      GPIO_PIN_11

#define LED3_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define LED3_GPIO_CLK      RCC_AHB_PERIPH_GPIOB
#define LED3_GPIO_PORT     CW_GPIOB
#define LED3_GPIO_PIN      GPIO_PIN_11

///< key (digital)

#define KEY1_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define KEY1_GPIO_CLK      RCC_AHB_PERIPH_GPIOB
#define KEY1_GPIO_PORT     CW_GPIOB
#define KEY1_GPIO_PIN      GPIO_PIN_4

#define KEY2_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define KEY2_GPIO_CLK      RCC_AHB_PERIPH_GPIOB
#define KEY2_GPIO_PORT     CW_GPIOB
#define KEY2_GPIO_PIN      GPIO_PIN_5

#define KEY3_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define KEY3_GPIO_CLK      RCC_AHB_PERIPH_GPIOB
#define KEY3_GPIO_PORT     CW_GPIOB
#define KEY3_GPIO_PIN      GPIO_PIN_10

///< buzzer (digital)

#define BEEP_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define BEEP_GPIO_CLK      RCC_AHB_PERIPH_GPIOA
#define BEEP_GPIO_PORT     CW_GPIOA
#define BEEP_GPIO_PIN      GPIO_PIN_12

///< potentiometer (analog)

#define BEEP_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define BEEP_GPIO_CLK      RCC_AHB_PERIPH_GPIOA
#define BEEP_GPIO_PORT     CW_GPIOA
#define BEEP_GPIO_PIN      GPIO_PIN_12

///< ntc thermistor (analog)

#define POT_GPIO_CLKEN     RCC_AHBPeriphClk_Enable
#define POT_GPIO_CLK       RCC_AHB_PERIPH_GPIOB
#define POT_GPIO_PORT      CW_GPIOB
#define POT_GPIO_PIN       GPIO_PIN_0
#define POT_GPIO_AF        PB00_ANALOG_ENABLE

#define NTC_GPIO_CLKEN     RCC_AHBPeriphClk_Enable
#define NTC_GPIO_CLK       RCC_AHB_PERIPH_GPIOA
#define NTC_GPIO_PORT      CW_GPIOA
#define NTC_GPIO_PIN       GPIO_PIN_4
#define NTC_GPIO_AF        PA04_ANALOG_ENABLE

///< uart (dbg,ble)

#define UART_CLKEN         RCC_APBPeriphClk_Enable2
#define UART_CLK           RCC_APB2_PERIPH_UART1
#define UART_PORT          CW_UART1

#define UART_TX_GPIO_CLKEN RCC_AHBPeriphClk_Enable
#define UART_TX_GPIO_CLK   RCC_AHB_PERIPH_GPIOB
#define UART_TX_GPIO_PORT  CW_GPIOB
#define UART_TX_GPIO_PIN   GPIO_PIN_8
#define UART_TX_GPIO_AF    PB08_AFx_UART1TXD

#define UART_RX_GPIO_CLKEN RCC_AHBPeriphClk_Enable
#define UART_RX_GPIO_CLK   RCC_AHB_PERIPH_GPIOB
#define UART_RX_GPIO_PORT  CW_GPIOB
#define UART_RX_GPIO_PIN   GPIO_PIN_9
#define UART_RX_GPIO_AF    PB09_AFx_UART1RXD

///< i2c (oled)

#define I2C_SCL_GPIO_CLKEN RCC_AHBPeriphClk_Enable
#define I2C_SCL_GPIO_CLK   RCC_AHB_PERIPH_GPIOB
#define I2C_SCL_GPIO_PORT  CW_GPIOB
#define I2C_SCL_GPIO_PIN   GPIO_PIN_6

#define I2C_SDA_GPIO_CLKEN RCC_AHBPeriphClk_Enable
#define I2C_SDA_GPIO_CLK   RCC_AHB_PERIPH_GPIOB
#define I2C_SDA_GPIO_PORT  CW_GPIOB
#define I2C_SDA_GPIO_PIN   GPIO_PIN_7

#define I2C_CLK
#define I2C_PORT

///< pwm (motor)

#define MOTOR_WH_GPIO_CLKEN  RCC_AHBPeriphClk_Enable
#define MOTOR_WH_GPIO_CLK    RCC_AHB_PERIPH_GPIOA
#define MOTOR_WH_GPIO_PORT   CW_GPIOA
#define MOTOR_WH_GPIO_PIN    GPIO_PIN_10

#define MOTOR_VH_GPIO_CLKEN  RCC_AHBPeriphClk_Enable
#define MOTOR_VH_GPIO_CLK    RCC_AHB_PERIPH_GPIOA
#define MOTOR_VH_GPIO_PORT   CW_GPIOA
#define MOTOR_VH_GPIO_PIN    GPIO_PIN_9

#define MOTOR_UH_GPIO_CLKEN  RCC_AHBPeriphClk_Enable
#define MOTOR_UH_GPIO_CLK    RCC_AHB_PERIPH_GPIOA
#define MOTOR_UH_GPIO_PORT   CW_GPIOA
#define MOTOR_UH_GPIO_PIN    GPIO_PIN_8

#define MOTOR_WL_GPIO_CLKEN  RCC_AHBPeriphClk_Enable
#define MOTOR_WL_GPIO_CLK    RCC_AHB_PERIPH_GPIOB
#define MOTOR_WL_GPIO_PORT   CW_GPIOB
#define MOTOR_WL_GPIO_PIN    GPIO_PIN_15

#define MOTOR_VL_GPIO_CLKEN  RCC_AHBPeriphClk_Enable
#define MOTOR_VL_GPIO_CLK    RCC_AHB_PERIPH_GPIOB
#define MOTOR_VL_GPIO_PORT   CW_GPIOB
#define MOTOR_VL_GPIO_PIN    GPIO_PIN_14

#define MOTOR_UL_GPIO_CLKEN  RCC_AHBPeriphClk_Enable
#define MOTOR_UL_GPIO_CLK    RCC_AHB_PERIPH_GPIOB
#define MOTOR_UL_GPIO_PORT   CW_GPIOB
#define MOTOR_UL_GPIO_PIN    GPIO_PIN_13

#define MOTOR_BKIN_GPIO_CEN  RCC_AHBPeriphClk_Enable
#define MOTOR_BKIN_GPIO_CLK  RCC_AHB_PERIPH_GPIOB
#define MOTOR_BKIN_GPIO_PORT CW_GPIOB
#define MOTOR_BKIN_GPIO_PIN  GPIO_PIN_12

// current feedback (analog)

#define MOTOR_UE_GPIO_CLKEN  RCC_AHBPeriphClk_Enable
#define MOTOR_UE_GPIO_CLK    RCC_AHB_PERIPH_GPIOA
#define MOTOR_UE_GPIO_PORT   CW_GPIOA
#define MOTOR_UE_GPIO_PIN    GPIO_PIN_0
#define MOTOR_UE_GPIO_AF     PA00_ANALOG_ENABLE

#define MOTOR_VE_GPIO_CLKEN  RCC_AHBPeriphClk_Enable
#define MOTOR_VE_GPIO_CLK    RCC_AHB_PERIPH_GPIOA
#define MOTOR_VE_GPIO_PORT   CW_GPIOA
#define MOTOR_VE_GPIO_PIN    GPIO_PIN_1
#define MOTOR_VE_GPIO_AF     PA01_ANALOG_ENABLE

// encoder (tim)

#define ENC_HA_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define ENC_HA_GPIO_CLK      RCC_AHB_PERIPH_GPIOA
#define ENC_HA_GPIO_PORT     CW_GPIOA
#define ENC_HA_GPIO_PIN      GPIO_PIN_15
#define ENC_HA_GPIO_AF       PA15_AFx_GTIM2CH1

#define ENC_HB_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define ENC_HB_GPIO_CLK      RCC_AHB_PERIPH_GPIOB
#define ENC_HB_GPIO_PORT     CW_GPIOB
#define ENC_HB_GPIO_PIN      GPIO_PIN_3
#define ENC_HB_GPIO_AF       PB03_AFx_GTIM2CH2

#define ENC_HC_GPIO_CLKEN    RCC_AHBPeriphClk_Enable
#define ENC_HC_GPIO_CLK      RCC_AHB_PERIPH_GPIOA
#define ENC_HC_GPIO_PORT     CW_GPIOA
#define ENC_HC_GPIO_PIN      GPIO_PIN_2
#define ENC_HC_GPIO_AF       PA02_AFx_GTIM2CH3

#define ENC_TIM_CLKEN        RCC_APBPeriphClk_Enable1
#define ENC_TIM_CLK          RCC_APB1_PERIPH_GTIM2
#define ENC_TIM_PORT         CW_GTIM2

#define ENC_TIM_IRQn         GTIM2_IRQn
#define ENC_TIM_IRQHandler   GTIM2_IRQHandler

#endif
