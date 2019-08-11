#include "msgprotocol.h"

/* Буфер для проверки новизны данных. */
static DataToRobot buffer;

void robot_calc_movement(DataToRobot* data, const JoystickData* joystick_data)
{
    static const uint8_t robot_speeds[3] = {0, 128, 255};
    /* Вычислить направление */
    switch (joystick_data->direction) {
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
    switch (joystick_data->direction) {
        case MIDDLE:
            data->speed_left  = robot_speeds[0];
            data->speed_right = robot_speeds[0];
            break;
        case LEFTUP:
        case LEFTDOWN:
            data->speed_left  = robot_speeds[joystick_data->y_abs];
            data->speed_right = robot_speeds[1];
            break;
        case RIGHTUP:
        case RIGHTDOWN:
            data->speed_left  = robot_speeds[1];
            data->speed_right = robot_speeds[joystick_data->y_abs];
            break;
        case UP:
        case DOWN:
            data->speed_left  = robot_speeds[joystick_data->y_abs];
            data->speed_right = robot_speeds[joystick_data->y_abs];
            break;
        case LEFT:
        case RIGHT:
            data->speed_left  = robot_speeds[joystick_data->x_abs];
            data->speed_right = robot_speeds[joystick_data->x_abs];
            break;
    }
}

bool robot_data_is_new(const DataToRobot* data)
{
    return (data->direction != buffer.direction
            || data->speed_left   != buffer.speed_left
            || data->speed_right  != buffer.speed_right
            || data->periph_state != buffer.periph_state);
}

void robot_data_update_buffer(const DataToRobot* data)
{
    buffer.direction    = data->direction;
    buffer.speed_left   = data->speed_left;
    buffer.speed_right  = data->speed_right;
    buffer.periph_state = data->periph_state;
}

