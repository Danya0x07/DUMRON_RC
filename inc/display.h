#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include <stm8s.h>

#define USE_VIRT_SPI

void display_init(void);
void display_test(char*);

#endif
