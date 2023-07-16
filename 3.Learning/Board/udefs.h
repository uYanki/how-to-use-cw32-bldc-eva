// C-Extensions: https://gcc.gnu.org/onlinedocs/gcc-4.6.2/gcc/C-Extensions.html#C-Extensions
// shell color: https://www.cnblogs.com/unclemac/p/12783387.html

#ifndef __DEFS_H__
#define __DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "uconf.h"

//////////////////////////////////////////////////////////////////////////////////////////////

#define UNUSED(x) ((void)(x))

#define nullptr   ((void*)0)

#define INLINE    inline

#define RW        ///< read / write
#define RO const  ///< read only

//////////////////////////////////////////////////////////////////////////////////////////////

// typedef long double f64;
// typedef long double float64_t;

typedef float  f32;
typedef double f64;

typedef signed char      s8;
typedef signed short     s16;
typedef signed long      s32;
typedef signed long long s64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned long      u32;
typedef unsigned long long u64;

// typedef float  float32_t;
// typedef double float64_t;

// typedef signed char      int8_t;
// typedef signed short     int16_t;
// typedef signed long      int32_t;
// typedef signed long long int64_t;

// typedef unsigned char      uint8_t;
// typedef unsigned short     uint16_t;
// typedef unsigned long      uint32_t;
// typedef unsigned long long uint64_t;

// typedef uint8_t   BYTE;
// typedef uint16_t  WORD;
// typedef uint32_t  DWORD;
// typedef float32_t REAL;

//////////////////////////////////////////////////////////////////////////////////////////////

#if 0
typedef enum {
    FALSE = 0,
    TRUE  = !FALSE,
} Bool_e;
#else
typedef u8 Bool_e;
#endif

#if 0
typedef enum {
    RESET = 0U,
    SET   = !RESET,
} State_e;
#else
typedef FlagStatus State_e;
#endif

#if 0
typedef enum {
    DISABLE = 0U,
    ENABLE  = !DISABLE,
} Enable_e;
#else
typedef FunctionalState Enable_e;
#endif

typedef enum {
    OFF = 0U,
    ON  = !OFF,
} Switch_e;

#if 0
typedef enum {
    FAILURE = 0U,
    SUCCESS = !FAILURE,
} Result_e;
#else
typedef ErrorStatus Result_e;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////

#define NULL                    0

#define PI                      3.1415926f  ///< 圆周率 π = 3.14159265358979323846
#define PI2                     6.2831852f  ///< 2π

#define PHI                     1.618033f   ///< 黄金比例 φ = (1 + sqrt(5)) / 2

#define RAD2DGE                 57.295779f  ///< 1 radians to 57.295780 degress
#define DEG2RAD                 0.017453f   ///< 1 degress to  0.017453 radians

#define SQRT2                   1.414213f
#define SQRT3                   1.732050f
#define SQRT5                   2.236067f
#define INVSQRT2                0.707106f                ///< 1/sqrt(2)
#define INVSQRT3                0.577350f                ///< 1/sqrt(3)
#define INVSQRT5                0.447213f                ///< 1/sqrt(5)

#define S8_MIN                  (s8)0x80                 ///< -128
#define S16_MIN                 (s16)0x8000              ///< -32678
#define S32_MIN                 (s32)0x80000000          ///< -2147483648
#define S64_MIN                 (s64)0x8000000000000000  ///< -9223372036854775808

#define S8_MAX                  (s8)0x7F                 ///< +127
#define S16_MAX                 (s16)0x7FFF              ///< +32767
#define S32_MAX                 (s32)0x7FFFFFFF          ///< +2147483647
#define S64_MAX                 (s64)0x7FFFFFFFFFFFFFFF  ///< +9223372036854775807

#define U8_MAX                  (u8)0xFF                 ///< 255
#define U16_MAX                 (u16)0xFFFF              ///< 65535
#define U32_MAX                 (u32)0xFFFFFFFF          ///< 4294967295
#define U64_MAX                 (u64)0xFFFFFFFFFFFFFFFF  ///< 18446744073709551615

