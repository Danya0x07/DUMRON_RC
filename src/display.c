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

static const uint8_t super_bitmap[2][28] = {
    {
        0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x18, 0x30, 0x60, 0xC0, 0x80, 0x03, 0x02,
        0x02, 0x03, 0xC0, 0x30, 0x18, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    },
    {
        0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
        0x40, 0x40, 0x40, 0x40, 0x60, 0x31, 0x1f,
        0x06, 0x0f, 0x19, 0x30, 0x60, 0xc0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    }
};

static const struct pcd8544_image super_image = {
    .bitmap = (const uint8_t *)super_bitmap,
    .lookup = FALSE,
    .width_px = 28,
    .height_pg = 2
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

void display_init(void)
{
    struct pcd8544_config config = {
        .brightness = 0x3F,
        .contrast = 0,
        .temperature_coeff = 0
    };

    pcd8544_reset();
    pcd8544_configure(&config);
    pcd8544_clear();
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

    pcd8544_draw_img(18, 2, &super_image);
}