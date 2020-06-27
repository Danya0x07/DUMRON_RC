#ifndef _JOYSTICK_H
#define _JOYSTICK_H

#include "protocol.h"

void joystick_update(void);
void joystick_form_robot_movement(data_to_robot_t *dtr);

#endif  /* _JOYSTICK_H */