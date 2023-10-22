
#include "oled_i2c.h"

#include "font.h"

sFONT* oled_font = &Font8x16;

uint8_t oled_gram[OLED_SCREEN_H][OLED_SCREEN_W];

void I2C_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStruct;

    __RCC_I2C1_CLK_ENABLE();
    __RCC_GPIOB_CLK_ENABLE();

    PB06_AFx_I2C1SCL();
    PB07_AFx_I2C1SDA();

    GPIO_InitStructure.Pins  = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);

    I2C_InitStruct.I2C_BaudEn = ENABLE;
    I2C_InitStruct.I2C_Baud   = 0x08;
    I2C_InitStruct.I2C_FLT    = DISABLE;
    I2C_InitStruct.I2C_AA     = DISABLE;

    I2C1_DeInit();
    I2C_Master_Init(CW_I2C1, &I2C_InitStruct);
    I2C_Cmd(CW_I2C1, ENABLE);
}

void I2C_MasterWrite(I2C_TypeDef* i2c, uint8_t dev, uint8_t reg, uint8_t* dat, uint32_t len)
{
    uint32_t cnt = 0, stat;
    I2C_GenerateSTART(i2c, ENABLE);  // send slave+w
    while (1) {
        while (0 == I2C_GetIrq(i2c)) {}
        stat = I2C_GetState(i2c);
        switch (stat) {
                // after send START signal
            case 0x08:
                I2C_GenerateSTART(i2c, DISABLE);
                I2C_Send7bitAddress(i2c, OLED_DEV, 0x00);  // send dev addr
                break;
                // recv ACK after send SLAVE+W signal
            case 0x18:
                I2C_SendData(i2c, reg);  // send reg addr
                break;
                // recv ACK after send DATA
            case 0x28:
                I2C_SendData(i2c, dat[cnt++]);
                break;
                // recv NACK after send SLAVE+W signal
            case 0x20:
                // timeout after send SLAVE+W signal or send DATA
                // recv NACK or timeout after send SLAVE+R signal
            case 0x38:
                I2C_GenerateSTART(i2c, ENABLE);
                break;
                // recv NACK
            case 0x30:
                I2C_GenerateSTOP(i2c, ENABLE);
                break;
            default:
                break;
        }
        if (cnt > len) {
            I2C_GenerateSTOP(i2c, ENABLE);
            I2C_ClearIrq(i2c);
            break;
        }
        I2C_ClearIrq(i2c);
    }
}

void oled_write_cmd(uint8_t cmd)
{
    I2C_MasterWrite(OLED_I2C, OLED_DEV, 0x00, &cmd, 1);
}

void oled_write_dat(uint8_t dat)
{
    I2C_MasterWrite(OLED_I2C, OLED_DEV, 0x40, &dat, 1);
}

void oled_display_on(void)
{
    oled_write_cmd(0x8D);
    oled_write_cmd(0x14);
    oled_write_cmd(0xAF);
}

void oled_display_off(void)
{
    oled_write_cmd(0x8D);
    oled_write_cmd(0x10);
    oled_write_cmd(0xAF);
}

void oled_update(void)
{
    // set col
    oled_write_cmd(0x21);
    oled_write_cmd(0x00);
    oled_write_cmd(0x7f);
    // set row
    oled_write_cmd(0x22);
    oled_write_cmd(0x00);
    oled_write_cmd(0x07);

    // display
#if 1
    I2C_MasterWrite(OLED_I2C, OLED_DEV, 0x40, (uint8_t*)oled_gram, OLED_SCREEN_W * OLED_SCREEN_H);
#else
    uint8_t i, j;
    for (i = 0; i < OLED_SCREEN_H; ++i) {
        for (j = 0; j < OLED_SCREEN_W; ++j) {
            oled_write_dat(oled_gram[i][j]);
        }
    }
#endif
}

void oled_clear_line(uint8_t row, uint8_t xstart, uint8_t xend, oled_color_t cr)
{
    uint8_t x;

    for (x = xstart; x < xend; x++) {
        switch (cr) {
            case crNone: {
                break;
            }
            case crBLACK: {
                oled_gram[row][x] = 0;
                break;
            }
            case crWHITE: {
                oled_gram[row][x] = 0xFF;
                break;
            }
            case crXOR: {
                oled_gram[row][x] ^= oled_gram[row][x];
                break;
            }
        }
    }
}

void oled_clear(void)
{
    uint16_t i, j;

    for (i = 0; i < OLED_SCREEN_H; ++i) {
        for (j = 0; j < OLED_SCREEN_W; ++j) {
            oled_gram[i][j] = 0;
        }
    }
}

