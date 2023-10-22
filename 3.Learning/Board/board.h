#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_conf.h"

#include "sys/sleep.h"

#include "bsp/uart/uart.h"
#include "bsp/gpio/led.h"
#include "bsp/gpio/key.h"
#include "bsp/gpio/beep.h"
#include "bsp/adc/ntc.h"
#include "bsp/clk/sysclk.h"

void BoardInit(void);

#endif
