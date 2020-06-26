#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "protocol.h"

void display_init(void);
void display_update(const data_to_robot_t *, const data_from_robot_t *,
                    bool ack_received, uint8_t battery_lvl);

#endif /* _DISPLAY_H */