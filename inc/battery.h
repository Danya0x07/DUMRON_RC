#ifndef BATTERY_H_INCLUDED
#define BATTERY_H_INCLUDED

#include <stm8s.h>

void battery_init(void);
uint8_t battery_get_voltage(void);

#endif
