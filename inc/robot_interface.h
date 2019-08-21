#ifndef ROBOT_INTERFACE_H_INCLUDED
#define ROBOT_INTERFACE_H_INCLUDED

#include <stm8s.h>

typedef enum {
    ROBOT_DIRECTION_NONE,
    ROBOT_DIRECTION_FORWARD,
    ROBOT_DIRECTION_BACKWARD,
    ROBOT_DIRECTION_LEFTWARD,
    ROBOT_DIRECTION_RIGHTWARD
} RobotDirection;

#define ROBOT_FLAG_ARM_DOWN (1 << 0)
#define ROBOT_FLAG_ARM_UP   (1 << 1)
#define ROBOT_FLAG_CLAW_SQUEEZE (1 << 2)
#define ROBOT_FLAG_CLAW_RELEASE (1 << 3)
#define ROBOT_FLAG_ARM_FOLD  (1 << 4)
#define ROBOT_FLAG_LIGHTS_EN (1 << 5)
#define ROBOT_FLAG_KLAXON_EN (1 << 6)

typedef struct {
    RobotDirection direction;
    uint8_t speed_left;
    uint8_t speed_right;
    uint8_t control_flags;
} DataToRobot;

typedef struct {
    uint8_t battery_motors;
    uint8_t battery_brains;
    int8_t  temperature;
} DataFromRobot;

#endif
