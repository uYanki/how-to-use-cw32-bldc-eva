#ifndef __USDK_DEFS_H__
#define __USDK_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Modifiers
//

#define RW        ///< read write
#define RO const  ///< read only
#define WO        ///< write only

#define REG volatile  ///< register

#define __IN   ///< input
#define __OUT  ///< output
#define __INOUT

#define __private static
#define __public

#define INLINE    static inline

#define UNUSED(x) ((void)(x))

//---------------------------------------------------------------------------
// Types
//

typedef signed char      s8;
typedef signed short     s16;
typedef signed long      s32;
typedef signed long long s64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned long      u32;
typedef unsigned long long u64;

typedef float  f32;
typedef double f64;
// typedef long double f64;

typedef int          INT;
typedef unsigned int UINT;

typedef s8  INT8;
typedef s16 INT16;
typedef s32 INT32;
typedef s64 INT64;

typedef u8  UINT8;
typedef u16 UINT16;
typedef u32 UINT32;
typedef u64 UINT64;

typedef f32 float32_t, FLOAT32;
typedef f64 float64_t, FLOAT64;

//---------------------------------------------------------

typedef volatile signed char      vs8;
typedef volatile signed short     vs16;
typedef volatile signed long      vs32;
typedef volatile signed long long vs64;

typedef volatile unsigned char      vu8;
typedef volatile unsigned short     vu16;
typedef volatile unsigned long      vu32;
typedef volatile unsigned long long vu64;

typedef volatile float vf32;
typedef volatile float vf64;

//---------------------------------------------------------

typedef u8  BYTE;
typedef u16 WORD;
typedef u32 DWORD;  ///< double
typedef u64 QWORD;  ///< quad
typedef f32 REAL;

//---------------------------------------------------------

typedef REG u8*  REG8;
typedef REG u16* REG16;
typedef REG u32* REG32;
typedef REG u64* REG64;

//---------------------------------------------------------

#if 0
#include <stdbool.h>
#else
#define bool  char
#define true  1
#define false 0
#endif

//---------------------------------------------------------

typedef void (*lpfnCbk0_t)(void);
// typedef bool_e (*lpfnCbk_t)(void* arg);

//---------------------------------------------------------------------------
// Enums
//

typedef enum {
    FALSE = 0U,
    TRUE  = !FALSE,
} boolean_e;

typedef boolean_e bool_e;

typedef enum {
    OFF = 0U,
    ON  = !OFF,
} switch_e;

typedef enum {
    DIR_CCW = 0,         ///< Clockwise, 顺时针
    DIR_CW  = !DIR_CCW,  ///< Counterclockwise, 逆时针
} direction_e;

typedef enum {
    ONESHOT    = 0,  ///< 单次
    CONTINUOUS = 1,  ///< 连续
} times_e;

/**
 * number = 0x1234
 *
 *      低地址  高地址
 * 大端   0x12   0x34
 * 小端   0x34   0x12
 *
 */

typedef enum {
    ENDIAN_BIG,     ///< 高位在低地址
    ENDIAN_LITTLE,  ///< 低位在高地址
} endian_e;

typedef enum {
    UINT_FORMAT_BIN = 0,  // binary
    UINT_FORMAT_DEC = 1,  // decimal
    UINT_FORMAT_HEX = 2,  // hexadecimal
    UINT_FORMAT_BCD = 3,
} uint_fmt_e;

//---------------------------------------------------------------------------
// Unions
//---------------------------------------------------------------------------

/**
 * @note
 *
 * f32 = pow(-1,sign) * (1+mag) * pow(2,exp-127)
 * f64 = pow(-1,sign) * (1+mag) * pow(2,exp-1023)
 *
 */

typedef union {
    f32 all;
    struct {
        u32 mag  : 23;  // 小数位
        u32 exp  : 8;   // 指数位
        u32 sign : 1;   // 符号位
    } bit;
} f32u, float32_u;

typedef union {
    f64 all;
    struct {
        u64 mag  : 52;
        u64 exp  : 11;
        u64 sign : 1;
    } bit;
} f64u, float64_u;

//---------------------------------------------------------------------------
// Constants
//

#ifndef NULL
#define NULL 0
#endif

#ifndef nullptr
#define nullptr ((void*)0)
#endif

//---------------------------------------------------------

#define S8_MIN                                     (s8)0x80                 ///< -128
#define S16_MIN                                    (s16)0x8000              ///< -32678
#define S32_MIN                                    (s32)0x80000000          ///< -2147483648
#define S64_MIN                                    (s64)0x8000000000000000  ///< -9223372036854775808

