#include <stm8s.h>

#include "joystick.h"
#include "halutils.h"
#include "config.h"

typedef enum {
    JOY_DIR_MIDDLE,
    JOY_DIR_UP,
    JOY_DIR_DOWN,
    JOY_DIR_LEFT,
    JOY_DIR_RIGHT,
    JOY_DIR_LEFTUP,
    JOY_DIR_RIGHTUP,
    JOY_DIR_LEFTDOWN,
    JOY_DIR_RIGHTDOWN
} joy_direction_t;

typedef enum {
    JOY_DEFL_NO,
    JOY_DEFL_LOW,
    JOY_DEFL_HIGH
} joy_deflection_t;

static struct {
    joy_direction_t direction;
    joy_deflection_t x_abs_defl;  // Абсолютное отклонение по x
    joy_deflection_t y_abs_defl;  // Абсолютное отклонение по y
} joystick;

// Подгонка для осей джойстика.
#define CALIBRATION_VAL_X   (15)
#define CALIBRATION_VAL_Y   (17)

static joy_direction_t joystick_get_direction(uint8_t x, uint8_t y);
static joy_deflection_t joystick_get_deflection(uint8_t axis_value);

void joystick_update(void)
{
    static ADC1_Channel_TypeDef channel_to_watch = JOYSTICK_X_ADC_CH;
    static bool new_x_available = FALSE, new_y_available = FALSE;
    static uint8_t x, y;

    if (ADC1->CSR & ADC1_FLAG_EOC) {
        int8_t calib_value = (channel_to_watch == JOYSTICK_X_ADC_CH) ?
                CALIBRATION_VAL_X : CALIBRATION_VAL_Y;

        uint8_t axis_value = (adc_read_value() + calib_value) * 4 / 1023;

        if (channel_to_watch == JOYSTICK_X_ADC_CH) {
            x = axis_value;
            new_x_available = TRUE;
            channel_to_watch = JOYSTICK_Y_ADC_CH;
        } else {  // channel_to_watch == JOYSTICK_Y_ADC_CH
            y = 4 - axis_value;  // обусловлено конструкцией джойстика
            new_y_available = TRUE;
            channel_to_watch = JOYSTICK_X_ADC_CH;
        }

        adc_start_conversion(channel_to_watch);
    }

    if (new_x_available && new_y_available) {
        joystick.direction = joystick_get_direction(x, y);
        joystick.x_abs_defl = joystick_get_deflection(x);
        joystick.y_abs_defl = joystick_get_deflection(y);
        new_x_available = new_y_available = FALSE;
    }
}

void joystick_form_robot_movement(data_to_robot_t *dtr)
{
    static const uint8_t robot_speeds[3] = {0, 128, 255};

    // Вычисляем направление.
    switch (joystick.direction)
    {
    case JOY_DIR_MIDDLE:
        dtr->ctrl.bf.move_dir = MOVEDIR_NONE;
        break;
    case JOY_DIR_UP:
        dtr->ctrl.bf.move_dir = MOVEDIR_FORWARD;
        break;
    case JOY_DIR_DOWN:
        dtr->ctrl.bf.move_dir = MOVEDIR_BACKWARD;
        break;
    case JOY_DIR_LEFT:
    case JOY_DIR_LEFTUP:
    case JOY_DIR_RIGHTDOWN:
        dtr->ctrl.bf.move_dir = MOVEDIR_LEFTWARD;
        break;
    case JOY_DIR_RIGHT:
    case JOY_DIR_RIGHTUP:
    case JOY_DIR_LEFTDOWN:
        dtr->ctrl.bf.move_dir = MOVEDIR_RIGHTWARD;
        break;
    }

    // Вычисляем скорости.
    switch (joystick.direction)
    {
    case JOY_DIR_MIDDLE:
        dtr->speed_left  = robot_speeds[0];
        dtr->speed_right = robot_speeds[0];
        break;
    case JOY_DIR_LEFTUP:
    case JOY_DIR_LEFTDOWN:
        dtr->speed_left  = robot_speeds[1];
        dtr->speed_right = robot_speeds[joystick.y_abs_defl];
        break;
    case JOY_DIR_RIGHTUP:
    case JOY_DIR_RIGHTDOWN:
        dtr->speed_left  = robot_speeds[joystick.y_abs_defl];
        dtr->speed_right = robot_speeds[1];
        break;
    case JOY_DIR_UP:
    case JOY_DIR_DOWN:
        dtr->speed_left  = robot_speeds[joystick.y_abs_defl];
        dtr->speed_right = robot_speeds[joystick.y_abs_defl];
        break;
    case JOY_DIR_LEFT:
    case JOY_DIR_RIGHT:
        dtr->speed_left  = robot_speeds[joystick.x_abs_defl];
        dtr->speed_right = robot_speeds[joystick.x_abs_defl];
        break;
    }
}

static joy_direction_t joystick_get_direction(uint8_t x, uint8_t y)
{
    joy_direction_t direction = JOY_DIR_MIDDLE;

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

static joy_deflection_t joystick_get_deflection(uint8_t axis_value)
{
    if (axis_value < 1 || axis_value > 3)
        return JOY_DEFL_HIGH;
    else if (axis_value < 2 || axis_value > 2)
        return JOY_DEFL_LOW;
    else
        return JOY_DEFL_NO;
}