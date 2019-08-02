#include "msgprotocol.h"


void robot_calc_movement(DataToRobot* data, const Joystick* joystick)
{
    static const uint8_t robot_speeds[3] = {0, 128, 255};
    /* Вычислить направление */
    switch (joystick->direction) {
        case UP:
            data->direction = FORWARD;
            break;
        case DOWN:
            data->direction = BACKWARD;
            break;
        case LEFT:
        case LEFTUP:
        case RIGHTDOWN:
            data->direction = LEFTWARD;
            break;
        case RIGHT:
        case RIGHTUP:
        case LEFTDOWN:
            data->direction = RIGHTWARD;
            break;
    }
    /* Вычислить скорости */
    switch (joystick->direction) {
        case MIDDLE:
            data->speed_left  = robot_speeds[0];
            data->speed_right = robot_speeds[0];
            break;
        case LEFTUP:
        case LEFTDOWN:
            data->speed_left  = robot_speeds[joystick->y_abs];
            data->speed_right = robot_speeds[1];
            break;
        case RIGHTUP:
        case RIGHTDOWN:
            data->speed_left  = robot_speeds[1];
            data->speed_right = robot_speeds[joystick->y_abs];
            break;
        case UP:
        case DOWN:
            data->speed_left  = robot_speeds[joystick->y_abs];
            data->speed_right = robot_speeds[joystick->y_abs];
            break;
        case LEFT:
        case RIGHT:
            data->speed_left  = robot_speeds[joystick->x_abs];
            data->speed_right = robot_speeds[joystick->x_abs];
            break;
    }
}

bool robot_data_is_sync(const DataToRobot* data1, const DataToRobot* data2)
{
    return (data1->direction == data2->direction
            && data1->speed_left   == data2->speed_left
            && data1->speed_right  == data2->speed_right
            && data1->periph_state == data2->periph_state);
}

void robot_data_sync(DataToRobot* dst_data, const DataToRobot* src_data)
{
    dst_data->direction    = src_data->direction;
    dst_data->speed_left   = src_data->speed_left;
    dst_data->speed_right  = src_data->speed_right;
    dst_data->periph_state = src_data->periph_state;
}

