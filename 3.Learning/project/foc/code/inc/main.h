#ifndef __MAIN_H__
#define __MAIN_H__

#include "board.h"

#include "math.h"

///< @note R = resolution
#define ADC_12R_3V3(val) ((f32)(val) / (f32)BIT12 * 3.3f)
#define ADC_12R_5V(val)  ((f32)(val) / (f32)BIT16 * 5.0f)
#define ADC_16R_3V3(val) ((f32)(val) / (f32)BIT12 * 3.3f)
#define ADC_16R_5V(val)  ((f32)(val) / (f32)BIT16 * 5.0f)

typedef struct {
    u16 Ue, Ve, We;  // U,V,W 三相反电动势
    u16 Vbus;        // 母线电压
    u16 Ib, Ic;      // 电流
    u16 Vpot;
    u16 ntc;         // 热敏电阻
} adc_result_t;

#endif  // !__MAIN_H__
