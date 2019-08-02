#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stm8s.h>

void uart_init(void);
void uart_write_byte(uint8_t);
void uart_write_str(const char*);

#endif
