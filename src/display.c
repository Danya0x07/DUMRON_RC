#include <stm8s.h>
#include <string.h>

#include "display.h"
#include "pcd8544.h"
#include "halutils.h"
#include "config.h"

static const uint8_t buzzer_en_bitmap[5] = {0x18, 0x28, 0x4C, 0x28, 0x18};
static const uint8_t lights_en_bitmap[5] = {0x1C, 0x22, 0x62, 0x22, 0x1C};
static const uint8_t connection_bitmap[5] = {0x7C, 0x04, 0x14, 0x10, 0x1F};
static const uint8_t celsius_bitmap[5] = {0x03, 0x3B, 0x44, 0x44, 0x44};

static const uint8_t arm_up_bitmap[5] = {0x04, 0x02, 0x7F, 0x02, 0x04};
static const uint8_t arm_down_bitmap[5] = {0x10, 0x20, 0x7F, 0x20, 0x10};

static const uint8_t claw_stop_bitmap[12] = {
    0x00, 0x00, 0x1C, 0x3F, 0x63, 0x40, 0x40, 0x63, 0x3F, 0x1C, 0x00, 0x00
};
static const uint8_t claw_squeeze_bitmap[12] = {
    0x00, 0x00, 0x00, 0x3C, 0x7F, 0x43, 0x43, 0x7F, 0x3C, 0x00, 0x00, 0x00
};
static const uint8_t claw_release_bitmap[12] = {
    0x00, 0x06, 0x3E, 0x38, 0x60, 0x40, 0x40, 0x60, 0x38, 0x3E, 0x06, 0x00
};

static const uint8_t speed_f0_bitmap[6] = {0x70, 0x70, 0x70, 0x70, 0x70, 0x70};
static const uint8_t speed_f1_bitmap[6] = {0x1C, 0x7C, 0x7C, 0x7C, 0x7C, 0x1C};
static const uint8_t speed_f2_bitmap[6] = {0x07, 0x1F, 0x7F, 0x7F, 0x1F, 0x07};
static const uint8_t speed_b0_bitmap[6] = {0x07, 0x07, 0x07, 0x07, 0x07, 0x07};
static const uint8_t speed_b1_bitmap[6] = {0x1C, 0x1F, 0x1F, 0x1F, 0x1F, 0x1C};
static const uint8_t speed_b2_bitmap[6] = {0x70, 0x7C, 0x7F, 0x7F, 0x7C, 0x70};

static const uint8_t cliff_bitmap[18] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x7F, 0x00, 0x00, 0x5C, 0x00
};

static const uint8_t obstacle_bitmap[18] = {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x57, 0x40, 0x40, 0x40, 0x7F, 0x00
};

static const uint8_t blank_bitmap[6] = {0, 0, 0, 0, 0, 0};

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

static const struct pcd8544_image arm_up_image = {
    .bitmap = arm_up_bitmap,
    .lookup = FALSE,
    .width_px = 5,
    .height_pg = 1
};

static const struct pcd8544_image arm_down_image = {
    .bitmap = arm_down_bitmap,
    .lookup = FALSE,
    .width_px = 5,
    .height_pg = 1
};

static const struct pcd8544_image claw_stop_image = {
    .bitmap = claw_stop_bitmap,
    .lookup = FALSE,
    .width_px = 12,
    .height_pg = 1
};

static const struct pcd8544_image claw_squeeze_image = {
    .bitmap = claw_squeeze_bitmap,
    .lookup = FALSE,
    .width_px = 12,
    .height_pg = 1
};

static const struct pcd8544_image claw_release_image = {
    .bitmap = claw_release_bitmap,
    .lookup = FALSE,
    .width_px = 12,
    .height_pg = 1
};

static const struct pcd8544_image speed_f0_image = {
    .bitmap = speed_f0_bitmap,
    .lookup = FALSE,
    .width_px = 6,
    .height_pg = 1
};

static const struct pcd8544_image speed_f1_image = {
    .bitmap = speed_f1_bitmap,
    .lookup = FALSE,
    .width_px = 6,
    .height_pg = 1
};

