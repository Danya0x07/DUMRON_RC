/*
 * Здесь интерфейс для взаимодействия с LCD дисплеем от Nokia 5110.
 */

#ifndef NOKIA5110LCD_H_INCLUDED
#define NOKIA5110LCD_H_INCLUDED

#include <stm8s.h>

void lcd_init(void);
void lcd_set_position(uint8_t x, uint8_t y);
void lcd_print_character(char);
void lcd_print_string(char*);
void lcd_clear(void);

#endif
