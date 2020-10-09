#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "protocol.h"

void display_init(void);
void display_splash_screen(void);
void display_transceiver_missing(void);
void display_rf_scan_started(void);
void display_rf_scan_ended(void);
void display_update(const data_to_robot_t *, const data_from_robot_t *,
                    bool was_response, uint8_t battery_lvl);

#endif /* _DISPLAY_H */