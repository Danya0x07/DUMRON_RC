#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <stm8s.h>

/*
 * Флаги событий для всех имеющихся кнопок.
 */
#define BTN_ARMUP_PRESSED  (1 << 0)
#define BTN_ARMUP_RELEASED (1 << 1)
#define BTN_ARMDOWN_PRESSED  (1 << 2)
#define BTN_ARMDOWN_RELEASED (1 << 3)
#define BTN_CLAWSQUEEZE_PRESSED  (1 << 4)
#define BTN_CLAWSQUEEZE_RELEASED (1 << 5)
#define BTN_CLAWRELEASE_PRESSED  (1 << 6)
#define BTN_CLAWRELEASE_RELEASED (1 << 7)
#define BTN_KLAXON_PRESSED  (1 << 8)
#define BTN_KLAXON_RELEASED (1 << 9)
#define BTN_TOGGLELIGHTS_PRESSED   (1 << 10)
#define BTN_TOGGLELIGHTS_PRESSED_2 (1 << 11)

void buttons_init(void);
void buttons_update(uint16_t*);

#endif
