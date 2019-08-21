#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include <stm8s.h>
#include "robot_interface.h"
#include "joystick.h"

void display_init(void);
void display_test(char*);
void display_update(const JoystickData*, const DataFromRobot*);

#endif
