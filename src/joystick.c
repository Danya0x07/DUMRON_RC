#include <stm8s.h>
#include "joystick.h"

#define JOYSTICK_X_CHANNEL   ADC1_CHANNEL_0
#define JOYSTICK_Y_CHANNEL   ADC1_CHANNEL_1

/* Подгонка для осей джойстика. */
#define JOYSTICK_X_CALIBRATION_VAL (15)
#define JOYSTICK_Y_CALIBRATION_VAL (17)

#define adc_select_channel(adc_channel) \
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, adc_channel, ADC1_ALIGN_RIGHT)

static JoystickDirection joystick_get_direction(uint8_t, uint8_t);
static JoystickDeflection joystick_get_deflection(uint8_t);

void joystick_start(void)
{
    ADC1_StartConversion();
}

void joystick_update(JoystickData* joystick_data)
{
    static ADC1_Channel_TypeDef channel_to_watch = JOYSTICK_X_CHANNEL;
    static bool new_x_available = FALSE, new_y_available = FALSE;
    static uint8_t x_buff, y_buff;

    if (ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
        int8_t calibration_val = (channel_to_watch == JOYSTICK_X_CHANNEL) ?
                                  JOYSTICK_X_CALIBRATION_VAL : JOYSTICK_Y_CALIBRATION_VAL;
        uint8_t axis_value = (ADC1_GetConversionValue() + calibration_val) * 4 / 1023;
        if (channel_to_watch == JOYSTICK_X_CHANNEL) {
            x_buff = axis_value;
            new_x_available = TRUE;
            channel_to_watch = JOYSTICK_Y_CHANNEL;
        } else {  /* channel_to_watch == JOYSTICK_Y_CHANNEL */
            y_buff = 4 - axis_value;  /* из-за конструкции джойстика */
            new_y_available = TRUE;
            channel_to_watch = JOYSTICK_X_CHANNEL;
        }
        adc_select_channel(channel_to_watch);
        ADC1_StartConversion();
    }

    if (new_x_available && new_y_available) {
        joystick_data->direction = joystick_get_direction(x_buff, y_buff);
        joystick_data->x_abs_defl = joystick_get_deflection(x_buff);
        joystick_data->y_abs_defl = joystick_get_deflection(y_buff);
        new_x_available = new_y_available = FALSE;
    }
}

void joystick_data_to_robot_movement(const JoystickData* joystick_data, DataToRobot* data_to_robot)
{
    static const uint8_t robot_speeds[3] = {0, 128, 255};
    /* Вычислить направление */
    switch (joystick_data->direction) {
        case JOYSTICK_DIRECTION_MIDDLE:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_NONE;
            break;
        case JOYSTICK_DIRECTION_UP:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_FORWARD;
            break;
        case JOYSTICK_DIRECTION_DOWN:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_BACKWARD;
            break;
        case JOYSTICK_DIRECTION_LEFT:
        case JOYSTICK_DIRECTION_LEFTUP:
        case JOYSTICK_DIRECTION_RIGHTDOWN:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_LEFTWARD;
            break;
        case JOYSTICK_DIRECTION_RIGHT:
        case JOYSTICK_DIRECTION_RIGHTUP:
        case JOYSTICK_DIRECTION_LEFTDOWN:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_RIGHTWARD;
            break;
    }
    /* Вычислить скорости */
    switch (joystick_data->direction) {
        case JOYSTICK_DIRECTION_MIDDLE:
            data_to_robot->speed_left  = robot_speeds[0];
            data_to_robot->speed_right = robot_speeds[0];
            break;
        case JOYSTICK_DIRECTION_LEFTUP:
        case JOYSTICK_DIRECTION_LEFTDOWN:
            data_to_robot->speed_left  = robot_speeds[1];
            data_to_robot->speed_right = robot_speeds[joystick_data->y_abs_defl];
            break;
        case JOYSTICK_DIRECTION_RIGHTUP:
        case JOYSTICK_DIRECTION_RIGHTDOWN:
            data_to_robot->speed_left  = robot_speeds[joystick_data->y_abs_defl];
            data_to_robot->speed_right = robot_speeds[1];
            break;
        case JOYSTICK_DIRECTION_UP:
        case JOYSTICK_DIRECTION_DOWN:
            data_to_robot->speed_left  = robot_speeds[joystick_data->y_abs_defl];
            data_to_robot->speed_right = robot_speeds[joystick_data->y_abs_defl];
            break;
        case JOYSTICK_DIRECTION_LEFT:
        case JOYSTICK_DIRECTION_RIGHT:
            data_to_robot->speed_left  = robot_speeds[joystick_data->x_abs_defl];
            data_to_robot->speed_right = robot_speeds[joystick_data->x_abs_defl];
            break;
    }
}

static JoystickDirection joystick_get_direction(uint8_t x, uint8_t y)
{
    JoystickDirection direction = JOYSTICK_DIRECTION_MIDDLE;
    if (x == 2) {
        if (y > 2)
            direction = JOYSTICK_DIRECTION_UP;
        else if (y < 2)
            direction = JOYSTICK_DIRECTION_DOWN;
    } else {
        if (x > 2) {
            if (y > 2)
                direction = JOYSTICK_DIRECTION_RIGHTUP;
            else if (y < 2)
                direction = JOYSTICK_DIRECTION_RIGHTDOWN;
            else
                direction = JOYSTICK_DIRECTION_RIGHT;
        } else if (x < 2) {
            if (y > 2)
                direction = JOYSTICK_DIRECTION_LEFTUP;
            else if (y < 2)
                direction = JOYSTICK_DIRECTION_LEFTDOWN;
            else
                direction = JOYSTICK_DIRECTION_LEFT;
        }
    }
    return direction;
}

static JoystickDeflection joystick_get_deflection(uint8_t axis_value)
{
    if(axis_value < 1 || axis_value > 3)
        return JOYSTICK_DEFLECTION_HIGH;
    else if(axis_value < 2 || axis_value > 2)
        return JOYSTICK_DEFLECTION_LOW;
    else return JOYSTICK_DEFLECTION_NO;
}
