#ifndef __PID_H__
#define __PID_H__

#include "usdk.h"

typedef struct {
    f32 Kp;  //!< proportional gain
    f32 Ki;  //!< integral gain
    f32 Kd;  //!< derivative gain

    f32 err;   //!< error
    f32 prop;  //!< proportional value
    f32 inte;  //!< integral value
    f32 deri;  //!< derivative value

    f32 ref;  //!< reference input value
    f32 ffw;  //!< feedforward input value
    f32 fbk;  //!< feedback input value

    f32 ramp;  //!< ramp limit
    f32 lo;    //!< lower saturation limit
    f32 hi;    //!< upper saturation limit
    f32 out;   //!< output value

    f32 Ts;

    void* args;
} PID_t;

typedef void (*PID_Handler_t)(PID_t*);

#endif
