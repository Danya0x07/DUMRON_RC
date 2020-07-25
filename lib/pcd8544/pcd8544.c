/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

#include "pcd8544.h"
#include "pcd8544_port.h"

#define NUM_PIXELS_X    84
#define NUM_PIXELS_Y    48
#define NUM_PAGES   6

#define X_MAX       (NUM_PIXELS_X - 1)
#define Y_MAX       (NUM_PIXELS_Y - 1)
#define PAGE_MAX    (NUM_PAGES - 1)

enum pcd8544_cmd {
    CMD_NOP = 0x00,
    CMD_FUNCTION_SET = 0x20,
    CMD_DISPLAY_CTRL = 0x08,
    CMD_SET_PAGE = 0x40,
    CMD_SET_X = 0x80,
    CMD_TEMPERATURE_CTRL = 0x04,
    CMD_BIAS_SYSTEM = 0x10,
    CMD_SET_VOP = 0x80
};

#define FUNCTION_SET(PD, V, H)  (CMD_FUNCTION_SET | (PD) << 2 | (V) << 1 | (H))

static const LOOKUP_TABLE_MEMORY_SPECIFIER uint8_t font_table[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00},  /* 20 SPACE */
    {0x00, 0x00, 0x5f, 0x00, 0x00},  /* 21 ! */
    {0x00, 0x07, 0x00, 0x07, 0x00},  /* 22 " */
    {0x14, 0x7f, 0x14, 0x7f, 0x14},  /* 23 # */
    {0x24, 0x2a, 0x7f, 0x2a, 0x12},  /* 24 $ */
    {0x23, 0x13, 0x08, 0x64, 0x62},  /* 25 % */
    {0x36, 0x49, 0x55, 0x22, 0x50},  /* 26 & */
    {0x00, 0x05, 0x03, 0x00, 0x00},  /* 27 ' */
    {0x00, 0x1c, 0x22, 0x41, 0x00},  /* 28 ( */
    {0x00, 0x41, 0x22, 0x1c, 0x00},  /* 29 ) */
    {0x14, 0x08, 0x3e, 0x08, 0x14},  /* 2a * */
    {0x08, 0x08, 0x3e, 0x08, 0x08},  /* 2b + */
    {0x00, 0x50, 0x30, 0x00, 0x00},  /* 2c , */
    {0x08, 0x08, 0x08, 0x08, 0x08},  /* 2d - */
    {0x00, 0x60, 0x60, 0x00, 0x00},  /* 2e . */
    {0x20, 0x10, 0x08, 0x04, 0x02},  /* 2f / */
    {0x3e, 0x51, 0x49, 0x45, 0x3e},  /* 30 0 */
    {0x00, 0x42, 0x7f, 0x40, 0x00},  /* 31 1 */
    {0x42, 0x61, 0x51, 0x49, 0x46},  /* 32 2 */
    {0x21, 0x41, 0x45, 0x4b, 0x31},  /* 33 3 */
    {0x18, 0x14, 0x12, 0x7f, 0x10},  /* 34 4 */
    {0x27, 0x45, 0x45, 0x45, 0x39},  /* 35 5 */
    {0x3c, 0x4a, 0x49, 0x49, 0x30},  /* 36 6 */
    {0x01, 0x71, 0x09, 0x05, 0x03},  /* 37 7 */
    {0x36, 0x49, 0x49, 0x49, 0x36},  /* 38 8 */
    {0x06, 0x49, 0x49, 0x29, 0x1e},  /* 39 9 */
    {0x00, 0x36, 0x36, 0x00, 0x00},  /* 3a : */
    {0x00, 0x56, 0x36, 0x00, 0x00},  /* 3b ; */
    {0x08, 0x14, 0x22, 0x41, 0x00},  /* 3c < */
    {0x14, 0x14, 0x14, 0x14, 0x14},  /* 3d = */
    {0x00, 0x41, 0x22, 0x14, 0x08},  /* 3e > */
    {0x02, 0x01, 0x51, 0x09, 0x06},  /* 3f ? */
    {0x32, 0x49, 0x79, 0x41, 0x3e},  /* 40 @ */
    {0x7e, 0x11, 0x11, 0x11, 0x7e},  /* 41 A */
    {0x7f, 0x49, 0x49, 0x49, 0x36},  /* 42 B */
    {0x3e, 0x41, 0x41, 0x41, 0x22},  /* 43 C */
    {0x7f, 0x41, 0x41, 0x22, 0x1c},  /* 44 D */
    {0x7f, 0x49, 0x49, 0x49, 0x41},  /* 45 E */
    {0x7f, 0x09, 0x09, 0x09, 0x01},  /* 46 F */
    {0x3e, 0x41, 0x49, 0x49, 0x7a},  /* 47 G */
    {0x7f, 0x08, 0x08, 0x08, 0x7f},  /* 48 H */
    {0x00, 0x41, 0x7f, 0x41, 0x00},  /* 49 I */
    {0x20, 0x40, 0x41, 0x3f, 0x01},  /* 4a J */
    {0x7f, 0x08, 0x14, 0x22, 0x41},  /* 4b K */
    {0x7f, 0x40, 0x40, 0x40, 0x40},  /* 4c L */
    {0x7f, 0x02, 0x0c, 0x02, 0x7f},  /* 4d M */
    {0x7f, 0x04, 0x08, 0x10, 0x7f},  /* 4e N */
    {0x3e, 0x41, 0x41, 0x41, 0x3e},  /* 4f O */
    {0x7f, 0x09, 0x09, 0x09, 0x06},  /* 50 P */
    {0x3e, 0x41, 0x51, 0x21, 0x5e},  /* 51 Q */
    {0x7f, 0x09, 0x19, 0x29, 0x46},  /* 52 R */
    {0x46, 0x49, 0x49, 0x49, 0x31},  /* 53 S */
    {0x01, 0x01, 0x7f, 0x01, 0x01},  /* 54 T */
    {0x3f, 0x40, 0x40, 0x40, 0x3f},  /* 55 U */
    {0x1f, 0x20, 0x40, 0x20, 0x1f},  /* 56 V */
    {0x3f, 0x40, 0x38, 0x40, 0x3f},  /* 57 W */
    {0x63, 0x14, 0x08, 0x14, 0x63},  /* 58 X */
    {0x07, 0x08, 0x70, 0x08, 0x07},  /* 59 Y */
    {0x61, 0x51, 0x49, 0x45, 0x43},  /* 5a Z */
    {0x00, 0x7f, 0x41, 0x41, 0x00},  /* 5b [ */
    {0x02, 0x04, 0x08, 0x10, 0x20},  /* 5c \ */
    {0x00, 0x41, 0x41, 0x7f, 0x00},  /* 5d ] */
    {0x04, 0x02, 0x01, 0x02, 0x04},  /* 5e ^ */
    {0x40, 0x40, 0x40, 0x40, 0x40},  /* 5f _ */
    {0x00, 0x01, 0x02, 0x04, 0x00},  /* 60 ` */
    {0x20, 0x54, 0x54, 0x54, 0x78},  /* 61 a */
    {0x7f, 0x48, 0x44, 0x44, 0x38},  /* 62 b */
    {0x38, 0x44, 0x44, 0x44, 0x20},  /* 63 c */
    {0x38, 0x44, 0x44, 0x48, 0x7f},  /* 64 d */
    {0x38, 0x54, 0x54, 0x54, 0x18},  /* 65 e */
    {0x08, 0x7e, 0x09, 0x01, 0x02},  /* 66 f */
    {0x0c, 0x52, 0x52, 0x52, 0x3e},  /* 67 g */
    {0x7f, 0x08, 0x04, 0x04, 0x78},  /* 68 h */
    {0x00, 0x44, 0x7d, 0x40, 0x00},  /* 69 i */
    {0x20, 0x40, 0x44, 0x3d, 0x00},  /* 6a j */
    {0x7f, 0x10, 0x28, 0x44, 0x00},  /* 6b k */
    {0x00, 0x41, 0x7f, 0x40, 0x00},  /* 6c l */
    {0x7c, 0x04, 0x18, 0x04, 0x78},  /* 6d m */
    {0x7c, 0x08, 0x04, 0x04, 0x78},  /* 6e n */
    {0x38, 0x44, 0x44, 0x44, 0x38},  /* 6f o */
    {0x7c, 0x14, 0x14, 0x14, 0x08},  /* 70 p */
    {0x08, 0x14, 0x14, 0x18, 0x7c},  /* 71 q */
    {0x7c, 0x08, 0x04, 0x04, 0x08},  /* 72 r */
    {0x48, 0x54, 0x54, 0x54, 0x20},  /* 73 s */
    {0x04, 0x3f, 0x44, 0x40, 0x20},  /* 74 t */
    {0x3c, 0x40, 0x40, 0x20, 0x7c},  /* 75 u */
    {0x1c, 0x20, 0x40, 0x20, 0x1c},  /* 76 v */
    {0x3c, 0x40, 0x30, 0x40, 0x3c},  /* 77 w */
    {0x44, 0x28, 0x10, 0x28, 0x44},  /* 78 x */
    {0x0c, 0x50, 0x50, 0x50, 0x3c},  /* 79 y */
    {0x44, 0x64, 0x54, 0x4c, 0x44},  /* 7a z */
    {0x00, 0x08, 0x36, 0x41, 0x00},  /* 7b { */
    {0x00, 0x00, 0x7f, 0x00, 0x00},  /* 7c | */
    {0x00, 0x41, 0x36, 0x08, 0x00},  /* 7d } */
    {0x10, 0x08, 0x08, 0x10, 0x08},  /* 7e ~ */
};

