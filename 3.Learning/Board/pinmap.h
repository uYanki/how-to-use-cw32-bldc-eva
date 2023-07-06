#ifndef __PIN_MAP_H__
#define __PIN_MAP_H__

// gpio

#define PIN_LED1 CW_GPIOC, GPIO_PIN_13
#define PIN_LED2 CW_GPIOA, GPIO_PIN_11
#define PIN_LED3 CW_GPIOB, GPIO_PIN_11

#define PIN_KEY1 CW_GPIOB, GPIO_PIN_4
#define PIN_KEY2 CW_GPIOB, GPIO_PIN_5
#define PIN_KEY3 CW_GPIOB, GPIO_PIN_10

#define PIN_POT  CW_GPIOB, GPIO_PIN_0   // potentiometer

#define PIN_BUZ  CW_GPIOA, GPIO_PIN_12  // buzzer

// uart

#define BLE_TX   CW_GPIOB, GPIO_PIN_8
#define BLE_RX   CW_GPIOB, GPIO_PIN_9

// i2c

#define OLED_SCL CW_GPIOB, GPIO_PIN_6
#define OLED_SDA CW_GPIOB, GPIO_PIN_7

//

#define FOC_WH   CW_GPIOA, GPIO_PIN_10
#define FOC_VH   CW_GPIOA, GPIO_PIN_9
#define FOC_UH   CW_GPIOA, GPIO_PIN_8

#define FOC_WL   CW_GPIOB, GPIO_PIN_15
#define FOC_VL   CW_GPIOB, GPIO_PIN_14
#define FOC_UL   CW_GPIOB, GPIO_PIN_13

#define FOC_Ue   CW_GPIOA, GPIO_PIN_0
#define FOC_Ve   CW_GPIOA, GPIO_PIN_1
#define FOC_HC   CW_GPIOA, GPIO_PIN_2

#define FOC_BKIN CW_GPIOB, GPIO_PIN_12

#endif  // __PIN_MAP_H__