static const struct pcd8544_image speed_f2_image = {
    .bitmap = speed_f2_bitmap,
    .lookup = FALSE,
    .width_px = 6,
    .height_pg = 1
};

static const struct pcd8544_image speed_b0_image = {
    .bitmap = speed_b0_bitmap,
    .lookup = FALSE,
    .width_px = 6,
    .height_pg = 1
};

static const struct pcd8544_image speed_b1_image = {
    .bitmap = speed_b1_bitmap,
    .lookup = FALSE,
    .width_px = 6,
    .height_pg = 1
};

static const struct pcd8544_image speed_b2_image = {
    .bitmap = speed_b2_bitmap,
    .lookup = FALSE,
    .width_px = 6,
    .height_pg = 1
};

static const struct pcd8544_image cliff_image = {
    .bitmap = cliff_bitmap,
    .lookup = FALSE,
    .width_px = 18,
    .height_pg = 1
};

static const struct pcd8544_image obstacle_image = {
    .bitmap = obstacle_bitmap,
    .lookup = FALSE,
    .width_px = 18,
    .height_pg = 1
};

static const struct pcd8544_image blank_image = {
    .bitmap = blank_bitmap,
    .lookup = FALSE,
    .width_px = 6,
    .height_pg = 1
};

static uint8_t print_int_value(int16_t value, char unit)
{
    const char *content = itoa(value, 10);
    uint8_t len = strlen(content);

    pcd8544_print_s(content);
    if (unit)
        pcd8544_print_c(unit);

    return len;
}

static void print_filling_spaces(int16_t value)
{
    if (value < 0)
        value = -value;
    if (value < 100)
        pcd8544_print_c(' ');
    if (value < 10)
        pcd8544_print_c(' ');
}

static void print_int_from_left_bd(uint8_t col, uint8_t row, int16_t value, char unit)
{
    pcd8544_set_cursor(col, row);
    print_int_value(value, unit);
    print_filling_spaces(value);
}

static void print_int_to_right_bd(uint8_t col, uint8_t row, int16_t value, char unit)
{
    pcd8544_set_cursor(col, row);
    print_filling_spaces(value);
    print_int_value(value, unit);
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
    pcd8544_set_txt_language(PCD8544_LANG_RU);
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_clear();
}

void display_splash_screen(void)
{
    pcd8544_setup_brush(FALSE, 2, 1);
    pcd8544_clear();
    pcd8544_set_cursor(0, 0);
    pcd8544_print_s("ДУМРОН");
    pcd8544_set_cursor(0, 1);
    pcd8544_print_s("пульт");
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_set_cursor(10, 3);
    pcd8544_print_s("v2.0");
    pcd8544_setup_brush(TRUE, 1, 1);
    pcd8544_set_cursor(0, 5);
    pcd8544_print_s(" by Danya0x07 ");
    delay_ms(3000);
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_clear();
}

void display_transceiver_missing(void)
{
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_clear();
    pcd8544_set_cursor(0, 0);
    pcd8544_print_s("[ошибка]:");
    pcd8544_set_cursor(0, 1);
    pcd8544_print_s("не найден");
    pcd8544_set_cursor(0, 2);
    pcd8544_print_s("радиомодуль");
}

void display_rf_scan_started(void)
{
    pcd8544_setup_brush(FALSE, 1, 1);
    pcd8544_clear();
    pcd8544_set_cursor(0, 0);
    pcd8544_print_s("Сканирование");
    pcd8544_set_cursor(0, 1);
    pcd8544_print_s("эфира...");
}

void display_rf_scan_ended(void)
{
    pcd8544_clear();
}