#define S8_MAX                                     (s8)0x7F                 ///< +127
#define S16_MAX                                    (s16)0x7FFF              ///< +32767
#define S32_MAX                                    (s32)0x7FFFFFFF          ///< +2147483647
#define S64_MAX                                    (s64)0x7FFFFFFFFFFFFFFF  ///< +9223372036854775807

#define U8_MAX                                     (u8)0xFF                 ///< +255
#define U16_MAX                                    (u16)0xFFFF              ///< +65535
#define U32_MAX                                    (u32)0xFFFFFFFF          ///< +4294967295
#define U64_MAX                                    (u64)0xFFFFFFFFFFFFFFFF  ///< +18446744073709551615

#define F32_MIN                                    1.17549435e-38f
#define F64_MIN                                    2.2250738585072014e-308

#define F32_MAX                                    3.40282347e+38f
#define F64_MAX                                    1.7976931348623157e+308

//---------------------------------------------------------

#define M_PI                                       3.1415926f  ///< 圆周率   π = 3.14159265358979323846
#define M_E                                        2.7182818f  ///< 自然常数 e = 2.71828182845904523536 = (1+1/n)^n
#define M_PHI                                      1.618033f   ///< 黄金比例 φ = 1.61803398874989484820 = (1+sqrt(5))/2

#define M_PI_X2                                    6.2831852f  ///< 2π
#define M_PI_X100                                  314         ///< 100π

#define M_LOG2E                                    1.4426950f  // log2(e) = 1.44269504088896340736
#define M_LOG10E                                   0.4342944f  // log10(e) = 0.434294481903251827651
#define M_LN2                                      0.6931471f  // ln(2) = 0.693147180559945309417
#define M_LN10                                     2.3025850f  // ln(10) = 2.30258509299404568402

#define M_RAD2DGE                                  57.295779f  ///< 1 radians to 57.295780 degress
#define M_DEG2RAD                                  0.017453f   ///< 1 degress to  0.017453 radians

#define M_SIN15                                    0.258819f
#define M_SIN30                                    0.5f
#define M_SIN45                                    0.707106f  ///< sqrt(2)/2
#define M_SIN60                                    0.866025f  ///< sqrt(3)/2
#define M_SIN75                                    0.965925f

#define M_COS15                                    SIN75
#define M_COS30                                    SIN60
#define M_COS45                                    SIN45
#define M_COS60                                    SIN30
#define M_COS75                                    SIN15

#define M_SQRT2                                    1.414213f
#define M_SQRT3                                    1.732050f
#define M_SQRT5                                    2.236067f
#define M_INVSQRT2                                 0.707106f  ///< 1/sqrt(2)
#define M_INVSQRT3                                 0.577350f  ///< 1/sqrt(3)
#define M_INVSQRT5                                 0.447213f  ///< 1/sqrt(5)

//---------------------------------------------------------------------------
// Macros
//

/**
 * @param lhs: left-hand side
 * @param rhs: right-hand side
 */
#define MIN(lhs, rhs)                              (((lhs) < (rhs)) ? (lhs) : (rhs))
#define MAX(lhs, rhs)                              (((lhs) > (rhs)) ? (lhs) : (rhs))

#define MIN3(a, b, c)                              (MIN(a, MIN(b, c)))
#define MAX3(a, b, c)                              (MAX(a, MAX(b, c)))

/**
 * @param lo: lowest allowable value
 * @param hi: highest allowable value
 */
#define CLAMP(val, lo, hi)                         (MAX(lo, MIN(val, hi)))
#define INOPEN(val, lo, hi)                        (((lo) < (val)) && ((val) < (hi)))    ///< check if the val is within the open range
#define INCLOSE(val, lo, hi)                       (((lo) <= (val)) && ((val) <= (hi)))  ///< check if the val is within the closed range

/**
 *              OutMax - OutMin
 *  outVal  = -------------------  x  ( inVal - inMin ) + outMin
 *               inMax - inMin
 */
#define MapTo(inVal, inMin, inMax, outMin, outMax) ((((outMax) - (outMin)) / (f32)((inMax) - (inMin))) * ((inVal) - (inMin)) + (outMin))

//---------------------------------------------------------

