#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "protocol.h"

void display_init(void);
void display_update(const data_to_robot_s *, const data_from_robot_s *,
                    bool ack_received, uint8_t battery_lvl);

#endif /* _DISPLAY_H */