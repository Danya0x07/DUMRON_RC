/**
 * Здесь интерфейс управления графическим интерфейсом.
 */

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include <stm8s.h>
#include "robot_interface.h"

void display_init(void);
void display_update(const DataToRobot*, const DataFromRobot*,
                    bool was_conn_error, uint8_t battery_lvl);

#endif