///< val of elements in an array
#define ARRAY_SIZE(array)                          (sizeof(array) / sizeof(*(array)))
///< byte offset of member in structure (OFFSETOF)
#define MEMBER_OFFSET(structure, member)           ((u32)(char*)&(((structure*)0)->member))
///< size of a member of a structure
#define MEMBER_SIZE(structure, member)             (sizeof(((structure*)0)->member))

//---------------------------------------------------------

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
    } while (0)

//---------------------------------------------------------

#ifndef _BV
#define _BV(n) (1UL << (n))
#endif

#define MASK8(LEN)               (~(U8_MAX << (LEN)))
#define MASK16(LEN)              (~(U16_MAX << (LEN)))
#define MASK32(LEN)              (~(U32_MAX << (LEN)))
#define MASK64(LEN)              (~(U64_MAX << (LEN)))

// @note STB: start bit

#define SETBIT(DAT, BIT)         ((DAT) |= (1UL << (BIT)))           ///< set a bit (bit-32)
#define CLRBIT(DAT, BIT)         ((DAT) &= (~(1UL << (BIT))))        ///< clear a bit (bit-32)
#define INVBIT(DAT, BIT)         ((DAT) ^= (1UL << (BIT)))           ///< invert a bit (bit-32)
#define GETBIT(DAT, BIT)         (((DAT) >> (BIT)) & 0x1)            ///< get a bit (bit-32/64)
#define SETBITS(DAT, STB, LEN)   ((DAT) |= (MASK32(LEN) << (STB)))   ///< set bits (bit-32)
#define CLRBITS(DAT, STB, LEN)   ((DAT) &= ~(MASK32(LEN) << (STB)))  ///< clear bits (bit-32)
#define INVBITS(DAT, STB, LEN)   ((DAT) ^= (MASK32(LEN) << (STB)))   ///< invert bits (bit-32)
#define GETBITS(DAT, STB, LEN)   (((DAT) >> (STB)) & MASK32(LEN))    ///< get bits (bit-32)

#define SETBIT64(DAT, BIT)       ((DAT) |= (1ULL << (BIT)))     ///< set a bit (bit-64)
#define CLRBIT64(DAT, BIT)       ((DAT) &= (~(1ULL << (BIT))))  ///< clear a bit (bit-64)
#define INVBIT64(DAT, BIT)       ((DAT) ^= (1ULL << (BIT)))     ///< invert a bit (bit-64)

#define LOBYTE(DAT)              (0xFF & (DAT))
#define HIBYTE(DAT)              (0xFF & ((DAT) >> 8))
#define LOWORD(DAT)              (0xFFFF & (DAT))
#define HIWORD(DAT)              (0xFFFF & ((DAT) >> 16))

///< link two 16bits data to a 32bits data
#define LINK32(HI, LO)           (((u32)(HI) << 16) | ((u16)(LO)))
///< link two 32bits data to a 64bits data
#define LINK64_2(HI, LO)         (((u64)(HI) << 32) | ((u32)(LO)))
///< link four 16bits data to a 64bits data
#define LINK64_4(W3, W2, W1, W0) (((u64)(W3) << 48) | ((u64)(W2) << 32) | ((u64)(W1) << 16) | ((u64)(W0)))

//---------------------------------------------------------

#define RETURN_CASE(condition, greater, equal, less) \
    do {                                             \
        u8 caseval = (condition);                    \
        if (caseval > 0) return (greater);           \
        if (caseval < 0) return (less);              \
        return (equal);                              \
    } while (0)

INLINE s32 sign(s32 n)
{
    return (n >> 31) | ((u32)(-n) >> 31);
}

//---------------------------------------------------------------------------
// Functions
//

INLINE f32 _floor(f32 x) { return (f32)((s32)x - (x < 0.0f && x != (s32)x)); }         ///< 向下取整
INLINE f32 _ceil(f32 x) { return (f32)((s32)x + (x > 0.0f && x != (s32)x)); }          ///< 向上取整
INLINE f32 _round(f32 x) { return (x >= 0.0f) ? _floor(x + 0.5f) : _ceil(x - 0.5f); }  ///< 四舍五入

//---------------------------------------------------------------------------
// Components
//

//---------------------------------------------------------
///< @defgroup memory

/**
 * @param dest: destination
 * @param src:  source
 */

void* _memset(void* src, u8 chr, u32 len);
void* _memcpy(void* dest, RO void* src, u32 len);
void* _memmove(void* dest, RO void* src, u32 len);
void  _memswap(void* src1, void* src2, u32 len);