#define F32_MIN                 1.17549435e-38f
#define F64_MIN                 2.2250738585072014e-308

#define F32_MAX                 3.40282347e+38f
#define F64_MAX                 1.7976931348623157e+308

//////////////////////////////////////////////////////////////////////////////////////////////

#define BIT(n)                  (1UL << (n))

#define BIT0                    (1UL << 0)
#define BIT1                    (1UL << 1)
#define BIT2                    (1UL << 2)
#define BIT3                    (1UL << 3)
#define BIT4                    (1UL << 4)
#define BIT5                    (1UL << 5)
#define BIT6                    (1UL << 6)
#define BIT7                    (1UL << 7)
#define BIT8                    (1UL << 8)
#define BIT9                    (1UL << 9)
#define BIT10                   (1UL << 10)
#define BIT11                   (1UL << 11)
#define BIT12                   (1UL << 12)
#define BIT13                   (1UL << 13)
#define BIT14                   (1UL << 14)
#define BIT15                   (1UL << 15)
#define BIT16                   (1UL << 16)
#define BIT17                   (1UL << 17)
#define BIT18                   (1UL << 18)
#define BIT19                   (1UL << 19)
#define BIT20                   (1UL << 20)
#define BIT21                   (1UL << 21)
#define BIT22                   (1UL << 22)
#define BIT23                   (1UL << 23)
#define BIT24                   (1UL << 24)
#define BIT25                   (1UL << 25)
#define BIT26                   (1UL << 26)
#define BIT27                   (1UL << 27)
#define BIT28                   (1UL << 28)
#define BIT29                   (1UL << 29)
#define BIT30                   (1UL << 30)
#define BIT31                   (1UL << 31)

#define MASK8(len)              (~(U8_MAX >> (len)))
#define MASK16(len)             (~(U16_MAX >> (len)))
#define MASK32(len)             (~(U32_MAX >> (len)))
#define MASK64(len)             (~(U64_MAX >> (len)))

#define SETBIT(dat, bit)        ((dat) |= (1UL << (bit)))     ///< set a bit
#define CLRBIT(dat, bit)        ((dat) &= (~(1UL << (bit))))  ///< clear a bit
#define INVBIT(dat, bit)        ((dat) ^= (~(1UL << (bit))))  ///< invert a bit
#define GETBIT(dat, bit)        (((dat) >> (bit)) & 0x1)      ///< get a bit

/**
 * @param stb: start bit
 */
#define GETBIT32(dat, stb, len) (((u32)(dat) >> (stb)) & MASK32(len))  ///< get bits
#define GETBIT64(dat, stb, len) (((u64)(dat) >> (stb)) & MASK64(len))  ///< get bits

//////////////////////////////////////////////////////////////////////////////////////////////

#define MSB(dat)                (0xFF & ((dat) >> 8))          ///< most significant byte
#define LSB(dat)                (0xFF & (dat))                 ///< least significant byte
#define MSW(dat)                (0xFFFF & ((dat) >> 16))       ///< most significant word
#define LSW(dat)                (0xFFFF & (dat))               ///< least significant word

#define INVWORD(dat)            (MSB(dat) | (LSB(dat) << 8))   ///< invert high and low byte in word
#define INVDWORD(dat)           (MSW(dat) | (LSW(dat) << 16))  ///< invert high and low word in dword

//////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @param lhs: left-hand side
 * @param rhs: right-hand side
 */
#define MIN(lhs, rhs)           (((lhs) < (rhs)) ? (lhs) : (rhs))
#define MAX(lhs, rhs)           (((lhs) > (rhs)) ? (lhs) : (rhs))

#define MIN3(a, b, c)           (MIN(a, MIN(b, c)))
#define MAX3(a, b, c)           (MAX(a, MAX(b, c)))

/**
 * @param lo: lowest allowable value
 * @param hi: highest allowable value
 */
