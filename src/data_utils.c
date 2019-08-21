#include "data_utils.h"
#include "robot_interface.h"

/* Буфер для проверки новизны данных. */
static DataToRobot buffer_to_robot;

bool data_to_robot_is_new(const DataToRobot* data_to_robot)
{
    return (data_to_robot->direction     != buffer_to_robot.direction   ||
            data_to_robot->speed_left    != buffer_to_robot.speed_left  ||
            data_to_robot->speed_right   != buffer_to_robot.speed_right ||
            data_to_robot->control_flags != buffer_to_robot.control_flags);
}

void data_to_robot_update_buffer(const DataToRobot* data_to_robot)
{
    buffer_to_robot.direction     = data_to_robot->direction;
    buffer_to_robot.speed_left    = data_to_robot->speed_left;
    buffer_to_robot.speed_right   = data_to_robot->speed_right;
    buffer_to_robot.control_flags = data_to_robot->control_flags;
}
