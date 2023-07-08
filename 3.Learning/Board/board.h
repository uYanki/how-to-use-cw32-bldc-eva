#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_conf.h"

#if CONFIG_REDIRECT_PRINTF
#include <stdio.h>
#endif

#include <stdbool.h>

#define println(fmt, args...) printf(fmt "\n", ##args)

/* fwlib */

#include "base_types.h"
#include "cw32f030.h"
#include "system_cw32f030.h"
#include "interrupts_cw32f030.h"

#include "cw32f030_adc.h"
#include "cw32f030_atim.h"
#include "cw32f030_awt.h"
#include "cw32f030_btim.h"
#include "cw32f030_crc.h"
#include "cw32f030_debug.h"
#include "cw32f030_digitalsign.h"
#include "cw32f030_dma.h"
#include "cw32f030_flash.h"
#include "cw32f030_gpio.h"
#include "cw32f030_gtim.h"
#include "cw32f030_i2c.h"
#include "cw32f030_iwdt.h"
#include "cw32f030_lvd.h"
#include "cw32f030_pwr.h"
#include "cw32f030_ram.h"
#include "cw32f030_rcc.h"
#include "cw32f030_rtc.h"
#include "cw32f030_spi.h"
#include "cw32f030_systick.h"
#include "cw32f030_uart.h"
#include "cw32f030_wwdt.h"

/* pinmap */

// gpio

#define PIN_LED1 CW_GPIOC, GPIO_PIN_13
#define PIN_LED2 CW_GPIOA, GPIO_PIN_11
#define PIN_LED3 CW_GPIOB, GPIO_PIN_11

#define PIN_KEY1 CW_GPIOB, GPIO_PIN_4
#define PIN_KEY2 CW_GPIOB, GPIO_PIN_5
#define PIN_KEY3 CW_GPIOB, GPIO_PIN_10

#define PIN_BUZ  CW_GPIOA, GPIO_PIN_12  // buzzer

#define PIN_POT  CW_GPIOB, GPIO_PIN_0   // potentiometer
#define PIN_NTC  CW_GPIOA, GPIO_PIN_4   // ntc thermistor

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

/* func */

void board_init(void);

#endif  // __BOARD_H__
