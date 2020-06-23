/**
 * Модуль получения примерного уровня заряда батареек пульта.
 */

#ifndef _BATTERY_H
#define _BATTERY_H

#include <stm8s.h>

uint8_t battery_get_charge(void);  // 0 - 100%

#endif  /* _BATTERY_H */
