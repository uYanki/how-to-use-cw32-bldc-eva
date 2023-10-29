#include "font.h"

//-------------------------------------
// fonts

#if (CONFIG_FONTSRC_CONSLONS_8X16 != FONT_SRC_NONE)
sFONT Font8x16 = {
    .width  = 8,
    .height = 16,
    .source = CONFIG_FONTSRC_CONSLONS_8X16,
#if (CONFIG_FONTSRC_CONSLONS_8X16 == FONT_SRC_FLASH)
    .address = (void*)ASCII8x16_Table,
#elif CONFIG_FONTSRC_CONSLONS_8X16 == FONT_SRC_EXTFLASH)
    .address = 10 * 1024,
#elif (CONFIG_FONTSRC_CONSLONS_8X16 == FONT_SRC_SDCARD)
    .address = "0:/System/Fonts/consolas8x16.FON",
#else
#error "unsupported font scource"
#endif
};
#endif

#if (CONFIG_FONTSRC_CONSLONS_16X24 != FONT_SRC_NONE)
sFONT Font16x24 = {
    .width  = 16,
    .height = 24,
    .source = CONFIG_FONTSRC_CONSLONS_16X24,
#if (CONFIG_FONTSRC_CONSLONS_16X24 == FONT_SRC_FLASH)
    .address = (void*)ASCII16x24_Table,
#elif (CONFIG_FONTSRC_CONSLONS_16X24 == FONT_SRC_EXTFLASH)
    .address = 12 * 1024,
#elif (CONFIG_FONTSRC_CONSLONS_16X24 == FONT_SRC_SDCARD)
    .address = "0:/System/Fonts/consolas16x24.FON",
#else
#error "unsupported font scource"
#endif
};
#endif

#if (CONFIG_FONTSRC_CONSLONS_24X32 != FONT_SRC_NONE)
sFONT Font24x32 = {
    .width  = 24,
    .height = 32,
    .source = CONFIG_FONTSRC_CONSLONS_24X32,
#if (CONFIG_FONTSRC_CONSLONS_24X32 == FONT_SRC_FLASH)
    .address = (void*)ASCII24x32_Table,
#elif (CONFIG_FONTSRC_CONSLONS_24X32 == FONT_SRC_EXTFLASH)
    .address = 17 * 1024,
#elif (CONFIG_FONTSRC_CONSLONS_24X32 == FONT_SRC_SDCARD)
    .address = "0:/System/Fonts/consolas24x32.FON",
#else
#error "unsupported font scource"
#endif
};
#endif

#if (CONFIG_FONTSRC_GB2312 != FONT_SRC_NONE)
sFONT GB2312_H1616 = {
    .width  = 16,
    .height = 16,
    .source = CONFIG_FONTSRC_GB2312,
#if (CONFIG_FONTSRC_GB2312 == FONT_SRC_EXTFLASH)
    .address = 387 * 4096,
#elif (CONFIG_FONTSRC_GB2312 == FONT_SRC_SDCARD)
    .address = "0:/System/Fonts/consolas32x32.FON",
#else
#error "unsupported font scource"
#endif
};
#endif

#if (CONFIG_FONTSRC_HZLIB != FONT_SRC_NONE)
// 0��HZLIB��ģ(�ɰ�)
sFONT GB2312_HZLIB = {
    .width  = 16,
    .height = 16,
    .source = CONFIG_FONTSRC_HZLIB,
#if (CONFIG_FONTSRC_HZLIB == FONT_SRC_EXTFLASH)
    .address = 1 * 4096,
#elif (CONFIG_FONTSRC_HZLIB == FONT_SRC_SDCARD)
    .address = "0:/System/Fonts/hzlib.FON",
#else
#error "unsupported font scource"
#endif
};

#endif

//-------------------------------------
// functions

uint8_t GetConslonsCode(uint8_t* pBuffer, char chr, sFONT* Fonts)
{
    // Ҫ��ȡ�ֽ�����
    uint8_t  nCount  = (Fonts->width * Fonts->height) >> 3;
    // ��ģƫ�Ƶ�ַ
    uint16_t nOffset = (chr - ' ') * nCount;
    // ��ģ��ʼ��ַ
    uint8_t* pTable  = (uint8_t*)(Fonts->address) + nOffset;
    switch (Fonts->source)
    {
#if FONT_SRC_FLASH
        case FONT_SRC_FLASH: {
            // ��ȡ��ģ
            while (nCount--)
            {
                *pBuffer++ = *pTable++;
            }
            return true;
        }
#endif
#if FONT_SRC_EXTFLASH
        case FONT_SRC_EXTFLASH: {
            // ��ȡ��ģ
            SPI_FLASH_BufferRead(pBuffer, pTable, nCount);
            return true;  // ���߷��ش�����
        }
#endif
#if FONT_SRC_SDCARD
        case FONT_SRC_SDCARD: {
            return false;
        }
#endif
        default:
        case FONT_SRC_NONE: {
            // pass
            return false;
        }
    }
}

uint8_t GetGBKCode(uint8_t* pBuffer, char chr, sFONT* Fonts)
{
    //  uint32_t nOffset = (((chr >> 8) - 0xa0 - 16) * 94 + (chr & 0xFF) - 0xa1) * nCount; // HZLIB

    // Ҫ��ȡ�ֽ�����
    uint8_t  nCount  = (Fonts->width * Fonts->height) >> 3;
    // ��ģƫ�Ƶ�ַ
    uint32_t nOffset = (((chr >> 8) - 0xa1) * 94 + (chr & 0xFF) - 0xa1) * nCount;
    // ��ģ��ʼ��ַ
    uint32_t pTable  = (uint32_t)(Fonts->address) + nOffset;

    switch (Fonts->source)
    {
#if FONT_SRC_FLASH
        case FONT_SRC_FLASH: {
            return false;
        }
#endif
#if FONT_SRC_EXTFLASH
        case FONT_SRC_EXTFLASH: {
            // ��ȡ��ģ
            SPI_FLASH_BufferRead(pBuffer, pTable, nOffset);
            return true;
        }
#endif
#if FONT_SRC_SDCARD
        case FONT_SRC_SDCARD: {
            static FIL     fnew; /* file objects */
            static FATFS   fs;   /* Work area (file system object) for logical drives */
            static FRESULT res_sd;
            static UINT    br; /* File R/W count */
            static uint8_t everRead = 0;

            // ��ȡ��ģ
            if (everRead == 0)
            {
                // ��һ��ʹ�ã������ļ�ϵͳ����ʼ��sd
                res_sd   = f_mount(&fs, "0:", 1);
                everRead = 1;
            }
            res_sd = f_open(&fnew, CONFIG_GBKCODE_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
            if (res_sd == FR_OK)
            {
                f_lseek(&fnew, nOffset);              // ָ��ƫ��
                f_read(&fnew, pBuffer, nCount, &br);  // 16*16��С�ĺ��� ����ģ ռ��16*16/8���ֽ�
                f_close(&fnew);
                return true;
            }
            else
            {
                return false;
            }
            break;
        }
#endif
        default:
        case FONT_SRC_NONE: {
            return false;
        }
    }
}
