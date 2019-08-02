#ifndef MSGPROTOCOL_H_INCLUDED
#define MSGPROTOCOL_H_INCLUDED

#include <stm8s.h>
#include "joystick.h"

typedef enum {
    NONE,
    FORWARD,
    BACKWARD,
    LEFTWARD,
    RIGHTWARD
} RobotDirection;

typedef enum {
    ARM_DOWN = 1 << 0,
    ARM_UP   = 1 << 1,
    CLAW_SQUEEZE = 1 << 2,
    CLAW_RELEASE = 1 << 3,
    ARM_FOLD  = 1 << 4,
    LIGHTS_EN = 1 << 5,
    KLAXON_EN = 1 << 6,
} RobotPeriphFlag;

typedef struct {
    RobotDirection direction;
    uint8_t speed_left;
    uint8_t speed_right;
    uint8_t periph_state;
} DataToRobot;

void robot_calc_movement(DataToRobot* data, const Joystick* joystick);
bool robot_data_is_sync(const DataToRobot*, const DataToRobot*);
void robot_data_sync(DataToRobot* dst_data, const DataToRobot* src_data);

#endif
