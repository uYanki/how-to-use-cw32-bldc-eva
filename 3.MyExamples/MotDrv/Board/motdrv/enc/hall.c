#include "hall.h"
#include "defs.h"

// ST HAL 霍尔模式配置 https://zhuanlan.zhihu.com/p/126478231
// 霍尔位置的对应角度 https://zhuanlan.zhihu.com/p/462932301
// 霍尔传感器信号超前于转子π/6，因此，电角度 = 霍尔角度 - π/6。正向旋转时

void HallEnc_ReadState(hall_encoder_t* pEnc)
{
    uint8_t HallStateCur = 0, HallStatePrev;

    do
    {
        HallStatePrev = HallStateCur;
        HallStateCur  = 0;

        switch (pEnc->Placement)
        {
            case 60:  // DEGREES_60
            {
                HallStateCur |= ((HAL_GPIO_ReadPin(pEnc->HB_Port, pEnc->HB_Pin) == GPIO_PIN_SET) ^ 1) << 2;
                HallStateCur |= (HAL_GPIO_ReadPin(pEnc->HC_Port, pEnc->HC_Pin) == GPIO_PIN_SET) << 1;
                HallStateCur |= (HAL_GPIO_ReadPin(pEnc->HA_Port, pEnc->HA_Pin) == GPIO_PIN_SET) << 0;
                break;
            }
            case 120:  // DEGREES_120
            {
                HallStateCur |= (HAL_GPIO_ReadPin(pEnc->HC_Port, pEnc->HC_Pin) == GPIO_PIN_SET) << 2;
                HallStateCur |= (HAL_GPIO_ReadPin(pEnc->HB_Port, pEnc->HB_Pin) == GPIO_PIN_SET) << 1;
                HallStateCur |= (HAL_GPIO_ReadPin(pEnc->HA_Port, pEnc->HA_Pin) == GPIO_PIN_SET) << 0;
                break;
            }
        }

    } while (HallStateCur != HallStatePrev);

    pEnc->HallState = HallStateCur;
}

void HallEnc_Init(hall_encoder_t* pEnc)
{
    HallEnc_ReadState(pEnc);

    // order: 513264 or 546231

    switch (pEnc->HallState)
    {
        case HALL_STATE_5:
            pEnc->ElecAngle = HALL_ANGLE_30 * 1;  // 30
            break;
        case HALL_STATE_1:
            pEnc->ElecAngle = HALL_ANGLE_30 * 3;  // 90
            break;
        case HALL_STATE_3:
            pEnc->ElecAngle = HALL_ANGLE_30 * 5;  // 150
            break;
        case HALL_STATE_2:
            pEnc->ElecAngle = HALL_ANGLE_30 * 7;  // 210
            break;
        case HALL_STATE_6:
            pEnc->ElecAngle = HALL_ANGLE_30 * 9;  // 270
            break;
        case HALL_STATE_4:
            pEnc->ElecAngle = HALL_ANGLE_30 * 11;  // 330
            break;
        default:
            pEnc->error++;
            break;
    }
}

dir_e HallEnc_GetDir(uint8_t HallStatePrev, uint8_t HallStateCur)
{
    // order: 513264 or 546231

    static const uint8_t pTblCCW[] = {0x62, 0x23, 0x31, 0x15, 0x54, 0x46, 0x63, 0x21, 0x35, 0x14, 0x56, 0x42};
    static const uint8_t pTblCW[]  = {0x45, 0x51, 0x13, 0x32, 0x26, 0x64, 0x41, 0x53, 0x12, 0x36, 0x24, 0x65};

    uint8_t match = ((HallStatePrev & 0x0F) << 4) | (HallStateCur & 0x0F);

    for (uint8_t i = 0; i < ARRAY_SIZE(pTblCW); ++i)
    {
        if (match == pTblCW[i])
        {
            return DIR_FWD;
        }
    }

    for (uint8_t i = 0; i < ARRAY_SIZE(pTblCCW); ++i)
    {
        if (match == pTblCCW[i])
        {
            return DIR_BACK;
        }
    }

    return DIR_NONE;
}

void HallEnc_Update(hall_encoder_t* pEnc)
{
    uint8_t HallStatePrev = pEnc->HallState;
    uint8_t DirectionPrev = pEnc->Direction;

    HallEnc_Init(pEnc);

    if (pEnc->HallState != HallStatePrev)
    {
        pEnc->EdgeCount++;
        pEnc->Direction = HallEnc_GetDir(HallStatePrev, pEnc->HallState);
    }
}
