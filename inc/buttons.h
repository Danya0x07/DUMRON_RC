#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <stm8s.h>

/*
 * Флаги событий для всех имеющихся кнопок.
 */
typedef enum {
    BTN_ARMUP_PRESSED  = 1 << 0,
    BTN_ARMUP_RELEASED = 1 << 1,
    BTN_ARMDOWN_PRESSED  = 1 << 2,
    BTN_ARMDOWN_RELEASED = 1 << 3,
    BTN_CLAWSQUEEZE_PRESSED  = 1 << 4,
    BTN_CLAWSQUEEZE_RELEASED = 1 << 5,
    BTN_CLAWRELEASE_PRESSED  = 1 << 6,
    BTN_CLAWRELEASE_RELEASED = 1 << 7,
    BTN_KLAXON_PRESSED  = 1 << 8,
    BTN_KLAXON_RELEASED = 1 << 9,
    BTN_TOGGLELIGHTS_PRESSED   = 1 << 10,
    BTN_TOGGLELIGHTS_PRESSED_2 = 1 << 11,
} ButtonEventFlag;

/* Регистр кнопочных событий */
extern uint16_t buttons_events;

void buttons_init(void);
void buttons_update(void);

#endif
