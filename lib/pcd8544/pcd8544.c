/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

#include "pcd8544.h"
#include "pcd8544_port.h"
#include "pcd8544_font.h"

#define NUM_PIXELS_X    84
#define NUM_PIXELS_Y    48
#define NUM_PAGES   6

#define X_MAX       (NUM_PIXELS_X - 1)
#define Y_MAX       (NUM_PIXELS_Y - 1)
#define PAGE_MAX    (NUM_PAGES - 1)

#define CHAR_WIDTH_PX   6

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

#define FS_PD   (1 << 2)
#define FS_V    (1 << 1)
#define FS_H    (1 << 0)

#if (PCD8544_USE_FRAMEBUFFER == 1)

static uint8_t framebuffer[NUM_PAGES][NUM_PIXELS_X];

static struct fb_page_boundaries {
    uint8_t start;
    uint8_t end;
} update_bd[NUM_PAGES];
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

#if (PCD8544_USE_FRAMEBUFFER == 1)
    framebuffer[cursor.page][cursor.x] = data;

    if (cursor.x < update_bd[cursor.page].start)
        update_bd[cursor.page].start = cursor.x;
    else if (cursor.x >= update_bd[cursor.page].end)
        update_bd[cursor.page].end = cursor.x + 1;
#else
    write_ddram(data);
#endif

    if (++cursor.x > X_MAX) {
        cursor.x = 0;
        if (++cursor.page > PAGE_MAX)
            cursor.page = 0;
    }
}

static void set_addr(uint8_t x, uint8_t page)
{
    if (x >= NUM_PIXELS_X)
        x -= NUM_PIXELS_X;
    if (page >= NUM_PAGES)
        page -= NUM_PAGES;

    cursor.x = x;
    cursor.page = page;
    cursor.y = 8 * page;

    write_cmd(CMD_SET_X | cursor.x);
    write_cmd(CMD_SET_PAGE | cursor.page);
}

void pcd8544_reset(void)
{
    _rst_low();
    _delay_ms(1);
    _rst_high();
}

void pcd8544_configure(struct pcd8544_config *config)
{
    config->bias &= 0x07;
    config->contrast &= 0x7F;
    config->tempcoeff &= 0x03;

    write_cmd(CMD_FUNCTION_SET | FS_H);
    write_cmd(CMD_BIAS_SYSTEM | config->bias);
    write_cmd(CMD_TEMPERATURE_CTRL | config->tempcoeff);
    write_cmd(CMD_SET_VOP | config->contrast);
    write_cmd(CMD_FUNCTION_SET);
}

void pcd8544_set_power(bool pwr)
{
    if (!pwr) {
        uint_fast8_t i, j;

        set_addr(0, 0);
        for (i = 0; i < NUM_PAGES; i++) {
            for (j = 0; j < NUM_PIXELS_X; j++) {
                write_ddram(0x00);
            }
            update_bd[i].start = 0;
            update_bd[i].end = NUM_PIXELS_X;
        }
        write_cmd(CMD_FUNCTION_SET | FS_PD);
    } else {
        write_cmd(CMD_FUNCTION_SET);
    }
}

void pcd8544_set_mode(enum pcd8544_mode mode)
{
    write_cmd(CMD_DISPLAY_CTRL | mode);
}

void pcd8544_setup_brush(bool inverse, uint8_t font_size, uint8_t image_scale)
{
    if (font_size > 3)
        font_size = 3;
    else if (font_size == 0)
        font_size = 1;

    if (image_scale > 6)
        image_scale = 6;
    else if (image_scale == 0)
        image_scale = 1;

    brush.inverse = inverse;
    brush.font_size = font_size;
    brush.image_scale = image_scale;
}

void pcd8544_set_cursor(uint8_t col, uint8_t row)
{
    set_addr(CHAR_WIDTH_PX * col * brush.font_size,
                     row * brush.font_size);
}

static void draw_byte(uint8_t data, uint8_t x_scale, uint8_t y_scale)
{
    static uint8_t buffer[NUM_PAGES];
    const uint8_t start_x = cursor.x;
    const uint8_t start_pg = cursor.page;
    uint8_t current_x = start_x;
    uint8_t current_pg = start_pg;
    bool bit_status;
    uint8_t bit_no;
    uint_fast8_t i, j;

    for (i = 0; i < y_scale; i++)
        buffer[i] = 0;

    bit_no = 0;
    for (i = 0; i < 8 && bit_no < NUM_PIXELS_Y; i++) {
        bit_status = data & 0x01;
        for (j = 0; j < y_scale; j++) {
            buffer[bit_no >> 3] |= bit_status << (bit_no & 0x07);
            bit_no++;
        }
        data >>= 1;
    }

    for (i = 0; i < y_scale; i++) {
        current_x = start_x;
        set_addr(current_x, current_pg);
        for (j = 0; j < x_scale; j++) {
            write_dd(buffer[i]);
            if (++current_x > X_MAX)
                break;
        }
        if (++current_pg > PAGE_MAX)
            break;
    }

    set_addr(current_x, start_pg);
}

