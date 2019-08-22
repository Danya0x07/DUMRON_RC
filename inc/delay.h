#ifndef DELAY_H_INCLUDED
#define DELAY_H_INCLUDED

#include <stm8s.h>

void delay_init(void);
void delay_ms(uint16_t);
void delay_interrupt_handler(void) __interrupt(23);

#endif
