#ifndef _KEYPAD_H
#define _KEYPAD_H

#include "protocol.h"

void keypad_update(void);
void keypad_form_robot_state(data_to_robot_t *dtr);

#endif