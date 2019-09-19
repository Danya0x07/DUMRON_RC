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
    {0x08, 0x14, 0x2A, 0xD6, 0x2A, 0x14, 0x08},
    {0x02, 0x05, 0x3A, 0x44, 0x82, 0x82, 0x44},
};

void display_init(void)
{
    lcd_init_gpio();
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

void display_update(const DataToRobot* data_to_robot,
                    const DataFromRobot* data_from_robot,
                    bool was_conn_error, uint8_t battery_voltage)
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
    if (was_conn_error) {
        lcd_print_ascii(' ');
    } else {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_CONNECTION);
    }

    /* состояние фар (вкл/выкл); */
    lcd_set_position(10, 2);
    if (data_to_robot->control_reg & ROBOT_CFLAG_LIGHTS_EN) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_LIGHTS);
    } else {
        lcd_print_ascii(' ');
    }

    /* состояние бибики (вкл/выкл); */
    if (data_to_robot->control_reg & ROBOT_CFLAG_KLAXON_EN) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_KLAXON);
    } else {
        lcd_print_ascii(' ');
    }

    /* направление вертикального перемещения манипулятора (вверх/вниз/нет); */
    lcd_set_position(8, 3);
    if (data_to_robot->control_reg & ROBOT_CFLAG_ARM_UP) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_ARROWUP);
    } else if (data_to_robot->control_reg & ROBOT_CFLAG_ARM_DOWN) {
        lcd_print_custom(custom_charset, CUSTOM_CHAR_ARROWDOWN);
    } else {
        lcd_print_ascii(' ');
    }

    /* направление движения клешни (сжимается/разжимается/нет); */
    lcd_set_position(7, 4);
    if (data_to_robot->control_reg & ROBOT_CFLAG_CLAW_SQUEEZE) {
        lcd_print_string("><");
    } else if (data_to_robot->control_reg & ROBOT_CFLAG_CLAW_RELEASE) {
        lcd_print_string("<>");
    } else {
        lcd_print_string("  ");
    }

    /* направление движения робота (вперёд/назад/налево/направо/нет); */
    lcd_set_position(10, 3);
    switch (data_to_robot->direction) {
        case ROBOT_DIRECTION_FORWARD:
            lcd_print_string("/\\");
            break;
        case ROBOT_DIRECTION_BACKWARD:
            lcd_print_string("\\/");
            break;
        case ROBOT_DIRECTION_LEFTWARD:
            lcd_print_string("<-");
            break;
        case ROBOT_DIRECTION_RIGHTWARD:
            lcd_print_string("->");
            break;
        case ROBOT_DIRECTION_NONE:
            lcd_print_string("  ");
            break;
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
    if (data_from_robot->back_distance > 22) {
        lcd_print_ascii('O');
    } else if (data_from_robot->back_distance < 10) {
        lcd_print_ascii('T');
    } else {
        lcd_print_ascii(' ');
    }
    
    /* температура окружающей среды (С); */
    lcd_set_position(0, 4);
    lcd_print_string(itoa(data_from_robot->temp_ambient, 10));
    lcd_print_custom(custom_charset, CUSTOM_CHAR_CELSIUS);
    lcd_print_ascii(' ');
    
    /* температура радиаторов (С); */
    lcd_set_position(0, 5);
    lcd_print_string(itoa(data_from_robot->temp_radiators, 10));
    lcd_print_custom(custom_charset, CUSTOM_CHAR_CELSIUS);
    lcd_print_ascii(' ');
}
