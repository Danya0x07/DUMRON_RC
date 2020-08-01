#include <stm8s.h>

#include "display.h"
#include "pcd8544.h"
#include "halutils.h"
#include "config.h"

static const uint8_t buzzer_en_bitmap[5] = {0x18, 0x28, 0x4C, 0x28, 0x18};
static const uint8_t lights_en_bitmap[5] = {0x1C, 0x22, 0x62, 0x22, 0x1C};
static const uint8_t arrow_up_bitmap[5] = {0x04, 0x02, 0x7F, 0x02, 0x04};
static const uint8_t arrow_down_bitmap[5] = {0x10, 0x20, 0x7F, 0x20, 0x10};
static const uint8_t connection_bitmap[5] = {0x7C, 0x04, 0x14, 0x10, 0x1F};
static const uint8_t celsius_bitmap[5] = {0x03, 0x3B, 0x44, 0x44, 0x44};

static const uint8_t super_bitmap[6][84] = {
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
        0xC0, 0xE0, 0xF0, 0x70, 0x30, 0x38, 0x1C, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
        0x1F, 0x1F, 0x3E, 0x3D, 0x7C, 0xF0, 0xE0, 0xC8, 0xC0, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFA, 0xFE, 0x7F,
        0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x20, 0x10, 0x18, 0x08, 0x09, 0x0B, 0x0F, 0x1F, 0x7F, 0xFE,
        0xF8, 0xF0, 0xE0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
        0xEC, 0x82, 0xC3, 0xC1, 0xC1, 0xE1, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x04, 0x06, 0x06, 0x02, 0x02, 0x02, 0x00, 0x01,
        0x07, 0x0F, 0x3F, 0xFF, 0xFF, 0xFC, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xE0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFB, 0xE0, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0F, 0x0F, 0xC3, 0xE0, 0xE1, 0xE1, 0x60, 0x40, 0x40, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0x85, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xE0, 0xC0, 0xC0, 0x80, 0x80, 0x00,
        0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
        0x80, 0x80, 0xC0, 0xE0, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xF8, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xBE, 0x3F, 0x1F, 0x0F, 0x1F,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xF8, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
};

static const uint8_t sb2[100] = {
    0x7E, 0x99, 0x99, 0x99, 0xDB, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x24, 0x3C
};

static const struct pcd8544_image super_image = {
    .bitmap = (const uint8_t *)super_bitmap,
    .lookup = FALSE,
    .width_px = sizeof(super_bitmap[0]),
    .height_pg = sizeof(super_bitmap) / sizeof(super_bitmap[0])
};

static const struct pcd8544_image sb2_image = {
    .bitmap = (const uint8_t *)sb2,
    .lookup = FALSE,
    .width_px = 14,
    .height_pg = 1
};

static const struct pcd8544_image buzzer_en_image = {
    .bitmap = buzzer_en_bitmap,
    .lookup = FALSE,
    .width_px = 5,
    .height_pg = 1
};

static const struct pcd8544_image lights_en_image = {
    .bitmap = lights_en_bitmap,
    .lookup = FALSE,
    .width_px = 5,
    .height_pg = 1
};

static const struct pcd8544_image arrow_up_image = {
    .bitmap = arrow_up_bitmap,
    .lookup = FALSE,
    .width_px = 5,
    .height_pg = 1
};

static const struct pcd8544_image arrow_down_image = {
    .bitmap = arrow_down_bitmap,
    .lookup = FALSE,
    .width_px = 5,
    .height_pg = 1
};

static const struct pcd8544_image connection_image = {
    .bitmap = connection_bitmap,
    .lookup = FALSE,
    .width_px = 5,
    .height_pg = 1
};

static const struct pcd8544_image celsius_image = {
    .bitmap = celsius_bitmap,
    .lookup = FALSE,
    .width_px = 5,
    .height_pg = 1
};

