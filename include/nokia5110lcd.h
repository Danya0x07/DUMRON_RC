/**
 * Здесь интерфейс для взаимодействия с LCD дисплеем от Nokia 5110.
 */

#ifndef NOKIA5110LCD_H_INCLUDED
#define NOKIA5110LCD_H_INCLUDED

#include <stm8s.h>

typedef enum {LCD_COMMAND, LCD_DATA} MeaningOfByte;

void lcd_reset(void);
void lcd_send_byte(MeaningOfByte meaning, uint8_t byte);
void lcd_set_position(uint8_t x, uint8_t y);
void lcd_print_ascii(char);
void lcd_print_custom(uint8_t charset[][7], uint8_t symbol);
void lcd_print_string(char*);
void lcd_clear(void);

#endif
