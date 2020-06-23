#ifndef JOYSTICK_H_INCLUDED
#define JOYSTICK_H_INCLUDED

#include "robot_interface.h"

typedef enum {
    JOY_DIR_MIDDLE,
    JOY_DIR_UP,
    JOY_DIR_DOWN,
    JOY_DIR_LEFT,
    JOY_DIR_RIGHT,
    JOY_DIR_LEFTUP,
    JOY_DIR_RIGHTUP,
    JOY_DIR_LEFTDOWN,
    JOY_DIR_RIGHTDOWN
} joystick_direction_e;

typedef enum {
    JOY_DEFL_NO,
    JOY_DEFL_LOW,
    JOY_DEFL_HIGH
} joystick_deflection_e;

typedef struct {
    joystick_direction_e direction;
    joystick_deflection_e x_abs_defl;  /* Абсолютное отклонение по x */
    joystick_deflection_e y_abs_defl;  /* Абсолютное отклонение по y */
} joystick_data_s;

void joystick_start(void);
void joystick_update(joystick_data_s *);
void joystick_data_to_robot_movement(const joystick_data_s *,
                                     data_to_robot_s *);

#endif
