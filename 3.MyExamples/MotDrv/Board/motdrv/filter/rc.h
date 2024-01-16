#ifndef __MOTDRV_RC_FILTER_H__
#define __MOTDRV_RC_FILTER_H__

#include "defs.h"

// Sr(sampling rate, times/s),f- cutoff freq (Hz), Pi = 3.14...
// https://blog.csdn.net/qq_34040067/article/details/114048308

typedef struct {
    float32_t coeff;  // k=(2*Pi*f)/Sr
    float32_t last_val;
} rc_filter_t;

float32_t rc_lpf(rc_filter_t* lpf, float32_t new_val);
float32_t rc_hpf(rc_filter_t* hpf, float32_t new_val);

#endif
