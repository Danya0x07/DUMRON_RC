/**
 * Здесь "биснес-логика"графического интерфейса.
 */

#include "display.h"
#include "nokia5110lcd.h"
#include "delay.h"

enum {
    CUSTOM_CHAR_KLAXON,
    CUSTOM_CHAR_LIGHTS,
    CUSTOM_CHAR_ARROWUP,
    CUSTOM_CHAR_ARROWDOWN,
    CUSTOM_CHAR_CONNECTION,
    
    NUMBER_OF_CUSTOM_CHARS
};

static const uint8_t custom_charset[NUMBER_OF_CUSTOM_CHARS][7] = {
    {0x0D, 0x34, 0xC6, 0xC6, 0xC6, 0x34, 0x0D},
    {0x1D, 0x22, 0xC1, 0xC1, 0xC1, 0x22, 0x1D},
    {0x00, 0x0C, 0x02, 0xFF, 0x02, 0x0C, 0x00},
    {0x00, 0x30, 0x40, 0xFF, 0x40, 0x30, 0x00},
    {0x08, 0x14, 0x2A, 0xD6, 0x2A, 0x14, 0x08},
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
}

void display_test(void)
{
    lcd_set_position(1, 1);
    lcd_print_string("display test.");
}

void display_update(const DataToRobot* data_to_robot,
                    const DataFromRobot* data_from_robot,
                    bool was_conn_error)
{
    
}
