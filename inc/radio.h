#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include <stm8s.h>
#include "robot_interface.h"

void radio_init(void);
void radio_send_data(DataToRobot*);

#endif
