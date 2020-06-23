#include <stm8s.h>
#include "joystick.h"

#define JOYSTICK_X_CHANNEL   ADC1_CHANNEL_0
#define JOYSTICK_Y_CHANNEL   ADC1_CHANNEL_1

/* Подгонка для осей джойстика. */
#define JOYSTICK_X_CALIBRATION_VAL (15)
#define JOYSTICK_Y_CALIBRATION_VAL (17)

#define adc_select_channel(adc_channel) \
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, adc_channel, ADC1_ALIGN_RIGHT)

static joystick_deflection_e joystick_get_direction(uint8_t, uint8_t);
static joystick_deflection_e joystick_get_deflection(uint8_t);

void joystick_start(void)
{
    ADC1_StartConversion();
}

void joystick_update(joystick_data_s* joystick_data)
{
    static ADC1_Channel_TypeDef channel_to_watch = JOYSTICK_X_CHANNEL;
    static bool new_x_available = FALSE, new_y_available = FALSE;
    static uint8_t x_buff, y_buff;

    if (ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
        int8_t calibration_val =
                (channel_to_watch == JOYSTICK_X_CHANNEL) ?
                        JOYSTICK_X_CALIBRATION_VAL : JOYSTICK_Y_CALIBRATION_VAL;

        uint8_t axis_value =
                (ADC1_GetConversionValue() + calibration_val) * 4 / 1023;

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

void joystick_data_to_robot_movement(const joystick_data_s* joystick_data, data_to_robot_s* data_to_robot)
{
    static const uint8_t robot_speeds[3] = {0, 128, 255};
    /* Вычислить направление */
    switch (joystick_data->direction) {
        case JOY_DIR_MIDDLE:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_NONE;
            break;
        case JOY_DIR_UP:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_FORWARD;
            break;
        case JOY_DIR_DOWN:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_BACKWARD;
            break;
        case JOY_DIR_LEFT:
        case JOY_DIR_LEFTUP:
        case JOY_DIR_RIGHTDOWN:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_LEFTWARD;
            break;
        case JOY_DIR_RIGHT:
        case JOY_DIR_RIGHTUP:
        case JOY_DIR_LEFTDOWN:
            data_to_robot->ctrl.bf.moveDir = MOVEDIR_RIGHTWARD;
            break;
    }
    /* Вычислить скорости */
    switch (joystick_data->direction) {
        case JOY_DIR_MIDDLE:
            data_to_robot->speed_left  = robot_speeds[0];
            data_to_robot->speed_right = robot_speeds[0];
            break;
        case JOY_DIR_LEFTUP:
        case JOY_DIR_LEFTDOWN:
            data_to_robot->speed_left  = robot_speeds[1];
            data_to_robot->speed_right = robot_speeds[joystick_data->y_abs_defl];
            break;
        case JOY_DIR_RIGHTUP:
        case JOY_DIR_RIGHTDOWN:
            data_to_robot->speed_left  = robot_speeds[joystick_data->y_abs_defl];
            data_to_robot->speed_right = robot_speeds[1];
            break;
        case JOY_DIR_UP:
        case JOY_DIR_DOWN:
            data_to_robot->speed_left  = robot_speeds[joystick_data->y_abs_defl];
            data_to_robot->speed_right = robot_speeds[joystick_data->y_abs_defl];
            break;
        case JOY_DIR_LEFT:
        case JOY_DIR_RIGHT:
            data_to_robot->speed_left  = robot_speeds[joystick_data->x_abs_defl];
            data_to_robot->speed_right = robot_speeds[joystick_data->x_abs_defl];
            break;
    }
}

static joystick_deflection_e joystick_get_direction(uint8_t x, uint8_t y)
{
    joystick_deflection_e direction = JOY_DIR_MIDDLE;
    if (x == 2) {
        if (y > 2)
            direction = JOY_DIR_UP;
        else if (y < 2)
            direction = JOY_DIR_DOWN;
    } else {
        if (x > 2) {
            if (y > 2)
                direction = JOY_DIR_RIGHTUP;
            else if (y < 2)
                direction = JOY_DIR_RIGHTDOWN;
            else
                direction = JOY_DIR_RIGHT;
        } else if (x < 2) {
            if (y > 2)
                direction = JOY_DIR_LEFTUP;
            else if (y < 2)
                direction = JOY_DIR_LEFTDOWN;
            else
                direction = JOY_DIR_LEFT;
        }
    }
    return direction;
}

static joystick_deflection_e joystick_get_deflection(uint8_t axis_value)
{
    if(axis_value < 1 || axis_value > 3)
        return JOY_DEFL_HIGH;
    else if(axis_value < 2 || axis_value > 2)
        return JOY_DEFL_LOW;
    else return JOY_DEFL_NO;
}
