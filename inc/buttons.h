#ifndef BUTTONS_H
#define BUTTONS_H

#include <stm8s.h>

/*
 * Флаги событий для всех имеющихся кнопок.
 */
enum {
    BTN_ARMUP_PRESSED   = 1 << 0,
    BTN_ARMUP_RELEASED  = 1 << 1,
    BTN_ARMUP_PRESSED_2 = 1 << 2,
    BTN_ARMDOWN_PRESSED   = 1 << 3,
    BTN_ARMDOWN_RELEASED  = 1 << 4,
    BTN_ARMDOWN_PRESSED_2 = 1 << 5,
    BTN_CLAWSQUEEZE_PRESSED   = 1 << 6,
    BTN_CLAWSQUEEZE_RELEASED  = 1 << 7,
    BTN_CLAWSQUEEZE_PRESSED_2 = 1 << 8,
    BTN_CLAWRELEASE_PRESSED   = 1 << 9,
    BTN_CLAWRELEASE_RELEASED  = 1 << 10,
    BTN_CLAWRELEASE_PRESSED_2 = 1 << 11,
    BTN_KLAXON_PRESSED   = 1 << 12,
    BTN_KLAXON_RELEASED  = 1 << 13,
    BTN_KLAXON_PRESSED_2 = 1 << 14,
    BTN_TOGGLELIGHTS_PRESSED   = 1 << 15,
    BTN_TOGGLELIGHTS_RELEASED  = 1L << 16,
    BTN_TOGGLELIGHTS_PRESSED_2 = 1L << 17
};

/* Регистр кнопочных событий */
extern uint32_t buttons_events;

void buttons_init(void);
void buttons_checkevents(void);

#endif
