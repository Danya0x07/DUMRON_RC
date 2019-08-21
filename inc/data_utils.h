#ifndef DATA_UTILS_H_INCLUDED
#define DATA_UTILS_H_INCLUDED

#include "robot_interface.h"

bool data_to_robot_is_new(const DataToRobot*);
void data_to_robot_update_buffer(const DataToRobot*);

#endif
