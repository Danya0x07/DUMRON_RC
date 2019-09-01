/**
 * Здесь интерфейс для управления радиосвязью.
 */

#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include <stm8s.h>
#include "robot_interface.h"

void radio_init(void);
bool radio_send_data(DataToRobot*);
bool radio_data_to_robot_is_new(const DataToRobot*);
bool radio_is_time_to_update_io_data(void);
void radio_check_for_incoming(DataFromRobot*);

#endif
