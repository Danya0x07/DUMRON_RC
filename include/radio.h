/**
 * Здесь интерфейс для управления радиосвязью.
 */

#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include "robot_interface.h"

void radio_init(void);
void radio_send_data(data_to_robot_s*);
bool radio_data_to_robot_is_new(const data_to_robot_s*);
bool radio_is_time_to_update_io_data(void);
bool radio_check_for_incoming(data_from_robot_s*);

#endif