void oled_draw_point(uint8_t x, uint8_t y, oled_color_t cr)
{
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;

    switch (cr) {
        case crNone: {
            break;
        }
        case crBLACK: {
            oled_gram[i][x] &= ~n;
            break;
        }
        case crWHITE: {
            oled_gram[i][x] |= n;
            break;
        }
        case crXOR: {
            oled_gram[i][x] ^= n;
            break;
        }
    }
}

void oled_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, oled_color_t cr)
{
    int   i = 0;
    float k, b;
    if (x1 == x2)  // 画竖线
    {
        for (i = 0; i < (y2 - y1); ++i)
            oled_draw_point(x1, y1 + i, cr);
    } else if (y1 == y2)  // 画横线
    {
        for (i = 0; i < (x2 - x1); ++i)
            oled_draw_point(x1 + i, y1, cr);
    } else  // 画斜线
    {
        // 斜率*10 防止斜率小于1时失真
        k = (y2 - y1) * 10 / (x2 - x1);
        b = y1 - k * x1 / 10;
        while ((x1 + i) != x2) {
            oled_draw_point(x1 + i, (int)((x1 + i) * k / 10 + b), cr);
            if (x1 < x2) {
                i++;
            } else {
                i--;
            }
        }
    }
}

void oled_draw_circle(uint8_t x, uint8_t y, uint8_t r, oled_color_t cr)
{
    int a, b, d;
    int r2 = r * r;
    a      = 0;
    b      = r;
    while (2 * b * b >= r2) {
        oled_draw_point(x + a, y - b, cr);
        oled_draw_point(x - a, y - b, cr);
        oled_draw_point(x - a, y + b, cr);
        oled_draw_point(x + a, y + b, cr);

        oled_draw_point(x + b, y + a, cr);
        oled_draw_point(x + b, y - a, cr);
        oled_draw_point(x - b, y - a, cr);
        oled_draw_point(x - b, y + a, cr);

        ++a;
        d = (a * a + b * b) - r2;  // 离圆心距离
        if (d > 0) {
            --b;
            --a;
        }
    }
}

void oled_draw_char(uint8_t x, uint8_t y, char ch, oled_color_t cr_fore, oled_color_t cr_back)
{
    uint8_t i, m, temp, size = oled_font->height * oled_font->width / 8;
    uint8_t x0 = x;
    uint8_t Pfont[size];

    GetConslonsCode(Pfont, ch, oled_font);

    //	size=Fonttemp->Height*Fonttemp->Width/8;  //得到字体一个字符对应点阵集所占的字节数
    for (i = 0; i < size; i++) {
        temp = Pfont[i];
        for (m = 0; m < 8; m++)  // 写入数据
        {
            oled_draw_point(x, y, (temp & 0x80) ? cr_fore : cr_back);
            temp <<= 1;
            x++;
        }
        if ((x - x0) == (oled_font->width)) {
            x = x0;
            y++;
        }
    }
}

void oled_draw_char_zh(uint8_t x, uint8_t y, uint16_t ch, oled_color_t cr_fore, oled_color_t cr_back)
{
    uint8_t  row, bit;
    uint8_t  buf[WIDTH_CH_CHAR * HEIGHT_CH_CHAR / 8];
    uint16_t word;
    uint8_t  x0 = x;

    GetGBKCode(buf, ch);

    for (row = 0; row < HEIGHT_CH_CHAR; row++) {
        word = (buf[row * 2] << 8) | buf[row * 2 + 1];
        for (bit = 0; bit < WIDTH_CH_CHAR; bit++) {
            oled_draw_point(x++, y, (word & (0x8000 >> bit)) ? cr_fore : cr_back);
        }
        if (x - x0 == WIDTH_CH_CHAR) {
            x = x0;
            ++y;
        }
    }
}

/**
 * @brief  处理转义字符
 */
