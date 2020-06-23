/**
 * Модуль излучателей.
 * В составе пульта есть отладочный светодиод и пьезобуззер.
 */

#ifndef _EMITTERS_H
#define _EMITTERS_H

#include <stm8s.h>

#include "config.h"

void led_blink(uint8_t times, uint16_t delay);
void buzzer_beep(uint8_t times, uint16_t delay);

#endif /* _EMITTERS_H */
