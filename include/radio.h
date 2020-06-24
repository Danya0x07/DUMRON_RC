#ifndef _RADIO_H
#define _RADIO_H

#include "protocol.h"

void radio_init(void);
void radio_send(data_to_robot_s *outcoming);
bool radio_out_data_is_new(const data_to_robot_s *outcoming);
bool radio_is_time_to_communicate(void);
bool radio_check_ack(data_from_robot_s *incoming);

#endif  /* _RADIO_H */