#define CLAMP(val, lo, hi)      (MAX(lo, MIN(val, hi)))
#define INOPEN(val, lo, hi)     (((lo) < (val)) && ((val) < (hi)))    ///< check if the val is within the open range
#define INCLOSE(val, lo, hi)    (((lo) <= (val)) && ((val) <= (hi)))  ///< check if the val is within the closed range

//////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief swap integer variable
 * @note  when rhs and lhs are equal, the XOR result of rhs and lhs is 0.
 */
#define SWAP_INT(lhs, rhs)                            \
    do {                                              \
        if (lhs != rhs) { lhs ^= rhs ^= lhs ^= rhs; } \
    } while (0)

///< swap variable
#define SWAP_ANY(type, lhs, rhs) \
    do {                         \
        type tmp;                \
        tmp   = (lhs);           \
        (lhs) = (rhs);           \
        (rhs) = tmp;             \
    } while (0)  ///< use typeof(x) ?

/**
 *              inMax - inMin
 *  outVal  = ------------------  x  ( inVal - inMin ) + outMin
 *             outMax - outMin
 */
#define MapTo(inVal, inMin, inMax, outMin, outMax) ((((inMax) - (inMin)) / (f32)((outMax) - (outMin))) * ((inVal) - (inMin)) + (outMin))

//////////////////////////////////////////////////////////////////////////////////////////////

///< val of elements in an array
#define ARRAY_SIZE(array)                          (sizeof(array) / sizeof(*(array)))
///< byte offset of member in structure (OFFSETOF)
#define MEMBER_OFFSET(structure, member)           ((u32) & (((structure*)0)->member))
///< size of a member of a structure
#define MEMBER_SIZE(structure, member)             (sizeof(((structure*)0)->member))

//////////////////////////////////////////////////////////////////////////////////////////////

#define RETURN_CASE(condition, greater, equal, less) \
    do {                                             \
        u8 caseval = (condition);                    \
        if (caseval > 0) return (greater);           \
        if (caseval < 0) return (less);              \
        return (equal);                              \
    } while (0)

//////////////////////////////////////////////////////////////////////////////////////////////

#if 1

/**
 * @brief level
 */
#define LV_ERR  0  ///< error
#define LV_WRN  1  ///< warning
#define LV_INFO 2  ///< info
#define LV_DBG  3  ///< debug

/**
 * @brief log
 */
#define LOG()
#define LOGE(fmt, ...) LOG()
#define LOGW(fmt, ...) LOG()
#define LOGI(fmt, ...) LOG()
#define LOGD(fmt, ...) LOG()
#endif

//////////////////////////////////////////////////////////////////////////////////////////////

#define STRVAL(x) #x  ///< variable name to string

#if CONFIG_INCLUDE_STDIO

#include <stdio.h>

// #define println(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define println(fmt, args...) printf(fmt "\n", ##args)

/**
 * @param var: number or variable
 * @param stb: start bit
 */
#define log_binary(val, stb, len)                   \
    do {                                            \
        u8 _len = len;                              \
        if (_len > 0) {                             \
            u32 _val = (val) >> (stb);              \
            u32 _msk = 1 << (_len - 1);             \
            do {                                    \
                putchar((_val & _msk) ? '1' : '0'); \
            } while (_msk >>= 1);                   \
        }                                           \
    } while (0)

#define log_binary_fast(val) log_binary(val, 0, 8 * sizeof(val))

#endif

//////////////////////////////////////////////////////////////////////////////////////////////

#if CONFIG_INCLUDE_MATH
#include <math.h>
#endif

//////////////////////////////////////////////////////////////////////////////////////////////

#define isPower2(x)    (!(x & (x - 1)))

/** @addtogroup bit-opt
 * @{
 */

#define lobit0(x)      (~x & (x + 1))  ///< the rightmost 0-bit. eg: 0101 -> 0010, 1010 -> 0001
#define lobit1(x)      (x & -x)        ///< the rightmost 1-bit. eg: 0101 -> 0001, 1010 -> 0010. notice: x ^ (x & (x - 1)) = x & ~(x - 1) = x & (-x)