void display_update(const data_to_robot_t *dtr, const data_from_robot_t *dfr,
                    bool was_response, uint8_t battery_charge)
{
    const struct pcd8544_image *img = NULL;

    // ----------- Печатаем данные пульта. ----------------

    // заряд батареи
    print_int_to_right_bd(10, 0, battery_charge, '%');

    // качество связи
    img = was_response ? &connection_image : &blank_image;
    pcd8544_draw_img(48, 0, img);

    // включен ли прожектор
    img = dtr->ctrl.bf.lights_en ? &lights_en_image : &blank_image;
    pcd8544_draw_img(72, 2, img);

    // включён ли пьезобуззер
    img = dtr->ctrl.bf.buzzer_en ? &buzzer_en_image : &blank_image;
    pcd8544_draw_img(78, 2, img);

    // состояние джойстика
    switch (dtr->ctrl.bf.move_dir)
    {
    case MOVEDIR_FORWARD:
    case MOVEDIR_RIGHTWARD:
        img = dtr->speed_left  > 128 ? &speed_f2_image : &speed_f1_image;
        break;
    default:
        img = &speed_f0_image;
    }
    pcd8544_draw_img(72, 4, img);

    switch (dtr->ctrl.bf.move_dir)
    {
    case MOVEDIR_FORWARD:
    case MOVEDIR_LEFTWARD:
        img = dtr->speed_right  > 128 ? &speed_f2_image : &speed_f1_image;
        break;
    default:
        img = &speed_f0_image;
    }
    pcd8544_draw_img(78, 4, img);

    switch (dtr->ctrl.bf.move_dir)
    {
    case MOVEDIR_BACKWARD:
    case MOVEDIR_LEFTWARD:
        img = dtr->speed_left  > 128 ? &speed_b2_image : &speed_b1_image;
        break;
    default:
        img = &speed_b0_image;
    }
    pcd8544_draw_img(72, 5, img);

    switch (dtr->ctrl.bf.move_dir)
    {
    case MOVEDIR_BACKWARD:
    case MOVEDIR_RIGHTWARD:
        img = dtr->speed_right  > 128 ? &speed_b2_image : &speed_b1_image;
        break;
    default:
        img = &speed_b0_image;
    }
    pcd8544_draw_img(78, 5, img);

    // состояние плеча манипулятора
    switch (dtr->ctrl.bf.arm_ctrl)
    {
    case ARMCTL_UP:     img = &arm_up_image;   break;
    case ARMCTL_DOWN:   img = &arm_down_image; break;
    case ARMCTL_STOP:   img = &blank_image;
    }
    pcd8544_draw_img(66, 3, img);

    // состояние клешни манипулятора
    switch (dtr->ctrl.bf.claw_ctrl)
    {
    case CLAWCTL_SQUEEZE: img = &claw_squeeze_image; break;
    case CLAWCTL_RELEASE: img = &claw_release_image; break;
    case CLAWCTL_STOP:    img = &claw_stop_image;
    }
    pcd8544_draw_img(72, 3, img);

    // ----------- Печатаем данные робота. ----------------

    // заряд батареи мозговой части
    print_int_from_left_bd(0, 0, dfr->battery_brains, '%');

    // заряд батареи силовой части
    print_int_from_left_bd(0, 1, dfr->battery_motors, '%');

    // состояние дел сзади
    switch (dfr->status.bf.back_distance)
    {
    case DIST_CLIFF:
        pcd8544_draw_img(0, 3, &cliff_image);
        break;
    case DIST_OBSTACLE:
        pcd8544_draw_img(0, 3, &obstacle_image);
        break;
    case DIST_NOTHING:
        pcd8544_clear_area(0, 3, 17, 3);
        break;
    case DIST_ERROR:
        pcd8544_set_cursor(0, 3);
        pcd8544_print_s("N/A");
    }

    // температура окружающей среды
    pcd8544_set_cursor(0, 4);
    if (dfr->temp_ambient != TEMPERATURE_ERROR_VALUE) {
        uint8_t pos = print_int_value(dfr->temp_ambient, 0);
        pcd8544_draw_img(pos * 6, 4, &celsius_image);
    } else {
        pcd8544_print_s("N/A ");
    }

    // температура внутри корпуса
    pcd8544_set_cursor(0, 5);
    if (dfr->temp_radiators != TEMPERATURE_ERROR_VALUE) {
        uint8_t pos = print_int_value(dfr->temp_radiators, 0);
        pcd8544_draw_img(pos * 6, 5, &celsius_image);
    } else {
        pcd8544_print_s("N/A ");
    }

    // номер текущего частотного канала
    pcd8544_set_cursor(5, 1);
    pcd8544_print_s("CH");
    print_int_from_left_bd(7, 1, dtr->radio.bf.channel, 0);
}