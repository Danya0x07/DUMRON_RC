#ifndef _RADIO_H
#define _RADIO_H

#include "protocol.h"

int8_t radio_init(void);
void radio_send(data_to_robot_t *outcoming);
uint8_t radio_select_new_channel(data_to_robot_t *dtr);
bool radio_out_data_is_new(const data_to_robot_t *outcoming);
bool radio_is_time_to_communicate(void);
bool radio_check_response(data_from_robot_t *incoming);
bool radio_current_channel_is_dirty(void);

#endif  /* _RADIO_H */