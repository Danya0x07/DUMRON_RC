/*
 * Слой аппаратной абстракции и всякая вспомогателовка.
 *
 * Если использовать хотя бы одну функцию из какого-либо файла
 * STM8S_StdPeriphDriver, компилятор SDCC пришьёт этот файл целиком к прошивке.
 * Поэтому, для экономии Flash-памяти МК, имеет смысл создать локальный HAL.
 */

#ifndef _HALUTILS_H
#define _HALUTILS_H

#include <stm8s.h>
#include <stddef.h>

typedef uint8_t uint_fast8_t;

void tim2_set_counter(uint16_t value);
uint16_t tim2_get_counter(void);

void tim3_set_counter(uint16_t value);
uint16_t tim3_get_counter(void);

#define tim4_set_counter(value)     (TIM4->CNTR = (value))
#define tim4_get_counter()  (TIM4->CNTR)

void adc_start_conversion(ADC1_Channel_TypeDef channel);
bool adc_conversion_complete(void);
uint16_t adc_read_value(void);

uint8_t spi_transfer_byte(uint8_t byte);
void spi_transfer_bytes(uint8_t *in, const uint8_t *out, uint8_t len);

void delay_ms(uint16_t ms);

/*
 * Собственный itoa: ходят слухи, что _itoa из стандартной библиотеки
 * SDCC работает криво и ломает память.
 */
char *itoa(int n, unsigned char radix);

#endif  /* _HALUTILS_H */