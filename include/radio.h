/**
 * Здесь интерфейс для управления радиосвязью.
 */

#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include "robot_interface.h"

void radio_init(void);
void radio_send_data(DataToRobot*);
bool radio_data_to_robot_is_new(const DataToRobot*);
bool radio_is_time_to_update_io_data(void);
bool radio_check_for_incoming(DataFromRobot*);

#endif