void test(void)
{
#if (PCD8544_USE_FRAMEBUFFER == 0)
#   define pcd8544_update()
#endif

    // печать строки размера 1, переполнение
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('0');

    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 1);
    pcd8544_print_s("string 1, should overflow, how many times I don't know, 1234567890");
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // печать строки размера 2, переполнение
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('1');

    pcd8544_setup_brush(FALSE, 2, 1);
    pcd8544_set_cursor(1, 0);
    pcd8544_print_s("string 2, should overflow, how many times I don't know");
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // печать строки размера 3, переполнение
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('2');

    pcd8544_setup_brush(FALSE, 3, 1);
    pcd8544_set_cursor(1, 0);
    pcd8544_print_s("string 3, should overflow, how many times I don't know");
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // рисование картинки, размер 1
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('3');

    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_draw_img(3, 1, &super_image);
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // рисование картинки, размер 2
    pcd8544_setup_brush(FALSE, 1, 2);
    pcd8544_draw_img(3, 1, &super_image);
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('4');
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // рисование картинки, размер 1, выезд
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('5');

    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_draw_img(70, 5, &super_image);
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // рисование картинки, размер 2, выезд
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('6');

    pcd8544_setup_brush(FALSE, 1, 2);
    pcd8544_draw_img(56, 4, &super_image);
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // рисование картинки, заполнение, инверсия
    pcd8544_setup_brush(TRUE, 1, 3);
    pcd8544_draw_img(0, 0, &super_image);
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('7');
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // рисование картинки, заполнение, выезд
    pcd8544_setup_brush(FALSE, 1, 3);
    pcd8544_draw_img(6, 1, &super_image);
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('8');
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // текст в виртуальной рамке
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('9');

    pcd8544_set_cursor(2, 1);
    pcd8544_print_s_f(7, 78, 4, "I am Inevideble; Fuck you!@@@@@@@@@@@@@@@@@@@@@");
    pcd8544_update();
    delay_ms(2000);
    pcd8544_clear();

    // очистка прямоугольником
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('A');

    pcd8544_setup_brush(TRUE, 1, 1);
    pcd8544_clear_area(6, 1, 64, 5);
    pcd8544_set_cursor(1, 1);
    pcd8544_print_s_f(7, 78, 4, "Fsosiety00");
    pcd8544_update();
    delay_ms(2000);

#if (PCD8544_USE_FRAMEBUFFER == 1)
    // рисование линий
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_clear();
    pcd8544_set_cursor(0, 0);
    pcd8544_print_c('B');

    pcd8544_draw_vline(42, 4, 43);
    pcd8544_draw_hline(23, 4, 79);
    pcd8544_draw_pixel(83, 0, 1);
    pcd8544_draw_pixel(83, 2, 0);
    pcd8544_setup_brush(TRUE, 1, 1);
    pcd8544_draw_vline(70, 4, 43);
    pcd8544_draw_hline(40, 4, 79);
    pcd8544_draw_pixel(83, 47, 1);
    pcd8544_draw_pixel(83, 45, 0);
    pcd8544_update();
#endif
    delay_ms(2000);
    pcd8544_set_power(0);
    delay_ms(2000);
    pcd8544_set_power(1);
    pcd8544_update();
}

void display_init(void)
{
    struct pcd8544_config config = {
        .bias = 0,
        .contrast = 0x3F,
        .tempcoeff = 0
    };

    pcd8544_reset();
    pcd8544_configure(&config);
    pcd8544_set_mode(PCD8544_MODE_NORMAL);
    pcd8544_clear();
    test();
    while (1);
}

