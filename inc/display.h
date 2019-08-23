/*
 * Здесь интерфейс управления графическим интерфейсом.
 */

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include "robot_interface.h"

void display_init(void);
void display_test(void);
void display_update(const DataToRobot*, const DataFromRobot*);

#endif
