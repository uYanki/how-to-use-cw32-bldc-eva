#include "stdio.h"
#include "math.h"

#define SQRT3            1.73
#define MOTOR_POLE_PAIRS 2
#define DEG2RAD          0.017444

typedef float f32;

int main()
{
    f32 mechAngle;

    FILE* f = fopen("dat.csv", "w+");

    fprintf(f, "a,b,c\n");

    for (mechAngle = 0; mechAngle < 360; mechAngle += 1)
    {
        f32 elecAngle = fmod(DEG2RAD * mechAngle * MOTOR_POLE_PAIRS, 6.18);

        f32 Ud = 0, Uq = 5;
        f32 theta = elecAngle;

        // rev-park
        f32 Ualpha = Ud * cos(theta) - Uq * sin(theta);
        f32 Ubeta  = Ud * sin(theta) + Uq * cos(theta);

        // rev-clarke
        f32 Ua = Ualpha;
        f32 Ub = 0.5f * (SQRT3 * Ubeta - Ualpha);
        f32 Uc = -0.5f * (SQRT3 * Ubeta + Ualpha);

        fprintf(f, "%f,%f,%f\n", Ua, Ub, Uc);
    }

    fclose(f);

    return 0;
}