#if (PCD8544_USE_FRAMEBUFFER > 0)
static uint8_t framebuffer[NUM_PAGES][NUM_PIXELS_X];
#endif

static struct {
    bool inverse;
    uint8_t font_size;
    uint8_t image_scale;
} brush = {FALSE, 1, 1};

static struct {
    uint8_t x;
    uint8_t page;
    uint8_t y;
} cursor;

static void write_cmd(uint8_t cmd)
{
    _ce_low();
    _dc_low();
    _spi_send_byte(cmd);
    _ce_high();
}

static void write_ddram(uint8_t data)
{
    _ce_low();
    _dc_high();
    _spi_send_byte(data);
    _ce_high();
}

static void write_dd(uint8_t data)
{
    if (brush.inverse)
        data = ~data;

#if (PCD8544_USE_FRAMEBUFFER > 0)
    framebuffer[cursor.page][cursor.x] = data;
#else
    write_ddram(data);
#endif

    if (++cursor.x > X_MAX) {
        cursor.x = 0;
        if (++cursor.page > PAGE_MAX)
            cursor.page = 0;
    }
}

#if (PCD8544_USE_FRAMEBUFFER > 0)

static void write_pixel(uint8_t x, uint8_t y, bool status)
{
    uint8_t page;
    uint8_t bit_no;

    if (x >= NUM_PIXELS_X)
        x %= NUM_PIXELS_X;

    if (y >= NUM_PIXELS_Y)
        y %= NUM_PIXELS_Y;

    page = y >> 3;
    bit_no = y & 0x07;

    framebuffer[page][x] &= ~(!status << bit_no);
    framebuffer[page][x] |= status << bit_no;
}