s32 _memcmp(RO void* src1, RO void* src2, u32 len);

void* _memxor(void* src, u8 chr, u32 len);      ///< 异或
void* _memlchr(RO void* src, u8 chr, u32 len);  ///< 顺找
void* _memrchr(RO void* src, u8 chr, u32 len);  ///< 倒找

void _mem_lshift(void* ptr, u16 len, u16 shift);
void _mem_rshift(void* ptr, u16 len, u16 shift);

//---------------------------------------------------------
///< @defgroup sort

typedef s32 (*sort_cmp_t)(RO void*, RO void*);

///< swap elements when function ('cmp') return value is greater than 0
void _bsort(void* array, u32 count, u32 size, sort_cmp_t cmp);  // bubble
void _qsort(void* array, u32 count, u32 size, sort_cmp_t cmp);  // quick

//---------------------------------------------------------
///< @defgroup crc (http://www.ip33.com/crc.html)

u8  crc8(RO u8* dat, u16 len);
u16 crc16(RO u8* dat, u16 len);

//---------------------------------------------------------
///< @defgroup string

#if CONFIG_MODULE_STRING

typedef const char* cstr;

INLINE char _islower(char c) { return ('a' <= c && c <= 'z'); }
INLINE char _isupper(char c) { return ('A' <= c && c <= 'Z'); }

INLINE int _isalpha(char c) { return _islower(c) || _isupper(c); }

///< is binary number
INLINE bool _isbdigit(char c) { return (c == '0' || c == '1'); }
///< is decimal number
INLINE bool _isdigit(char c) { return ('0' <= c && c <= '9'); }
///< is hexadecimal number
INLINE bool _isxdigit(char c) { return (_isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')); }
///< is blank character
INLINE bool _isspace(char c) { return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'); }

s32 _str2dec(__IN RO u8* str);                  ///< ascii to decimal integer
u32 _str2hex(__IN RO u8* str);                  ///< ascii to hexadecimal integer
f32 _str2flt(__IN RO u8* str);                  ///< ascii to real
u8* _int2str(__OUT u8* str, s32 num, u8 base);  ///< integers from 2 to 36 bases convert to ascii

u32 _strlen(RO u8* str);                                ///< calculate the length of a string
s32 _strcmp(RO u8* str1, RO u8* str2);                  ///< compare the size of two strings
u8* _strcpy(u8* dest, RO u8* src);                      ///< copy the source string to the destination string
u8* _strncpy(u8* dest, RO u8* src, u32 len);            ///< copy the first n characters of the source string to the destination string
u8* _strcat(u8* dest, RO u8* src);                      ///< append the source string to the end of the destination string
u8* _strstr(RO u8* str, RO u8* substr);                 ///< find the position of a substring in a string
u8* _substr(u8* dest, RO u8* src, u32 start, u32 len);  ///< take n characters from a string starting at a certain position

u8* _strlwr(RO u8* str);  ///< lower
u8* _strupr(RO u8* str);  ///< upper

#endif

//---------------------------------------------------------
///< @defgroup math

f32 _sqrt(f32 n);
f32 _polynomial(f32 x, f32 b[], u8 n);

// clang-format off

#if 1
INLINE f32 _square(f32 n) { return n * n; }
INLINE f32 _cube(f32 n) { return n * n * n; }
#else
#define _square(x) ({ (x) * (x); })
#define _cube(x)   ({ (x) * (x) * (x); })
#endif

// clang-format on

//---------------------------------------------------------
///< @defgroup byte swap

INLINE u16 _bswap16(u16 x)
{
    return ((x & 0xFF00) >> 8) |
           ((x & 0x00FF) << 8);
}

INLINE u32 _bswap32(u32 x)
{
    return ((x & 0xFF000000) >> 24) |
           ((x & 0x00FF0000) >> 8) |
           ((x & 0x0000FF00) << 8) |
           ((x & 0x000000FF) << 24);
}

INLINE u64 _bswap64(u64 x)
{
    return ((x & 0xFF00000000000000) >> 56) |
           ((x & 0x00FF000000000000) >> 40) |
           ((x & 0x0000FF0000000000) >> 24) |
           ((x & 0x000000FF00000000) >> 8) |
           ((x & 0x00000000FF000000) << 8) |
           ((x & 0x0000000000FF0000) << 24) |
           ((x & 0x000000000000FF00) << 48) |
           ((x & 0x00000000000000FF) << 56);
}

#ifdef __cplusplus
}
#endif

#endif
