#include "udefs.h"

#if CONFIG_USING_CRC

u8 crc8(RO u8* dat, u16 len)
{
    //  x^8 + x^2 + x^1 + 1
    u8 i, crc = 0x00;

    while (len--) {
        crc ^= *dat++;
        for (i = 0; i < 8; ++i) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

u16 crc16(RO u8* dat, u16 len)
{
    // x^16 + x^15 + x^2 + 1

    u16 i, crc = 0xFFFF;

    while (len--) {
        crc ^= *dat++;
        for (i = 0; i < 8; ++i) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

#endif

#if CONFIG_USING_STRING

u32 _strlen(RO u8* str)
{
    RO u8* p = str;
    while (*++p) {}
    return (p - str);
}

s32 _strcmp(RO u8* str1, RO u8* str2)
{
    for (; *str1 == *str2 && *str1; str1++, str2++) {}
    return (*str1 - *str2);
}

u8* _strcpy(u8* dest, RO u8* src)
{
    u8* p = dest;
    while ((*p++ = *src++)) {}
    return dest;
}

u8* _strcat(u8* dest, RO u8* src)
{
    u8* dest = dest;
    while (*dest++) {}
    while ((*dest++ = (u8)*src++)) {}
    return dest;
}

u8* _strstr(RO u8* str, RO u8* substr)
{
    for (; *str; ++str) {
        RO u8 *p1 = str, *p2 = substr;
        while (*p1 && *p2 && *p1 == *p2) { ++p1, ++p2; }
        if (!*p2) return (u8*)str;
    }
    return nullptr;
}

u8* _strncpy(u8* dest, RO u8* src, u32 len)
{
    u8* p = dest;
    while ((len-- > 0) && (*dest++ = *src++)) {}
    while ((len-- > 0)) { *dest++ = '\0'; }
    return p;
}

u8* _substr(u8* dest, RO u8* src, u32 start, u32 len)
{
    RO u8 *p = src + start, *ret = dest;

    while (*p && len--) { *dest++ = *p++; }
    *dest = '\0';

    return ret;
}

#endif

#if CONFIG_USING_MEMORY

void* _memcpy(void* dest, RO void* src, u32 len)
{
    u8*    dp = dest;
    RO u8* sp = src;
    while (len--) { *dp++ = *sp++; }
    return dest;
}

void* _memmove(void* dest, RO void* src, u32 len)
{
    u8*    dp = dest;
    RO u8* sp = src;
    if (dp < sp) {
        while (len--) {
            *dp++ = *sp++;
        }
    } else if (dp > sp) {
        dp += len;
        sp += len;
        while (len--) {
            *--dp = *--sp;
        }
    }
    return dest;
}

void* _memset(void* src, u8 chr, u32 len)
{
    u8* p = src;
    while (len--) { *p++ = chr; }
    return src;
}

#endif

s32 _atoi(RO u8* str)
{
    u8  sig = 1;
    s32 ret = 0;

    while (*str == ' ' || (*str >= '\t' && *str <= '\r')) {
        ++str;
    }

    if (*str == '-') {
        sig = 0;
        ++str;
    } else if (*str == '+') {
        ++str;
    }

    while (*str >= '0' && *str <= '9') {
        ret = ret * 10 + (*str++ - '0');
    }

    return sig ? ret : -ret;
}

u32 _atox(RO u8* str)
{
    u8  num;
    u32 ret = 0;

    while (*str) {
        if ('0' <= *str && *str <= '9') {
            num = *str - '0';
        } else if ('a' <= *str && *str <= 'f') {
            num = *str - 'a' + 10;
        } else if ('A' <= *str && *str <= 'F') {
            num = *str - 'A' + 10;
        } else {
            break;
        }
        ret *= 16;
        ret += num;
        ++str;
    }

    return ret;
}

f64 _atof(RO u8* str)
{
    u8  sig   = 1;
    f64 ret   = 0.0;
    f64 power = 1.0;

    while (*str == ' ' || (*str >= '\t' && *str <= '\r')) {
        ++str;
    }

    if (*str == '-') {
        sig = 0;
        ++str;
    } else if (*str == '+') {
        ++str;
    }

    while (*str >= '0' && *str <= '9') {
        ret *= 10.0;
        ret += (*str++ - '0');
    }

    if (*str == '.') {
        str++;
        while (*str >= '0' && *str <= '9') {
            ret *= 10.0;
            ret += (*str++ - '0');
            power *= 10.0;
        }
    }

    return (sig ? ret : -ret) / power;
}

u8* _itoa(u8* str, s32 num, u8 base)
{
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }

    u8 *p = str, *q = str;

    u8  tmp;
    u32 rem;  // remainder
    u32 val = (num < 0) ? -num : num;

    do {
        rem = val % base;
#if 0
        // *p++ = "0123456789abcdefghijklmnopqrstuvwxyz"[rem];
        *p++ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rem];
#else
        // *p++ = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
        *p++ = (rem < 10) ? (rem + '0') : (rem - 10 + 'A');
#endif
        val /= base;
    } while (val != 0);

    // negative
    if (num < 0) { *p++ = '-'; }

    // endwith '0'
    *p-- = '\0';

    // reverse string
    while (q < p) {
        tmp  = *q;
        *q++ = *p;
        *p-- = tmp;
    }

    return str;
}

void _swap(void* src1, void* src2, u32 size)
{
    u8* p1 = (u8*)src1;
    u8* p2 = (u8*)src2;
    u8  tmp;
    while (size--) {
        tmp   = *p1;
        *p1++ = *p2;
        *p2++ = tmp;
    }
}

#if CONFIG_USING_SORT

/**
 * @brief bubble sort and quick sort
 *
 * @param array  start of target array.
 * @param count array size in elements.
 * @param size  element size in bytes.
 * @param cmp   pointer to a user-supplied routine that compares two array elements
 *              and returns a value that specifies their relationship.
 *
 *      @example s32 compare(RO void* p1, RO void* p2) { return (*(s32*)p1 - *(s32*)p2); }
 *          @retval -1, 0, +1
 *
 */

void _bsort(void* array, u32 count, u32 size, s32 (*cmp)(RO void*, RO void*))
{
    u32 i, j;

    if (size < 2 || count < 2) { return; }

    for (i = 0; i < count - 1; ++i) {
        for (j = 0; j < count - i - 1; ++j) {
            u8* p1 = (u8*)array + j * size;
            u8* p2 = p1 + size;
            if (cmp(p1, p2) > 0) {
                _swap(p1, p2, size);
            }
        }
    }
}

void _qsort(void* array, u32 count, u32 size, s32 (*cmp)(RO void*, RO void*))
{
    if (count <= 1) return;

    u8* base  = (u8*)array;
    u8* left  = base;
    u8* right = base + (count - 1) * size;
    u8* pivot = base + (count / 2) * size;

    while (left <= right) {
        while (cmp(left, pivot) < 0) { left += size; }
        while (cmp(right, pivot) > 0) { right -= size; }
        if (left <= right) {
            _swap(left, right, size);
            left += size;
            right -= size;
        }
    }

    _qsort(base, (right - base) / size + 1, size, cmp);
    _qsort(left, (base + count * size - left) / size, size, cmp);
}

#endif

#if CONFIG_USING_COLOR

void RGB2HSV(u8 r, u8 g, u8 b, f32* h, f32* s, f32* v)
{
    f32 cmax  = MAX3(r, g, b);
    f32 cmin  = MIN3(r, g, b);
    f32 delta = cmax - cmin;

    *v = cmax / 255.0 * 100.0;
    *s = cmax ? (delta / cmax) : 0;

    if (delta == 0) {
        *h = 0;
    } else if (cmax == r) {
        *h = 60 * ((g - b) / delta);
    } else if (cmax == g) {
        *h = 60 * ((b - r) / delta + 2);
    } else {
        *h = 60 * ((r - g) / delta + 4);
    }

    if (*h < 0) {
        *h += 360;
    }
}

#endif
