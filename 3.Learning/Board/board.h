#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_conf.h"
#include "pinmap.h"

#include "sys/sleep.h"

#include "bsp/clk/sysclk.h"

#include "bsp/uart/uart.h"
// #include "bsp/uart/modbus.h"

#include "bsp/gpio/led.h"
#include "bsp/gpio/key.h"
#include "bsp/gpio/beep.h"

#include "bsp/adc/ntc.h"
#include "bsp/adc/pot.h"
#include "bsp/adc/adc.h"

#include "bsp/tim/hallEnc.h"
#include "bsp/tim/pwm.h"

#include "bsp/i2c/oled/oled_i2c.h"

void BoardInit(void);

#endif