#define lobits0(x)     (~x & (x - 1))  ///< the rightmost 0-bits. eg: 0100 -> 0011
// #define lobits0(x)  (~(x | -x))
// #define lobits0(x)  ((x & -x) - 1)

#define lobit1bits0(x) (x ^ (x - 1))              ///< the rightmost 1-bit and the 0-bits behind it. eg: 1010 -> 0011

#define lobits0to1(x)  (x | (x - 1))              ///< the rightmost 0-bits convert to 1-bits. eg: 1010 -> 1011
#define lobits1to0(x)  (((x | (x - 1)) + 1) & x)  ///< the rightmost 0-bits convert to 1-bits. eg: 1010 -> 1000

static INLINE s8
hibit1(u32 n)  ///< the leftmost 1-bit. return pos ( align right ), no mask. eg: 0010_0000 -> 5
{
    /**
     * 浮点数 = 尾数 × （ 基数 ^ 阶码 ）, f = m × ( b ^ e )
     *
     *  - 基数 b ：二进制，即 2
     *  - 尾数 m ：用原码表示
     *  - 阶码 e : 用移码表示
     *             -----------------------------------------
     * 32-bits : | S (1-bit) | E (8-bits)  | M (23-bits) |
     * 64-bits : | S (1-bit) | E (11-bits) | M (52-bits) |
     *             -----------------------------------------
     *  - 符号位 S (0正1负), 尾数 M = m, 阶码 E = e + 127 (32-bits) / 2047 (64-bits)
     */

    if (n == 0) return -1;

#if 0
    float b = n;
    return (*((u32*)&b) >> 23 & 255) - 127;
#else
    double b = n;
    return ((*(u64*)&b) >> 52 & 2047) - 1023;
#endif

    /*

    上述代码首先将整数提升成浮点数，然后获得浮点数的地址，对该地址进行强转，转成int型地址。
    获得int型地址之后再获得该地址对应的int值。最后根据浮点数的结构解析阶码减去127即是最左侧1的位置。

    !!! 缺陷(极少数情况):

        eg. to float:

            u32 num1 = (1 << 25);      // 0000_0010_0000_0000_0000_0000_0000_0000 -> output = 25
            u32 num2 = (1 << 25) - 1;  // 0000_0001_1111_1111_1111_1111_1111_1111 -> output = 24

            但是在我的电脑上 num2 的输出时 25 ?
            原因是: 浮点数精度缺少, 浮点数的尾数最大只能包含24个有效位, 因此编译器对末尾进行了处理：① num2+=1 (产生了进位) ② num2-=1

        eg. 2147483584 (0111_1111_1111_1111_1111_1111_1100_0000) -> 2147483648.0 ( 1000_0000_0000_0000_0000_0000_0000_0000 )

            此处相差了 64 !!最大的整数2^31-1包含31个1，在转换成浮点数的时候最多可能会丢弃7位有效数字。7位有效数字的上限是128，
            根据丢弃值的大小是否大于等于64，编译器会选择对结果加1，从而产生进位和误差。解决方案是 flaot 转 double, double 尾数有 52 位
            总以承载整个int整数。但是对64位整数，它也无能为力了。
    */
}

static INLINE u8 cntbit(u32 x)  ///< number of 1-bit
{
    u32 t = x;

    t = ((t & 0xAAAA) >> 1) + (t & 0x5555);
    t = ((t & 0xCCCC) >> 2) + (t & 0x3333);
    t = ((t & 0xF0F0) >> 4) + (t & 0x0F0F);
    t = ((t & 0xFF00) >> 8) + (t & 0x00FF);

    return t;
}

/**
 * @}
 */

//////////////////////////////////////////////////////////////////////////////////////////////

