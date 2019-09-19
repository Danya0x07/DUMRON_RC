#ifndef JOYSTICK_H_INCLUDED
#define JOYSTICK_H_INCLUDED

#include "robot_interface.h"

typedef enum {
    JOYSTICK_DIRECTION_MIDDLE,
    JOYSTICK_DIRECTION_UP,
    JOYSTICK_DIRECTION_DOWN,
    JOYSTICK_DIRECTION_LEFT,
    JOYSTICK_DIRECTION_RIGHT,
    JOYSTICK_DIRECTION_LEFTUP,
    JOYSTICK_DIRECTION_RIGHTUP,
    JOYSTICK_DIRECTION_LEFTDOWN,
    JOYSTICK_DIRECTION_RIGHTDOWN
} JoystickDirection;

typedef enum {
    JOYSTICK_DEFLECTION_NO,
    JOYSTICK_DEFLECTION_LOW,
    JOYSTICK_DEFLECTION_HIGH
} JoystickDeflection;

typedef struct {
    JoystickDirection direction;
    JoystickDeflection x_abs_defl;  /* Абсолютное отклонение по x */
    JoystickDeflection y_abs_defl;  /* Абсолютное отклонение по y */
} JoystickData;

void joystick_start(void);
void joystick_update(JoystickData*);
void joystick_data_to_robot_movement(const JoystickData*, DataToRobot*);

#endif
