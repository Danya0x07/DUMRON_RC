#include <stdint.h>
#include <stdbool.h>
#include <unity.h>

static struct {
    bool inverse;
    uint8_t font_size;
    uint8_t image_scale;
} brush = {0, 1, 1};

static struct {
    uint8_t x;
    uint8_t page;
    uint8_t y;
} cursor;

static void draw_byte(uint8_t data, uint8_t x_scale, uint8_t y_scale,
                      uint8_t *buffer)
{
    const uint8_t start_x = cursor.x;
    const uint8_t start_pg = cursor.page;
    uint8_t current_x = start_x;
    uint8_t current_pg = start_pg;
    uint8_t page_ovf;
    bool bit_status;
    uint8_t idx;
    uint8_t bit_no;
    uint_fast8_t i, j;

    /*
     * buffer[y_scale]
     */

    bit_no = 0;
    for (i = 0; i < 8; i++) {
        bit_status = data & 0x01;
        for (j = 0; j < y_scale; j++) {
            idx = bit_no >> 3;
            buffer[idx] |= bit_status << (bit_no & 0x07);
            bit_no++;
        }
        data >>= 1;
    }
    //buffer[0] = data;

    for (i = 0; i < y_scale; i++) {
        page_ovf = 0;
        //set_addr(current_x, current_pg);
        for (j = 0; j < x_scale; j++) {
            //write_dd(buffer[i]);
            /*if (++current_x > X_MAX) {
                uint8_t below_pg;
                current_x = 0;
                page_ovf += y_scale;
                below_pg = current_pg + page_ovf;
                if (below_pg > PAGE_MAX)
                    break;
                set_addr(current_x, below_pg);
            }*/
        }
        current_pg++;
    }
/*
    current_x = start_x + x_scale;
    current_pg = start_pg + current_x / NUM_PIXELS_X;
    set_addr(current_x, current_pg);*/
}

void test_draw(void)
{
    uint8_t buffer[6] = {0};
    draw_byte(0x0F, 1, 2, buffer);
    TEST_ASSERT_EQUAL(0xFF, buffer[0]);
    TEST_ASSERT_EQUAL(0x00, buffer[1]);
}