static INLINE int _isalpha(int c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
static INLINE int _islower(int c) { return (c >= 'a' && c <= 'z'); }
static INLINE int _isupper(int c) { return (c >= 'a' && c <= 'z'); }

static INLINE int _isdigit(int c) { return ('0' <= c && c <= '9'); }                                              ///< is decimal number
static INLINE int _isxdigit(int c) { return (_isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')); }  ///< is hexadecimal number

static INLINE int _isspace(int c) { return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'); }

//////////////////////////////////////////////////////////////////////////////////////////////

#if CONFIG_USING_CRC

///< crc test: http://www.ip33.com/crc.html
u8  crc8(RO u8* dat, u16 len);
u16 crc16(RO u8* dat, u16 len);

#endif

//////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @param dest: destination
 * @param src:  source
 */

#if CONFIG_USING_MEMORY

void* _memset(void* src, u8 chr, u32 len);
void* _memcpy(void* dest, RO void* src, u32 len);
void* _memmove(void* dest, RO void* src, u32 len);

#endif

#if CONFIG_USING_STRING

u32 _strlen(RO u8* str);                                ///< calculate the length of a string
s32 _strcmp(RO u8* str1, RO u8* str2);                  ///< compare the size of two strings
u8* _strcpy(u8* dest, RO u8* src);                      ///< copy the source string to the destination string
u8* _strncpy(u8* dest, RO u8* src, u32 len);            ///< copy the first n characters of the source string to the destination string
u8* _strcat(u8* dest, RO u8* src);                      ///< append the source string to the end of the destination string
u8* _strstr(RO u8* str, RO u8* substr);                 ///< find the position of a substring in a string
u8* _substr(u8* dest, RO u8* src, u32 start, u32 len);  ///< take n characters from a string starting at a certain position

#endif

//////////////////////////////////////////////////////////////////////////////////////////////

s32 _atoi(RO u8* str);                 ///< ascii to decimal integer
u32 _atox(RO u8* str);                 ///< ascii to hexadecimal integer
f64 _atof(RO u8* str);                 ///< ascii to real
u8* _itoa(u8* str, s32 num, u8 base);  ///< integers from 2 to 36 bases convert to ascii

//////////////////////////////////////////////////////////////////////////////////////////////

void _swap(void* src1, void* src2, u32 size);

#if CONFIG_USING_SORT

///< swap elements when function ('cmp') return value is greater than 0
void _bsort(void* array, u32 count, u32 size, s32 (*cmp)(RO void*, RO void*));
void _qsort(void* array, u32 count, u32 size, s32 (*cmp)(RO void*, RO void*));

#endif

//////////////////////////////////////////////////////////////////////////////////////////////

#if CONFIG_USING_COLOR

typedef struct {
    union {
        u32 v;
        struct {
            u8 a;  ///< alpha: 0(fully transparent)~128(translucent)~255(opaque)
            u8 r;  ///< red value
            u8 g;  ///< green value
            u8 b;  ///< blue value
        };
    };
} ARGB_t;

typedef struct {
    union {
        u32 v;
        struct {
            u8 a;
            u8 b;
            u8 g;
            u8 r;
        };
    };
} ABGR_t;

typedef ARGB_t RGB_t;
typedef ABGR_t BGR_t;

typedef struct {
    f32 h;  ///< hue: 0(RED)~120(GREEN)~240(BLUE)~360(RED), 色相
    f32 s;  ///< saturation: 0(colorless)~1(colorful), 饱和度
    f32 v;  ///< value/brightness/lightness: 0(BLACK)~1(WHITE), 明度
} HSV_t;

#define MAKE_ARGB(s, A, R, G, B) (s.a = A, s.r = R, s.g = G, s.b = B, s)
#define MAKE_RGB(s, R, G, B)     MAKE_ARGB(s, 0xFF, R, G, B)
#define MAKE_ABGR(s, A, B, G, R) MAKE_ARGB(s, A, R, G, B)
#define MAKE_BGR(s, B, G, R)     MAKE_ABGR(s, 0xFF, B, G, R)

#define RGB(r, g, b)             ((0xFF & (r)) << 16 | (0xFF & (g)) << 8 | (0xFF & (b)))
#define BGR(b, g, r)             ((0xFF & (b)) << 16 | (0xFF & (g)) << 8 | (0xFF & (r)))

#define RGB2BGR(rgb)             (((0xFF0000 & (rgb)) >> 16) | (0x00FF00 & (rgb)) | ((0x0000FF & (rgb)) << 16))
#define BGR2RGB(bgr)             (((0xFF0000 & (bgr)) >> 16) | (0x00FF00 & (bgr)) | ((0x0000FF & (bgr)) << 16))

#define RGB_GetR(rgb)            (0xFF & ((rgb) >> 16))
#define RGB_GetG(rgb)            (0xFF & ((rgb) >> 8))
#define RGB_GetB(rgb)            (0xFF & ((rgb) >> 0))

#define BGR_GetB(bgr)            (0xFF & ((bgr) >> 16))
#define BGR_GetG(bgr)            (0xFF & ((bgr) >> 8))
#define BGR_GetR(bgr)            (0xFF & ((bgr) >> 0))

/** @brief RGB Color
 *  @note Recommend installing the vscode extension `Color Highlight` to view colors
 * @{
 */

#define RGBCR_RED                0xFF0000  ///< 红色
#define RGBCR_GREEN              0x00FF00  ///< 绿色
#define RGBCR_BLUE               0x0000FF  ///< 蓝色
#define RGBCR_YELLOW             0xFFFF00  ///< 黄色
#define RGBCR_PURPLE             0x800080  ///< 紫色
#define RGBCR_ORANGE             0xFFA500  ///< 橙色
#define RGBCR_PINK               0xFFC0CB  ///< 粉色
#define RGBCR_BROWN              0xA52A2A  ///< 棕色
#define RGBCR_GRAY               0x808080  ///< 灰色
#define RGBCR_BLACK              0x000000  ///< 黑色
#define RGBCR_WHITE              0xFFFFFF  ///< 白色
#define RGBCR_SILVER             0xC0C0C0  ///< 银色
#define RGBCR_GOLD               0xFFD700  ///< 金色
#define RGBCR_NAVY               0x000080  ///< 海军蓝
#define RGBCR_TEAL               0x008080  ///< 鸭绿色
#define RGBCR_OLIVE              0x808000  ///< 橄榄色
#define RGBCR_MAROON             0x800000  ///< 栗色
#define RGBCR_FUCHSIA            0xFF00FF  ///< 紫红色
#define RGBCR_CYAN               0x00FFFF  ///< 青色
#define RGBCR_CORAL              0xFF7F50  ///< 珊瑚色
#define RGBCR_SKY_BLUE           0x87CEEB  ///< 天蓝色
#define RGBCR_TURQUOISE          0x40E0D0  ///< 绿松石色
#define RGBCR_INDIGO             0x4B0082  ///< 靛蓝色
#define RGBCR_VIOLET             0xEE82EE  ///< 紫罗兰色
#define RGBCR_BEIGE              0xF5F5DC  ///< 米色
#define RGBCR_KHAKI              0xF0E68C  ///< 卡其色
#define RGBCR_SALMON             0xFA8072  ///< 鲑鱼色
#define RGBCR_TOMATO             0xFF6347  ///< 番茄红
#define RGBCR_ORCHID             0xDA70D6  ///< 紫兰色
#define RGBCR_LIGHT_GRAY         0xD3D3D3  ///< 浅灰色
#define RGBCR_LIGHT_CORAL        0xF08080  ///< 浅珊瑚色
#define RGBCR_LIGHT_SEA_GREEN    0x20B2AA  ///< 浅海洋绿
#define RGBCR_LIGHT_BLUE         0xADD8E6  ///< 淡蓝色
#define RGBCR_LIGHT_GREEN        0x90EE90  ///< 淡绿色
#define RGBCR_LIGHT_YELLOW       0xFFFFE0  ///< 浅黄色
#define RGBCR_LIGHT_PINK         0xFFB6C1  ///< 浅粉色
#define RGBCR_DARK_GREEN         0x006400  ///< 暗绿色
#define RGBCR_DARK_BLUE          0x00008B  ///< 暗蓝色
#define RGBCR_DARK_RED           0x8B0000  ///< 暗红色
#define RGBCR_DARK_CYAN          0x008B8B  ///< 暗青色
#define RGBCR_DARK_MAGENTA       0x8B008B  ///< 暗洋红色
#define RGBCR_DARK_ORANGE        0xFF8C00  ///< 暗橙色
#define RGBCR_DARK_VIOLET        0x9400D3  ///< 暗紫罗兰色
#define RGBCR_DARK_GOLDENROD     0xB8860B  ///< 暗金黄色
#define RGBCR_DARK_KHAKI         0xBDB76B  ///< 暗卡其色
#define RGBCR_DARK_OLIVE_GREEN   0x556B2F  ///< 暗橄榄绿色
#define RGBCR_DARK_SEA_GREEN     0x8FBC8F  ///< 暗海绿色
#define RGBCR_DARK_SLATE_BLUE    0x483D8B  ///< 暗灰蓝色
#define RGBCR_DARK_TURQUOISE     0x00CED1  ///< 暗绿宝石色
#define RGBCR_DARK_SALMON        0xE9967A  ///< 暗肉色
#define RGBCR_DARK_GRAY          0xA9A9A9  ///< 暗灰色
#define RGBCR_DEEP_SKY_BLUE      0x00BFFF  ///< 深天蓝色
#define RGBCR_FIRE_BRICK         0xB22222  ///< 火砖色
#define RGBCR_FOREST_GREEN       0x228B22  ///< 森林绿
#define RGBCR_GAINSBORO          0xDCDCDC  ///< 淡灰色
#define RGBCR_INDIAN_RED         0xCD5C5C  ///< 印第安红
#define RGBCR_DIM_GRAY           0x696969  ///< 暗灰色
#define RGBCR_MEDIUM_BLUE        0x0000CD  ///< 中蓝色
#define RGBCR_CRIMSON            0xDC143C  ///< 深红色
#define RGBCR_PALE_GREEN         0x98FB98  ///< 苍绿色
#define RGBCR_SEA_GREEN          0x2E8B57  ///< 海绿色
#define RGBCR_STEEL_BLUE         0x4682B4  ///< 钢蓝色
#define RGBCR_ROSY_BROWN         0xBC8F8F  ///< 玫瑰棕色
#define RGBCR_AQUAMARINE         0x7FFFD4  ///< 绿色宝石
#define RGBCR_BLUE_VIOLET        0x8A2BE2  ///< 蓝紫色
#define RGBCR_CHOCOLATE          0xD2691E  ///< 巧克力色

/**
 * @}
 */

void RGB2HSV(u8 r, u8 g, u8 b, f32* h, f32* s, f32* v);

#endif

//////////////////////////////////////////////////////////////////////////////////////////////

#if 0

#define ONESHOT    0  // 单次
#define CONTINUOUS 1  // 连续

#endif

//////////////////////////////////////////////////////////////////////////////////////////////

#if CONFIG_USING_DELAY

#define TIME_UNIT_US 1e-3
#define TIME_UNIT_MS 1
#define TIME_UNIT_S  1e+3

#if 0
    switch (nUnit) {
        case TIME_UNIT_US:
            nWaitTime /= 1000;
            break;
        default:
        case TIME_UNIT_MS:
            // pass
            break;
        case TIME_UNIT_S:
            nWaitTime *= 1000;
            break;
    }
#endif

void   DelayInit(void);
void   DelayBlock(u32 nWaitTime);
Bool_e DelayNonBlock(u32 nStartTick, u32 nWaitTime);

#endif

#ifdef __cplusplus
}
#endif

#endif  // __DEFS_H__
