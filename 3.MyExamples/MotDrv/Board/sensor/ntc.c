#include "ntc.h"

uint16_t NTC_GetTemp(uint16_t adconv)  // unit: 0.1℃
{
    int ntc_resistance = 1000 * adconv / (4096 - adconv);

    // B = 3380K, 10k , from 0 degree to 100 degree, table, the step is 5 degree
    static const uint16_t tbl[] = {2749, 2218, 1802, 1472, 1210, 1000, 831, 694, 583, 491, 416, 354, 302, 259, 223, 193, 167, 146, 127, 111, 98};

    uint16_t min = 0, max = sizeof(tbl) / 2 - 1, mid;  // index

    if (ntc_resistance > tbl[min])
    {
        return 100.f;
    }
    else if (ntc_resistance < tbl[max])
    {
        return 0.f;
    }

    while ((max - min) > 1)  // 二分法
    {
        mid = (max + min) >> 1;
        (tbl[mid] < ntc_resistance) ? (max = mid) : (min = mid);
    }

    return 50 * min + 50 * (tbl[min] - ntc_resistance) / (tbl[min] - tbl[min + 1]);
}
