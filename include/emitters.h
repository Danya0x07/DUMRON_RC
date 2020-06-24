#ifndef _EMITTERS_H
#define _EMITTERS_H

#include <stm8s.h>

void led_blink(uint8_t times, uint16_t interval);
void buzzer_beep(uint8_t times, uint16_t interval);

#endif  /* _EMITTERS_H */