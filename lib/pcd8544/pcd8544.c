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

#define FUNCTION_SET(PD, V, H)  (CMD_FUNCTION_SET | (PD) << 2 | (V) << 1 | (H))

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

static struct {
    uint8_t start_x;
    uint8_t end_x;
} update_boundaries[NUM_PAGES];

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
    if (cursor.x < update_boundaries[cursor.page].start_x)
        update_boundaries[cursor.page].start_x = cursor.x;
    else if (cursor.x + 1 > update_boundaries[cursor.page].end_x)
        update_boundaries[cursor.page].end_x = cursor.x + 1;
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
        x -= NUM_PIXELS_X;

    if (y >= NUM_PIXELS_Y)
        y -= NUM_PIXELS_Y;

    page = y >> 3;
    bit_no = y & 0x07;

    framebuffer[page][x] &= ~(!status << bit_no);
    framebuffer[page][x] |= status << bit_no;
}
#endif

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

    write_cmd(FUNCTION_SET(0, 0, 1));
    write_cmd(CMD_BIAS_SYSTEM | config->bias);
    write_cmd(CMD_TEMPERATURE_CTRL | config->tempcoeff);
    write_cmd(CMD_SET_VOP | config->contrast);
    write_cmd(FUNCTION_SET(0, 0, 0));
}

void pcd8544_set_power(bool pwr)
{
    write_cmd(FUNCTION_SET(!pwr, 0, 0));
}

void pcd8544_set_mode(enum pcd8544_mode mode)
{
    write_cmd(CMD_DISPLAY_CTRL | mode);
}

void pcd8544_setup_brush(bool inverse, uint8_t font_size, uint8_t image_scale)
{
    if (font_size > 3)
        font_size = 3;
    if (image_scale > 6)
        image_scale = 6;

    brush.inverse = inverse;
    brush.font_size = font_size;
    brush.image_scale = image_scale;
}

void pcd8544_set_cursor(uint8_t col, uint8_t row)
{
    pcd8544_set_addr(CHAR_WIDTH_PX * col * brush.font_size,
                     row * brush.font_size);
}

void pcd8544_set_addr(uint8_t x, uint8_t page)
{
    if (x >= NUM_PIXELS_X) {
        x -= NUM_PIXELS_X;
    }
    if (page >= NUM_PAGES)
        page -= NUM_PAGES;

    cursor.x = x;
    cursor.page = page;
    cursor.y = 8 * page;

    write_cmd(CMD_SET_X | cursor.x);
    write_cmd(CMD_SET_PAGE | cursor.page);
}

static void draw_byte(uint8_t data, uint8_t x_scale, uint8_t y_scale)
{
    const uint8_t start_x = cursor.x;
    const uint8_t start_pg = cursor.page;
    uint8_t current_x = start_x;
    uint8_t current_pg = start_pg;
    uint8_t buffer[NUM_PAGES];
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
        pcd8544_set_addr(current_x, current_pg);
        for (j = 0; j < x_scale; j++) {
            write_dd(buffer[i]);
            if (++current_x > X_MAX)
                break;
        }
        if (++current_pg > PAGE_MAX)
            break;
    }

    pcd8544_set_addr(current_x, start_pg);
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

void pcd8544_print_s_f(uint8_t left_bd, uint8_t right_bd, uint8_t bottom_bd,
                       const char *s)
{
    uint_fast8_t current_pg = cursor.page;
    uint_fast8_t char_places_per_row =
            right_bd / CHAR_WIDTH_PX / brush.font_size;
    uint_fast8_t i = cursor.x / CHAR_WIDTH_PX / brush.font_size;

    while(*s) {
        pcd8544_print_c(*s++);
        if (++i >= char_places_per_row) {
            i = left_bd / CHAR_WIDTH_PX / brush.font_size;
            current_pg += brush.font_size;
            if (current_pg > bottom_bd)
                break;
            pcd8544_set_addr(left_bd, current_pg);
            if (*s == ' ')
                s++;
        }
    }
}

void pcd8544_draw_img(uint8_t x, uint8_t page, const struct pcd8544_image *img)
{
    const uint8_t *bitmap = img->bitmap;
    const uint8_t start_x = x;
    uint_fast8_t i, j;

    for (i = 0; i < img->height_pg; i++) {
        x = start_x;
        pcd8544_set_addr(x, page);
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
        bitmap += img->width_px - j - x / NUM_PIXELS_X;
        page += brush.image_scale;
        if (page > PAGE_MAX)
            break;
    }
}

void pcd8544_erase_txt(uint8_t col, uint8_t row, uint8_t length)
{
    uint_fast8_t i;

    pcd8544_set_cursor(col, row);
    while (length--) {
        for (i = 0; i < CHAR_WIDTH_PX; i++) {
            draw_byte(0x00, brush.font_size, brush.font_size);
        }
    }
}

void pcd8544_erase_polygon(uint8_t start_x, uint8_t start_pg,
                           uint8_t end_x, uint8_t end_pg)
{
    uint_fast8_t i, j;

    for (i = start_pg; i < end_pg; i++) {
        pcd8544_set_addr(start_x, i);
        for (j = start_x; j < end_x; j++) {
            write_dd(0x00);
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
    //pcd8544_update();
}

#if (PCD8544_USE_FRAMEBUFFER > 0)

void pcd8544_update(void)
{
    uint_fast8_t i, j;

    for (i = 0; i < NUM_PAGES; i++) {
        if (update_boundaries[i].start_x == update_boundaries[i].end_x)
            continue;
        pcd8544_set_addr(update_boundaries[i].start_x, i);
        for (j = update_boundaries[i].start_x; j < update_boundaries[i].end_x; j++) {
            write_ddram(framebuffer[i][j]);
        }
        update_boundaries[i].start_x = update_boundaries[i].end_x = 0;
    }
}
#endif