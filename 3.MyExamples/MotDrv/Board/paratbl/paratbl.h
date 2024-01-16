#ifndef __PARATBL_H__
#define __PARATBL_H__

#include "defs.h"

extern uint8_t u8ParaBuf[];
extern uint8_t u8DbgBuf[];

// 直轴电流Ids Ids：用于产生磁场，与转子的磁场叠加
// 交轴电流Iqs Iqs：：用于控制转矩（其作用等同于直流电机的电枢电流）。
//  iqs=Is ; ids=0时，转矩达到最大值

#define ParaTbl (*(ParaTable_t*)u8ParaBuf)
#define DbgTbl  (*(DebugTable_t*)u8DbgBuf)

typedef enum {
    FOC_MODE_MCU,    // PWM_DUTY
    FOC_MODE_LOGIC,  // VQ VD
} foc_mode_e;

typedef struct __packed {
    s64 s64Buf[16];  ///< P0000
    s32 s32Buf[16];  ///< P0064
    s16 s16Buf[16];  ///< P0096
    f32 f32Buf[16];  ///< P0112
} DebugTable_t;

typedef struct __packed {
    u16 u16RunState;  ///< P0000
    s16 s16CurPhAFb;  ///< P0001 A相反馈电流 mA
    s16 s16CurPhBFb;  ///< P0002 B相反馈电流 mA
    s16 s16CurPhCFb;  ///< P0003 C相反馈电流 mA
    s16 s16IdFb;      ///< P0004 D轴反馈电流
    s16 s16IqFb;      ///< P0005 Q轴反馈电流
    s16 s16VdRef;     ///< P0006 D轴电压指令
    s16 s16VqRef;     ///< P0007 Q轴电压指令

    u16 u16EncType;     ///< P0008 编码器类型
    u32 u32EncRes;      ///< P0009 编码器分辨率
    s32 u32EncOffset;   ///< P0011 编码器偏置
    s64 s64EncPosInit;  ///< P0013 编码器上电位置
    s32 s32EncPos;      ///< P0017 编码器单圈位置
    s32 u32EncTurns;    ///< P0019 编码器圈数

    u16 u16HallAngFb;    ///< P0021 霍尔反馈角度
    u16 u16ElecAngRef;   ///< P0022 电角度指令
    s16 s16SpdDigRef;    ///< P0023 数字速度指令
    s16 s16SpdFb;        ///< P0024 速度反馈 RPM
    u16 u16ElecAngInc;   ///< P0025 开环电角度自增量
    u16 u16WaitTimeInc;  ///< P0026 开环电角度自增间隔 (us)

    s16 s16SpdKp;  ///< P0027 速度环增益系数 0.001
    s16 s16SpdKi;  ///< P0028 速度环积分系数 0.001
    s16 s16PosKp;  ///< P0029 位置环增益系数 0.001
    s16 s16PosKi;  ///< P0030 位置环积分系数 0.001
    s16 s16TrqKp;  ///< P0031 力矩环增益系数 0.001
    s16 s16TrqKi;  ///< P0032 力矩环积分系数 0.001

    u16 u16ElecOffset;  ///< P0033 电角度偏置

    s64 s64PosFb;  ///< P0034 位置反馈 Pulse
    s16 s16TrqFb;  ///< P0038 转矩反馈

    u32 u32DrvScheme;  ///< P0039 驱动器方案

    u32 u32SwBuildDate;  ///< P0041 软件构建日期
    u32 u32SwBuildTime;  ///< P0043 软件构建时间
    u32 u32McuSwVer;     ///< P0045

    u16 u16CtrlMode;  ///< P0047
    u16 u16FocMode;   ///< P0048

    /**
     * @brief motor infos
     */
    u16 u16MotType;       ///< P0049 电机类型
    u16 u16MotPolePairs;  ///< P0050

    u16 u16AccMax;     ///< P0051 最大加速度
    u32 u32InertiaWL;  ///< P0052 转动惯量
    u16 u16StatorRes;  ///< P0054 定子电阻
    u16 u16StatorLd;   ///< P0055 定子D轴电感
    u16 u16StatorLq;   ///< P0056 定子Q轴电感

    u16 u16CurRate;   ///< P0057 额定电流
    u16 u16CurMax;    ///< P0058 最大电流
    u16 u16TrqRate;   ///< P0059 额定转矩
    u16 u16TrqMax;    ///< P0060 最大转矩
    u16 u16SpdRate;   ///< P0061 额定转速
    u16 u16SpdMax;    ///< P0062 最大转速
    u16 u16EmfCoeff;  ///< P0063 反电动势常数
    u16 u16TrqCoeff;  ///< P0064 转矩系数
    u16 u16Tm;        ///< P0065 机械时间常数

    /**
     * @brief encoder infos
     */
    u16 u16HallState;     ///< P0066 霍尔真值
    s16 s16MechOffset;    ///< P0067 机械位置偏置
    u32 u32ElecGearNum;   ///< P0068 电子齿轮比分子
    u32 u32ElecGearDeno;  ///< P0070 电子齿轮比分母

    u16 u16ElecAngSrc;  ///< P0072 电角度来源

    u16 u16EncTurnClr;  ///< P0073 清除编码器多圈值命令

    u16 u16AppSel;  ///< P0074 应用选择

    /**
     * @brief foc infos
     */

    s16 s16IalphaFb;  ///< P0075 Alpha轴反馈电流
    s16 s16IbetaFb;   ///< P0076 Beta轴反馈电流

    s16 s16ValphaRef;  ///< P0077 Alpha轴电压指令
    s16 s16VbetaRef;   ///< P0078 Beta轴电压指令
    s16 s16VphaRef;    ///< P0079 A相电压指令
    s16 s16VphbRef;    ///< P0080 B相电压指令
    s16 s16VphcRef;    ///< P0081 C相电压指令
    u16 u16DutyPha;    ///< P0082 A相占空比指令
    u16 u16DutyPhb;    ///< P0083 B相占空比指令
    u16 u16DutyPhc;    ///< P0084 C相占空比指令
    u16 u16Sector;     ///< P0085 输出电压矢量扇区

    u16 u16Umdc;        ///< P0086 母线电压 0.1v
    u16 u16ElecAngFb;   ///< P0087 电角度反馈
    u16 u16MechAngFb;   ///< P0088 机械角度反馈
    u16 u16CarryFreq;   ///< P0089 载波频率
    u16 u16PwmDutyMax;  ///< P0090 占空比最大值
    u16 u16MechAngRef;  ///< P0091 机械角度指令 0~360

    s16 s16Ai1U;     ///< P0092 模拟量输入, mv
    s16 s16Ai1Bias;  ///< P0093 模拟量偏置, mv
    s16 s16Ai2U;     ///< P0094 模拟量输入, mv
    s16 s16Ai2Bias;  ///< P0095 模拟量偏置, mv

    u16 u16SpdMulRefSel;    ///< P0096 多段数字速度指令选择
    s16 s16SpdDigRefs[16];  ///< P0097 多段数字速度指令

    u16 u16PosTgtSrc;       ///< P0113 位置指令来源
    s64 s64PosDigRef;       ///< P0114 数字位置指令
    u16 u16PosMulRefSel;    ///< P0118 多段数字位置指令选择
    s64 s64PosDigRefs[16];  ///< P0119 多段数字位置速度指令

    u16 u16PosLimSrc;  ///< P0183 位置限制来源
    u16 u16PosLimRes;  ///< P0184 位置限制状态
    u16 s64PosFwdLim;  ///< P0185 正向位置限制
    u16 s64PosRevLim;  ///< P0186 反向位置限制

    s16 s16TrqDigRef;       ///< P0187 数字转矩指令
    u16 u16TrqMulRefSel;    ///< P0188 多段数字转矩指令选择
    s16 s16TrqDigRefs[16];  ///< P0189 多段数字转矩指令

    s16 s16SpdTgt;  ///< P0205 当前速度指令
    s64 s64PosTgt;  ///< P0206 当前位置指令
    s16 s16TrqTgt;  ///< P0210 当前转矩指令

    u16 u16EncFreqDivDir;   ///< P0211 编码器分频输出脉冲方向
    u16 u16EncFreqDivNum;   ///< P0212 编码器分频输出分子
    u16 u16EncFreqDivDeno;  ///< P0213 编码器分频输出分母

    /**
     * @brief Curve Trace
     */

    u16 u16LogSampPrd;   ///< P0214 数据记录采样周期设定
    u16 u16LogSampPts;   ///< P0215 数据记录采样点数设定
    u16 u16LogChCnt;     ///< P0216 数据记录通道数量设定
    u16 u16LogChSrc[8];  ///< P0217 数据记录通道地址设定 (数据源)

    int16_t s16IaBias;  ///<
    int16_t s16IbBias;  ///<
    int16_t s16IcBias;  ///<

    u32 u32LogicSwVer;     ///< P0225
    u32 u32LogicSwSubVer;  ///< P0227
    u16 u16TickCount;      ///< P0229

    u16 u16Temp;  ///< P0230 温度, 0.1℃

} ParaTable_t;

typedef struct __packed {
    s32 s32LogChBuf[2000 * 8];
} CurveTraceBuffer_t;

void ParaTbl_Init(void);

#endif