char* oled_str_esc_char(uint8_t* x, uint8_t* y, char* ch, oled_color_t cr_back)
{
    uint8_t TabSize;
    while (*ch < ' ' && *ch != '\0') {
        switch (*ch) {
                // 退格（BS）
            case '\b': {
                *x -= oled_font->width;
                break;
            }
                // 换行（LF）
            case '\n': {
                switch (oled_font->height) {
                    case 32: oled_clear_line((*y / 8) + 3, *x, 128, cr_back);    // 清除第3行
                    case 24: oled_clear_line((*y / 8) + 2, *x, 128, cr_back);    // 清除第2行
                    case 16: oled_clear_line((*y / 8) + 1, *x, 128, cr_back);    // 清除第1行
                    case 8: oled_clear_line((*y / 8), *x, 128, cr_back); break;  // 清除第0行
                }
                *x = 0;
                *y += oled_font->height;
                if (*y > (OLED_SCREEN_H * 8) - oled_font->height)  // 屏幕写满
                {
                    *y -= oled_font->height;
                }
                break;
            }
            // 回车（CR）
            case '\r': {
                switch (oled_font->height) {
                    case 32: oled_clear_line((*y / 8) + 3, *x, 128, cr_back);    // 清除第3行
                    case 24: oled_clear_line((*y / 8) + 2, *x, 128, cr_back);    // 清除第2行
                    case 16: oled_clear_line((*y / 8) + 1, *x, 128, cr_back);    // 清除第1行
                    case 8: oled_clear_line((*y / 8), *x, 128, cr_back); break;  // 清除第0行
                }
                *x = 0;
                *y = *y + oled_font->height;
                if (*y > (OLED_SCREEN_H * 8) - oled_font->height)  // 屏幕写满
                {
                    *y -= oled_font->height;
                }
                break;
            }
                // 水平制表（HT）
            case '\t': {
                TabSize = 4 * oled_font->width - (*x % (4 * oled_font->width));
                switch (oled_font->height) {
                    case 32: oled_clear_line((*y / 8) + 3, *x, *x + TabSize, cr_back);    // 清除第3行
                    case 24: oled_clear_line((*y / 8) + 2, *x, *x + TabSize, cr_back);    // 清除第2行
                    case 16: oled_clear_line((*y / 8) + 1, *x, *x + TabSize, cr_back);    // 清除第1行
                    case 8: oled_clear_line((*y / 8), *x, *x + TabSize, cr_back); break;  // 清除第0行
                }
                *x += TabSize;
                break;
            }
        }
        ch++;
    }
    return ch;
}

void oled_draw_str(uint8_t x, uint8_t y, char* str, oled_color_t cr_fore, oled_color_t cr_back)
{
    uint16_t chinese_chr;

    while (*str != '\0') {
        str = oled_str_esc_char(&x, &y, str, cr_back);
        if (*str == '\0') break;

        if (*str < 127) {  // ascii
            oled_draw_char(x, y, *str, cr_fore, cr_back);
            x += oled_font->width;
            if (x > 128 - oled_font->width) {
                // newline
                x = 0;
                y += oled_font->height;
            }
            str++;
        } else if (*str > 127) {  // chinese
            chinese_chr = *(uint16_t*)str;
            chinese_chr = (chinese_chr << 8) + (chinese_chr >> 8);
            oled_draw_char_zh(x, y, chinese_chr, cr_fore, cr_back);
            x += WIDTH_CH_CHAR;
            if (x > 128 - WIDTH_CH_CHAR) {
                // newline
                x = 0;
                y += HEIGHT_CH_CHAR;
            }
            str += 2;
        }
    }
}

// m^n
uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--) result *= m;
    return result;
}

void oled_draw_img(uint8_t x, uint8_t y, uint8_t imgw, uint8_t imgh, uint8_t* bmp)
{
    uint8_t x0 = x, y0 = y;
    for (y = y0; y < y0 + imgh; ++y) {
        for (x = x0; x < x0 + imgw; ++x) {
            oled_gram[y][x] = *bmp;
            bmp++;
        }
    }
}

void oled_init(void)
{
    I2C_init();

    delay1ms(200);

    oled_write_cmd(0xAE);  // display off

    oled_write_cmd(0x40);  // set low column address
    oled_write_cmd(0xB0);  // set high column address

    oled_write_cmd(0xC8);  // not offset

    oled_write_cmd(0x81);  // 设置对比度
    oled_write_cmd(0xff);

    oled_write_cmd(0xa1);  // 段重定向设置

    oled_write_cmd(0xa6);

    oled_write_cmd(0xa8);  // 设置驱动路数
    oled_write_cmd(0x1f);

    oled_write_cmd(0xd3);
    oled_write_cmd(0x00);

    oled_write_cmd(0xd5);
    oled_write_cmd(0xf0);

    oled_write_cmd(0xd9);
    oled_write_cmd(0x22);

    oled_write_cmd(0xda);
    oled_write_cmd(0x02);

    oled_write_cmd(0x20);  // Set Page Addressing Mode (0x00/0x01/0x02)
    // WriteCmd(0x02);     // 页地址模式
    oled_write_cmd(0x00);  // 水平寻址模式

    oled_write_cmd(0xdb);
    oled_write_cmd(0x49);

    oled_write_cmd(0x8d);
    oled_write_cmd(0x14);

    oled_write_cmd(0xaf);
}
