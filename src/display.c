#include <stm8s.h>

#include "display.h"
#include "pcd8544.h"
#include "halutils.h"

enum {
    CUSTOM_CHAR_KLAXON,
    CUSTOM_CHAR_LIGHTS,
    CUSTOM_CHAR_ARROWUP,
    CUSTOM_CHAR_ARROWDOWN,
    CUSTOM_CHAR_CONNECTION,
    CUSTOM_CHAR_CELSIUS,

    NUMBER_OF_CUSTOM_CHARS
};

static const uint8_t custom_charset[NUMBER_OF_CUSTOM_CHARS][5] = {
    {0x0D, 0x34, 0xC6, 0xC6, 0xC6, 0x34, 0x0D},
    {0x1D, 0x22, 0xC1, 0xC1, 0xC1, 0x22, 0x1D},
    {0x00, 0x0C, 0x02, 0xFF, 0x02, 0x0C, 0x00},
    {0x00, 0x30, 0x40, 0xFF, 0x40, 0x30, 0x00},
    {0x02, 0x0A, 0x2A, 0xFF, 0x2A, 0x0A, 0x02},
    {0x02, 0x05, 0x3A, 0x44, 0x82, 0x82, 0x44},
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
    delay_ms(100);
    pcd8544_clear();
}

void display_update(const data_to_robot_t *dtr, const data_from_robot_t *dfr,
                    bool ack_received, uint8_t battery_charge)
{
    /* Рисуем данные пульта. */

    /* заряд батареи пульта (%); */
    pcd8544_set_cursor(8, 0);
    if (battery_charge < 100) {
        pcd8544_print_c(' ');
    }
    if (battery_charge < 10) {
        pcd8544_print_c(' ');
    }
    pcd8544_print_s(itoa(battery_charge, 10));
    pcd8544_print_c('%');

    /* состояние связи (есть/нет); */
    pcd8544_set_cursor(7, 2);
    if (ack_received) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_CONNECTION);
    } else {
        pcd8544_print_c(' ');
    }

    /* состояние фар (вкл/выкл); */
    pcd8544_set_cursor(10, 2);
    if (dtr->ctrl.bf.lights_en) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_LIGHTS);
    } else {
        pcd8544_print_c(' ');
    }

    /* состояние бибики (вкл/выкл); */
    if (dtr->ctrl.bf.buzzer_en) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_KLAXON);
    } else {
        pcd8544_print_c(' ');
    }

    /* направление вертикального перемещения манипулятора (вверх/вниз/нет); */
    pcd8544_set_cursor(8, 3);
    switch (dtr->ctrl.bf.arm_ctrl)
    {
    case ARMCTL_UP:
        lcd_print_custom(custom_charset, CUSTOM_CHAR_ARROWUP);
        break;
    case ARMCTL_DOWN:
        lcd_print_custom(custom_charset, CUSTOM_CHAR_ARROWDOWN);
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
        lcd_print_custom(custom_charset, CUSTOM_CHAR_CELSIUS);
    } else {
        pcd8544_print_s("E ");
    }
    pcd8544_print_s("  ");

    /* и температура радиаторов. */
    pcd8544_set_cursor(0, 5);
    if (dfr->temp_radiators != TEMPERATURE_ERROR_VALUE) {
        pcd8544_print_s(itoa(dfr->temp_radiators, 10));
        lcd_print_custom(custom_charset, CUSTOM_CHAR_CELSIUS);
    } else {
        pcd8544_print_s("E ");
    }
    pcd8544_print_s("  ");
}