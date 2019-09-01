/**
 * Здесь "биснес-логика"графического интерфейса.
 *
 * На данный момент GUI планируется таким: 
 *
 *             __1___2___3_
 *            |%100 100 100|    1: % заряда аккумулятора силовой части робота;
 *            |            |    2: % заряда батареи мозговой части робота;
 *            |            |    3: % заряда батареи пульта;
 *            |            |
 *            |          ^^|
 *            |          ^^|
 *            TTTTTTTTTTTTTT
 */

#include "display.h"
#include "nokia5110lcd.h"
#include "delay.h"

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
