/**
 * Модуль опроса кнопок.
 */

#ifndef _BUTTONS_H
#define _BUTTONS_H

#include <stm8s.h>

typedef enum {
    BTN_EV_NONE = 0,
    BTN_EV_PRESSED,
    BTN_EV_RELEASED,
    BTN_EV_DOUBLE_PRESSED
} btn_event_e;

typedef struct {
    btn_event_e arm_up   :2;
    btn_event_e arm_down :2;
    btn_event_e claw_squeeze :2;
    btn_event_e claw_release :2;
    btn_event_e buzzer :2;
    btn_event_e toggle_lights :2;
} btn_events_s;

void buttons_get_events(btn_events_s *ev);

#endif  /* _BUTTONS_H */
