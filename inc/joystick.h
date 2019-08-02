#ifndef JOYSTICK_H_INCLUDED
#define JOYSTICK_H_INCLUDED

#include <stm8s.h>

typedef enum {
    MIDDLE,
    UP, DOWN,
    LEFT, RIGHT,
    LEFTUP, RIGHTUP,
    LEFTDOWN, RIGHTDOWN
} JoystickDirection;

typedef enum {
    NO_DEFLECTION,
    LOW_DEFLECTION,
    HIGH_DEFLECTION
} JoystickAbsDeflection;

typedef struct {
    JoystickDirection direction;
    JoystickAbsDeflection x_abs;  /* Абсолютное отклонение по x */
    JoystickAbsDeflection y_abs;  /* Абсолютное отклонение по y */
} Joystick;

void joystick_init(void);
void joystick_update(Joystick*);

#endif