void pcd8544_print_c(char c)
{
    const uint8_t *ch_addr;
    uint_fast8_t i;

    c = _get_character_bitmap_index(c);
    ch_addr = (const uint8_t *)&font_table[c];

    for (i = 0; i < 5; i++) {
        draw_byte(_lookup(ch_addr + i), brush.font_size, brush.font_size);
    }

    draw_byte(0x00, brush.font_size, brush.font_size);
}

void pcd8544_print_s(const char *s)
{
    pcd8544_print_s_f(0, NUM_PIXELS_X, PAGE_MAX, s);
}

void pcd8544_print_s_f(uint8_t x0, uint8_t x1, uint8_t p1, const char *s)
{
    uint_fast8_t current_pg = cursor.page;
    uint_fast8_t char_places_per_row =
            x1 / CHAR_WIDTH_PX / brush.font_size;
    uint_fast8_t i = cursor.x / CHAR_WIDTH_PX / brush.font_size;

    while(*s) {
        pcd8544_print_c(*s++);
        if (++i >= char_places_per_row) {
            i = x0 / CHAR_WIDTH_PX / brush.font_size;
            current_pg += brush.font_size;
            if (current_pg > p1)
                break;
            set_addr(x0, current_pg);
            /*
             * There is no point in printing a space at the beginning
             * of the wrapped line.
             */
            if (*s == ' ')
                s++;
        }
    }
}

void pcd8544_draw_img(uint8_t x0, uint8_t p0, const struct pcd8544_image *img)
{
    const uint8_t *bitmap = img->bitmap;
    uint_fast8_t x, pg = p0;
    uint_fast8_t i, j;

    for (i = 0; i < img->height_pg; i++) {
        x = x0;
        set_addr(x, pg);
        if (img->lookup){
            for (j = 0; j < img->width_px; j++) {
                draw_byte(_lookup(bitmap++), brush.image_scale,
                          brush.image_scale);
                x += brush.image_scale;
                if (x > X_MAX)
                    break;
            }
        } else {
            for (j = 0; j < img->width_px; j++) {
                draw_byte(*bitmap++, brush.image_scale, brush.image_scale);
                x += brush.image_scale;
                if (x > X_MAX)
                    break;
            }
        }
        bitmap += img->width_px - j - (x >= NUM_PIXELS_X);
        pg += brush.image_scale;
        if (pg > PAGE_MAX)
            break;
    }
}

void pcd8544_fill_area(uint8_t x0, uint8_t p0, uint8_t x1, uint8_t p1)
{
    uint_fast8_t i, j;

    if (x1 > X_MAX)
        x1 = X_MAX;
    if (p1 > PAGE_MAX)
        p1 = PAGE_MAX;

    for (i = p0; i <= p1; i++) {
        set_addr(x0, i);
        for (j = x0; j <= x1; j++) {
            write_dd(0x00);
        }
    }
}

void pcd8544_clear(void)
{
    uint_fast16_t i;

    set_addr(0, 0);
    for (i = 0; i < NUM_PIXELS_X * NUM_PAGES; i++) {
        write_dd(0);
    }
}

#if (PCD8544_USE_FRAMEBUFFER == 1)

void pcd8544_update(void)
{
    uint_fast8_t i, j;

    for (i = 0; i < NUM_PAGES; i++) {
        if (update_bd[i].start == update_bd[i].end)
            continue;
        set_addr(update_bd[i].start, i);
        for (j = update_bd[i].start; j < update_bd[i].end; j++) {
            write_ddram(framebuffer[i][j]);
        }
        update_bd[i].start = update_bd[i].end = NUM_PIXELS_X / 2;
    }
}

void pcd8544_write_pixel(uint8_t x, uint8_t y, bool state)
{
    uint8_t page;
    uint8_t bit_no;

    if (x >= NUM_PIXELS_X)
        x -= NUM_PIXELS_X;

    if (y >= NUM_PIXELS_Y)
        y -= NUM_PIXELS_Y;

    page = y >> 3;
    bit_no = y & 0x07;

    framebuffer[page][x] &= ~(!state << bit_no);
    framebuffer[page][x] |= state << bit_no;
}

void pcd8544_draw_hline(uint8_t y, uint8_t x0, uint8_t x1)
{
    uint_fast8_t i;
    uint8_t pg = y >> 3;

    set_addr(x0, pg);
    y &= 0x07;
    for (i = x0; i <= x1; i++) {
        write_dd(framebuffer[pg][i] | 1 << y);
    }
}

void pcd8544_draw_vline(uint8_t x, uint8_t y0, uint8_t y1)
{
    uint_fast8_t i;
    uint_fast8_t p0 = y0 >> 3;
    uint_fast8_t p1 = y1 >> 3;
    uint_fast8_t shift_l, shift_r;

    for (i = p0; i <= p1; i++) {
        set_addr(x, i);
        shift_l = (y0 & 0x07) * (i == p0);
        shift_r = (7 - (y1 & 0x07)) * (i == p1);
        write_dd(framebuffer[i][x] | ((0xFF << shift_l) & (0xFF >> shift_r)));
    }
}
#endif