/**
 * Здесь "биснес-логика"графического интерфейса.
 */

#include "display.h"
#include "nokia5110lcd.h"
#include "delay.h"

extern char* itoa(int, unsigned char);

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

void display_update(const data_to_robot_s* data_to_robot,
                    const data_from_robot_s* data_from_robot,
                    bool ack_received, uint8_t battery_voltage)
{
    /* Рисуем данные пульта. */

    /* заряд батареи пульта (%); */
    lcd_set_position(8, 0);
    if (battery_voltage < 100) {
        lcd_print_ascii(' ');
    }
    if (battery_voltage < 10) {
        lcd_print_ascii(' ');
    }
    lcd_print_string(itoa(battery_voltage, 10));
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
    if (data_to_robot->ctrl.bf.lightsEn) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_LIGHTS);
    } else {
        lcd_print_ascii(' ');
    }

    /* состояние бибики (вкл/выкл); */
    if (data_to_robot->ctrl.bf.buzzerEn) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_KLAXON);
    } else {
        lcd_print_ascii(' ');
    }

    /* направление вертикального перемещения манипулятора (вверх/вниз/нет); */
    lcd_set_position(8, 3);
    switch (data_to_robot->ctrl.bf.armCtrl)
    {
    case ARM_UP:
        lcd_print_custom(custom_charset, CUSTOM_CHAR_ARROWUP);
        break;
    case ARM_DOWN:
        lcd_print_custom(custom_charset, CUSTOM_CHAR_ARROWDOWN);
        break;
    case ARM_STOP:
        lcd_print_ascii(' ');
    }

    /* направление движения клешни (сжимается/разжимается/нет); */
    lcd_set_position(7, 4);
    switch (data_to_robot->ctrl.bf.clawCtrl)
    {
    case CLAW_SQUEESE:
        lcd_print_string("><");
        break;
    case CLAW_RELEASE:
        lcd_print_string("<>");
        break;
    case CLAW_STOP:
        lcd_print_string("  ");
    }

    /* направление движения робота (вперёд/назад/налево/направо/нет); */
    lcd_set_position(10, 3);
    switch (data_to_robot->ctrl.bf.moveDir)
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
    lcd_print_ascii(data_to_robot->speed_left  > 128 ? '^' : ' ');
    lcd_print_ascii(data_to_robot->speed_right > 128 ? '^' : ' ');
    lcd_set_position(10, 5);
    lcd_print_ascii(data_to_robot->speed_left  > 0 ? '^' : ' ');
    lcd_print_ascii(data_to_robot->speed_right > 0 ? '^' : ' ');

    /* Печатаем данные робота. */

    /* заряд мозговой части (%); */
    lcd_set_position(0, 0);
    lcd_print_string(itoa(data_from_robot->battery_brains, 10));
    lcd_print_string("% ");

    /* заряд силовой части (%); */
    lcd_set_position(0, 1);
    lcd_print_string(itoa(data_from_robot->battery_motors, 10));
    lcd_print_string("% ");

    /* наличия сзади препятствия или перепада высоты; */
    lcd_set_position(5, 2);
    switch (data_from_robot->status.bf.backDistance)
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

    /* температура окружающей среды (С); */
    lcd_set_position(0, 4);
    lcd_print_string(itoa(data_from_robot->temp_ambient, 10));
    lcd_print_custom(custom_charset, CUSTOM_CHAR_CELSIUS);
    lcd_print_string("  ");

    /* температура радиаторов (С); */
    lcd_set_position(0, 5);
    lcd_print_string(itoa(data_from_robot->temp_radiators, 10));
    lcd_print_custom(custom_charset, CUSTOM_CHAR_CELSIUS);
    lcd_print_string("  ");
}
