#ifndef _HALUTILS_H
#define _HALUTILS_H

#include <stm8s.h>

void delay_ms(uint16_t ms);

/*
 * Собственный itoa: ходят слухи, что _itoa из стандартной библиотеки
 * SDCC работает криво и ломает память.
 */
char *itoa(int n, unsigned char radix);

/*
 * Абстракция для работы с АЦП.
 */
void adc_start_conversion(ADC1_Channel_TypeDef channel);
uint16_t adc_read_value(void);

#endif  /* _HALUTILS_H */