void pcd8544_update(void)
{
    uint_fast8_t i, j;

    pcd8544_set_addr(0, 0);
    for (i = 0; i < NUM_PAGES; i++) {
        for (j = 0; j < NUM_PIXELS_X; j++) {
            write_ddram(framebuffer[i][j]);
        }
    }
}
#endif

void pcd8544_reset(void)
{
    _rst_low();
    _delay_ms(1);
    _rst_high();
}

void pcd8544_set_power(bool pwr)
{
    write_cmd(FUNCTION_SET(!pwr, 0, 0));
}

void pcd8544_configure(struct pcd8544_config *config)
{
    write_cmd(FUNCTION_SET(0, 0, 1));
    write_cmd(CMD_BIAS_SYSTEM | config->contrast);
    write_cmd(CMD_TEMPERATURE_CTRL | config->temperature_coeff);
    write_cmd(CMD_SET_VOP | config->brightness);
    write_cmd(FUNCTION_SET(0, 0, 0));
    write_cmd(CMD_DISPLAY_CTRL | PCD8544_MODE_NORMAL);
}

void pcd8544_set_mode(enum pcd8544_mode mode)
{
    write_cmd(CMD_DISPLAY_CTRL | mode);
}

void pcd8544_set_brush(bool inverse, uint8_t font_size, uint8_t image_scale)
{
    if (font_size > 3)
        font_size = 3;
    if (image_scale > 6)
        image_scale = 6;

    brush.inverse = inverse;
    brush.font_size = font_size;
    brush.image_scale = image_scale;
}

void pcd8544_set_cursor(uint8_t column, uint8_t row)
{
    pcd8544_set_addr(6 * column * brush.font_size, row * brush.font_size);
}

void pcd8544_set_addr(uint8_t x, uint8_t page)
{
    if (x >= NUM_PIXELS_X)
        x %= NUM_PIXELS_X;

    if (page >= NUM_PAGES)
        page %= NUM_PAGES;

    cursor.x = x;
    cursor.page = page;
    cursor.y = 8 * page;

    write_cmd(CMD_SET_X | cursor.x);
    write_cmd(CMD_SET_PAGE | cursor.page);
}

