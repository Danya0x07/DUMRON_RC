#include <stm8s.h>

#include "display.h"
#include "nokia5110lcd.h"
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

static const uint8_t custom_charset[NUMBER_OF_CUSTOM_CHARS][7] = {
    {0x0D, 0x34, 0xC6, 0xC6, 0xC6, 0x34, 0x0D},
    {0x1D, 0x22, 0xC1, 0xC1, 0xC1, 0x22, 0x1D},
    {0x00, 0x0C, 0x02, 0xFF, 0x02, 0x0C, 0x00},
    {0x00, 0x30, 0x40, 0xFF, 0x40, 0x30, 0x00},
    {0x02, 0x0A, 0x2A, 0xFF, 0x2A, 0x0A, 0x02},
    {0x02, 0x05, 0x3A, 0x44, 0x82, 0x82, 0x44},
};

void display_init(void)
{
    lcd_reset();
    lcd_send_byte(LCD_COMMAND, 0x21);  /* Переключиться в режим расширенных команд. */
    lcd_send_byte(LCD_COMMAND, 0xBF);  /* Установить яркость. */
    lcd_send_byte(LCD_COMMAND, 0x04);  /* Установить температурный коэффициент 0. */
    lcd_send_byte(LCD_COMMAND, 0x10);  /* Установить смещение напряжения 1:100. */
    lcd_send_byte(LCD_COMMAND, 0x20);  /* Переключиться в режим стандартных команд. */
    lcd_send_byte(LCD_COMMAND, 0x0C);  /* Перевести дисплей в нормальный режим. */
    delay_ms(100);
    lcd_clear();
}

void display_update(const data_to_robot_t *dtr, const data_from_robot_t *dfr,
                    bool ack_received, uint8_t battery_charge)
{
    /* Рисуем данные пульта. */

    /* заряд батареи пульта (%); */
    lcd_set_position(8, 0);
    if (battery_charge < 100) {
        lcd_print_ascii(' ');
    }
    if (battery_charge < 10) {
        lcd_print_ascii(' ');
    }
    lcd_print_string(itoa(battery_charge, 10));
    lcd_print_ascii('%');

    /* состояние связи (есть/нет); */
    lcd_set_position(7, 2);
    if (ack_received) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_CONNECTION);
    } else {
        lcd_print_ascii(' ');
    }

    /* состояние фар (вкл/выкл); */
    lcd_set_position(10, 2);
    if (dtr->ctrl.bf.lights_en) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_LIGHTS);
    } else {
        lcd_print_ascii(' ');
    }

    /* состояние бибики (вкл/выкл); */
    if (dtr->ctrl.bf.buzzer_en) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_KLAXON);
    } else {
        lcd_print_ascii(' ');
    }

    /* направление вертикального перемещения манипулятора (вверх/вниз/нет); */
    lcd_set_position(8, 3);
    switch (dtr->ctrl.bf.arm_ctrl)
    {
    case ARMCTL_UP:
        lcd_print_custom(custom_charset, CUSTOM_CHAR_ARROWUP);
        break;
    case ARMCTL_DOWN:
        lcd_print_custom(custom_charset, CUSTOM_CHAR_ARROWDOWN);
        break;
    case ARMCTL_STOP:
        lcd_print_ascii(' ');
    }

    /* направление движения клешни (сжимается/разжимается/нет); */
    lcd_set_position(7, 4);
    switch (dtr->ctrl.bf.claw_ctrl)
    {
    case CLAWCTL_SQUEEZE:
        lcd_print_string("><");
        break;
    case CLAWCTL_RELEASE:
        lcd_print_string("<>");
        break;
    case CLAWCTL_STOP:
        lcd_print_string("  ");
    }

    /* направление движения робота (вперёд/назад/налево/направо/нет); */
    lcd_set_position(10, 3);
    switch (dtr->ctrl.bf.move_dir)
    {
    case MOVEDIR_FORWARD:
        lcd_print_string("/\\");
        break;
    case MOVEDIR_BACKWARD:
        lcd_print_string("\\/");
        break;
    case MOVEDIR_LEFTWARD:
        lcd_print_string("<-");
        break;
    case MOVEDIR_RIGHTWARD:
        lcd_print_string("->");
        break;
    case MOVEDIR_NONE:
        lcd_print_string("  ");
    }
    /* скорости гусениц (быстро/медленно/нет); */
    lcd_set_position(10, 4);
    lcd_print_ascii(dtr->speed_left  > 128 ? '^' : ' ');
    lcd_print_ascii(dtr->speed_right > 128 ? '^' : ' ');
    lcd_set_position(10, 5);
    lcd_print_ascii(dtr->speed_left  > 0 ? '^' : ' ');
    lcd_print_ascii(dtr->speed_right > 0 ? '^' : ' ');

    /* Печатаем данные робота. */

    /* заряд мозговой части (%); */
    lcd_set_position(0, 0);
    lcd_print_string(itoa(dfr->battery_brains, 10));
    lcd_print_string("% ");

    /* заряд силовой части (%); */
    lcd_set_position(0, 1);
    lcd_print_string(itoa(dfr->battery_motors, 10));
    lcd_print_string("% ");

    /* наличия сзади препятствия или перепада высоты; */
    lcd_set_position(5, 2);
    switch (dfr->status.bf.back_distance)
    {
    case DIST_CLIFF:
        lcd_print_ascii('O');
        break;
    case DIST_OBSTACLE:
        lcd_print_ascii('|');
        break;
    case DIST_ERROR:
        lcd_print_ascii('E');
        break;
    case DIST_NOTHING:
        lcd_print_ascii(' ');
    }

    /* температура окружающей среды; */
    lcd_set_position(0, 4);
    if (dfr->temp_ambient != TEMPERATURE_ERROR_VALUE) {
        lcd_print_string(itoa(dfr->temp_ambient, 10));
        lcd_print_custom(custom_charset, CUSTOM_CHAR_CELSIUS);
    } else {
        lcd_print_string("E ");
    }
    lcd_print_string("  ");

    /* и температура радиаторов. */
    lcd_set_position(0, 5);
    if (dfr->temp_radiators != TEMPERATURE_ERROR_VALUE) {
        lcd_print_string(itoa(dfr->temp_radiators, 10));
        lcd_print_custom(custom_charset, CUSTOM_CHAR_CELSIUS);
    } else {
        lcd_print_string("E ");
    }
    lcd_print_string("  ");
}