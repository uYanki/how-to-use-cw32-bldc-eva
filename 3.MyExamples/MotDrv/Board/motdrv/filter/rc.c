#include "rc.h"

float32_t rc_lpf(rc_filter_t* lpf, float32_t new_val)
{
    lpf->last_val = (float32_t)new_val * lpf->coeff + lpf->last_val * (1 - lpf->coeff);
    return lpf->last_val;
}

float32_t rc_hpf(rc_filter_t* hpf, float32_t new_val)
{
    hpf->last_val = (float32_t)new_val * hpf->coeff + hpf->last_val * (1 - hpf->coeff);

    return -(new_val - hpf->last_val);
}