void display_update(const data_to_robot_t *dtr, const data_from_robot_t *dfr,
                    bool ack_received, uint8_t battery_charge)
{
    /* Рисуем данные пульта. */

    /* заряд батареи пульта (%); */
    pcd8544_set_cursor(9, 0);
    if (battery_charge < 100) {
        pcd8544_print_c(' ');
    }
    if (battery_charge < 10) {
        pcd8544_print_c(' ');
    }
    pcd8544_print_s(itoa(battery_charge, 10));
    pcd8544_print_c('%');

    /* состояние связи (есть/нет); */
    pcd8544_set_cursor(8, 2);
    if (ack_received) {
        pcd8544_draw_img(48, 2, &connection_image);
    } else {
        pcd8544_print_c(' ');
    }

    /* состояние фар (вкл/выкл); */
    pcd8544_set_cursor(10, 2);
    if (dtr->ctrl.bf.lights_en) {
        pcd8544_draw_img(60, 2, &lights_en_image);
    } else {
        pcd8544_print_c(' ');
    }

    /* состояние бибики (вкл/выкл); */
    if (dtr->ctrl.bf.buzzer_en) {
        pcd8544_draw_img(66, 2, &buzzer_en_image);
    } else {
        pcd8544_print_c(' ');
    }

    /* направление вертикального перемещения манипулятора (вверх/вниз/нет); */
    pcd8544_set_cursor(8, 3);
    switch (dtr->ctrl.bf.arm_ctrl)
    {
    case ARMCTL_UP:
        pcd8544_draw_img(48, 3, &arrow_up_image);
        break;
    case ARMCTL_DOWN:
        pcd8544_draw_img(48, 3, &arrow_down_image);
        break;
    case ARMCTL_STOP:
        pcd8544_print_c(' ');
    }

    /* направление движения клешни (сжимается/разжимается/нет); */
    pcd8544_set_cursor(7, 4);
    switch (dtr->ctrl.bf.claw_ctrl)
    {
    case CLAWCTL_SQUEEZE:
        pcd8544_print_s("><");
        break;
    case CLAWCTL_RELEASE:
        pcd8544_print_s("<>");
        break;
    case CLAWCTL_STOP:
        pcd8544_print_s("  ");
    }

    /* направление движения робота (вперёд/назад/налево/направо/нет); */
    pcd8544_set_cursor(10, 3);
    switch (dtr->ctrl.bf.move_dir)
    {
    case MOVEDIR_FORWARD:
        pcd8544_print_s("/\\");
        break;
    case MOVEDIR_BACKWARD:
        pcd8544_print_s("\\/");
        break;
    case MOVEDIR_LEFTWARD:
        pcd8544_print_s("<-");
        break;
    case MOVEDIR_RIGHTWARD:
        pcd8544_print_s("->");
        break;
    case MOVEDIR_NONE:
        pcd8544_print_s("  ");
    }
    /* скорости гусениц (быстро/медленно/нет); */
    pcd8544_set_cursor(10, 4);
    pcd8544_print_c(dtr->speed_left  > 128 ? '^' : ' ');
    pcd8544_print_c(dtr->speed_right > 128 ? '^' : ' ');
    pcd8544_set_cursor(10, 5);
    pcd8544_print_c(dtr->speed_left  > 0 ? '^' : ' ');
    pcd8544_print_c(dtr->speed_right > 0 ? '^' : ' ');

    /* Печатаем данные робота. */

    /* заряд мозговой части (%); */
    pcd8544_set_cursor(0, 0);
    pcd8544_print_s(itoa(dfr->battery_brains, 10));
    pcd8544_print_s("% ");

    /* заряд силовой части (%); */
    pcd8544_set_cursor(0, 1);
    pcd8544_print_s(itoa(dfr->battery_motors, 10));
    pcd8544_print_s("% ");

    /* наличия сзади препятствия или перепада высоты; */
    pcd8544_set_cursor(5, 2);
    switch (dfr->status.bf.back_distance)
    {
    case DIST_CLIFF:
        pcd8544_print_c('O');
        break;
    case DIST_OBSTACLE:
        pcd8544_print_c('|');
        break;
    case DIST_ERROR:
        pcd8544_print_c('E');
        break;
    case DIST_NOTHING:
        pcd8544_print_c(' ');
    }

    /* температура окружающей среды; */
    pcd8544_set_cursor(0, 4);
    if (dfr->temp_ambient != TEMPERATURE_ERROR_VALUE) {
        pcd8544_print_s(itoa(dfr->temp_ambient, 10));
        pcd8544_draw_img(0, 4, &celsius_image);
    } else {
        pcd8544_print_s("E ");
    }
    pcd8544_print_s("  ");

    /* и температура радиаторов. */
    pcd8544_set_cursor(0, 5);
    if (dfr->temp_radiators != TEMPERATURE_ERROR_VALUE) {
        pcd8544_print_s(itoa(dfr->temp_radiators, 10));
        pcd8544_draw_img(0, 5, &celsius_image);
    } else {
        pcd8544_print_s("E ");
    }
    pcd8544_print_s("  ");


    pcd8544_setup_brush(FALSE, 1, 6);
    //pcd8544_set_cursor(0, 1);
    pcd8544_draw_img(0, 0, &sb2_image);

    //pcd8544_print_s("Wake up, Neo... Matrix has you...");
    pcd8544_setup_brush(FALSE, 1, 1);
}