static void draw(uint8_t *data, uint8_t length,
                 uint8_t x_scale, uint8_t y_scale, uint8_t *buffer)
{
    uint_fast8_t i, j;
    bool bit_status;
    uint8_t idx;
    uint8_t bit_no;

    /*
     * buffer[length * y_scale]
     */
    uint8_t start_x = cursor.x;
    uint8_t start_page = cursor.page;

    bit_no = 0;
    for (i = 0; i < 8 * length; i++) {
        bit_status = data[i >> 3] & 0x01;
        for (j = 0; j < y_scale; j++) {
            idx = bit_no >> 3;
            buffer[idx] |= bit_status << (bit_no & 0x07);
            bit_no++;
        }
        data[i >> 3] >>= 1;
    }

    for (i = 0; i < length * y_scale; i++) {
        pcd8544_set_addr(start_x, start_page + i);
        for (j = 0; j < x_scale; j++) {
            write_dd(buffer[i]);
        }
    }
    pcd8544_set_addr(start_x + x_scale, start_page);
}

void pcd8544_print_c(char c)
{
    uint_fast8_t i;
    const uint8_t *ch_addr;
    uint8_t buffer[3] = {0};
    uint8_t data;

    c -= 0x20;
    ch_addr = (const uint8_t *)font_table + 5 * c;

    for (i = 0; i < 5; i++) {
        data = _lookup(ch_addr + i);
        draw(&data, 1, brush.font_size, brush.font_size, buffer);
    }

    data = 0x00;
    draw(&data, 1, brush.font_size, brush.font_size, buffer);
}

void pcd8544_print_s(const char *s)
{
    while(*s) {
        pcd8544_print_c(*s++);
    }
}

void pcd8544_draw_img(uint8_t x, uint8_t page, const struct pcd8544_image *img)
{
    const uint8_t *bitmap = img->bitmap;
    uint_fast8_t current_x;
    uint8_t tmp;

    tmp = page + img->height_pg;
    const uint8_t end_pg = tmp > NUM_PAGES ? NUM_PAGES : tmp;

    tmp = x + img->width_px;
    const uint_fast8_t end_x = tmp > NUM_PIXELS_X ? NUM_PIXELS_X : tmp;

    if (img->lookup) {
        for (; page < end_pg; page++) {
            pcd8544_set_addr(x, page);
            for (current_x = x; current_x < end_x; current_x++) {
                write_dd(_lookup(bitmap++));
            }
        }
    } else {
        for (; page < end_pg; page++) {
            pcd8544_set_addr(x, page);
            for (current_x = x; current_x < end_x; current_x++) {
                write_dd(*bitmap++);
            }
        }
    }
}

void pcd8544_draw_img_part(uint8_t x, uint8_t page,
                           const struct pcd8544_image *img,
                           uint8_t x0, uint8_t x1,
                           uint8_t y0, uint8_t y1)
{
    const uint8_t *bitmap;
    uint_fast8_t i, current_x;
    uint8_t end_x;
    uint8_t end_pg;
    uint8_t data;
    uint8_t buffer[6] = {0};

    uint8_t tmp;

    if (x0 > img->width_px)
        x0 = img->width_px;
    if (x1 > img->width_px)
        x1 = img->width_px;
    if (y0 > img->height_pg)
        y0 = img->height_pg;
    if (y1 > img->height_pg)
        y1 = img->height_pg;

    tmp = page + y1 - y0;
    end_pg = tmp > NUM_PAGES ? NUM_PAGES : tmp;

    tmp = x + x1 - x0;
    end_x = tmp > NUM_PIXELS_X ? NUM_PIXELS_X : tmp;

    bitmap = img->bitmap + y0 * img->width_px + x0;

    if (img->lookup) {
        for (i = 0; page < end_pg; page++, i++) {
            pcd8544_set_addr(x, page);
            bitmap = img->bitmap + (y0 + i) * img->width_px + x0;
            for (current_x = x; current_x < end_x; current_x++) {
                data = _lookup(bitmap++);
                draw(&data, y1 - y0, brush.image_scale, brush.image_scale, buffer);
            }
        }
    } else {
        for (i = 0; page < end_pg; page++, i++) {
            pcd8544_set_addr(x, page);
            bitmap = img->bitmap + (y0 + i) * img->width_px + x0;
            for (current_x = x; current_x < end_x; current_x++) {
                data = *bitmap++;
                draw(&data, y1 - y0, brush.image_scale, brush.image_scale, buffer);
            }
        }
    }
}

void pcd8544_clear(void)
{
    uint_fast16_t i;

    pcd8544_set_addr(0, 0);
    for (i = 0; i < NUM_PIXELS_X * NUM_PAGES; i++) {
        write_dd(0);
    }
}