#ifndef JOYSTIC_H
#define JOYSTIC_H

#include <stm8s.h>

typedef enum {
    NONE,
    FORWARD,
    BACKWARD,
    LEFTWARD,
    RIGHTWARD,
    /* только у джостика */
    LEFTFRONT,
    RIGHTFRONT,
    LEFTBACK,
    RIGHTBACK
} JoysticDirection;

extern struct Joystic {
    JoysticDirection direction;
    uint8_t x_abs;
    uint8_t y_abs;
} joystic;

void joystic_init(void);
void joystic_update(void);

#endif
