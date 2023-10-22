#include "ntc.h"
#include "pinmap.h"

void NtcInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pins  = NTC_GPIO_PIN,
        .Mode  = GPIO_MODE_ANALOG,
        .Speed = GPIO_SPEED_HIGH,
    };

    NTC_GPIO_CLKEN(NTC_GPIO_CLK, ENABLE);
    GPIO_Init(NTC_GPIO_PORT, &GPIO_InitStruct);
}

f32 NtcConv(u16 advalue)
{
    static RO u16 tbl[98] = {28017, 26826, 25697, 24629, 23618, 22660, 21752, 20892, 20075, 19299, 18560, 18482, 18149, 17632, 16992, 16280, 15535, 14787, 14055, 13354, 12690, 12068, 11490, 10954, 10458, 10000, 9576, 9184, 8819, 8478, 8160, 7861, 7579, 7311, 7056, 6813, 6581, 6357, 6142, 5934, 5734, 5541, 5353, 5173, 4998, 4829, 4665, 4507, 4355, 4208, 4065, 3927, 3794, 3664, 3538, 3415, 3294, 3175, 3058, 2941, 2825, 2776, 2718, 2652, 2582, 2508, 2432, 2356, 2280, 2207, 2135, 2066, 2000, 1938, 1879, 1823, 1770, 1720, 1673, 1628, 1586, 1546, 1508, 1471, 1435, 1401, 1367, 1334, 1301, 1268, 1236, 1204, 1171, 1139, 1107, 1074, 1042, 1010};

    u16 min = 0, max = ARRAY_SIZE(tbl) - 1, mid;  // index

#if 0
    f32 mv  = advalue / 4095.f * 3300.f;
    f32 cmp = mv * 10 / (5 - mv / 1000);
#else
    // 上式的简化
    u16 cmp = 100000000 / (62060606 / advalue - 10000);
#endif

    while ((max - min) > 1)
    {
        // 二分法查表
        mid = (max + min) >> 1;
        (tbl[mid] < cmp) ? (max = mid) : (min = mid);
    }

    f32 ret = min;

    if (max != min)
    {
        ret += (f32)(tbl[min] - cmp) / (f32)(tbl[min] - tbl[max]);
    }

    return ret;
}
