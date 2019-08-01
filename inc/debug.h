#ifndef DEBUG_H
#define DEBUG_H

#include <stm8s.h>

void uart_init(void);
void uart_write_byte(uint8_t);
void uart_write_str(const char*);

#endif
