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

typedef struct {
    RobotDirection direction;
    uint8_t speed_left;
    uint8_t speed_right;
    uint8_t control_reg;  /* control register */
} DataToRobot;

/* control flags */
#define ROBOT_CFLAG_ARM_DOWN (1 << 0)
#define ROBOT_CFLAG_ARM_UP   (1 << 1)
#define ROBOT_CFLAG_CLAW_SQUEEZE (1 << 2)
#define ROBOT_CFLAG_CLAW_RELEASE (1 << 3)
/* #define ROBOT_CFLAG_ARM_FOLD  (1 << 4) */
#define ROBOT_CFLAG_LIGHTS_EN (1 << 5)
#define ROBOT_CFLAG_KLAXON_EN (1 << 6)

typedef struct {
    uint8_t status;
    uint8_t battery_brains;
    uint8_t battery_motors;
    int8_t  temp_ambient;
    int8_t  temp_radiators;
} DataFromRobot;

#define ROBOT_SFLAG_CLIFF   (1 << 0)
#define ROBOT_SFLAG_OBSTACLE    (1 << 1)

#endif
