#include "joystick.h"
#include <stm8s.h>

#define JOYSTICK_X_CHANNEL   ADC1_CHANNEL_0
#define JOYSTICK_Y_CHANNEL   ADC1_CHANNEL_1

/* Подгонка для осей джойстика. */
#define JOYSTICK_X_CALIBRATION_VAL 0
#define JOYSTICK_Y_CALIBRATION_VAL 12

#define adc_select_channel(adc_channel) \
 ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, adc_channel, ADC1_ALIGN_RIGHT)

static JoystickDirection joystick_direction(uint8_t, uint8_t);
static JoystickAbsDeflection joystick_abs_deflection(uint8_t);

void joystick_init(void)
{
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, JOYSTICK_X_CHANNEL, ADC1_ALIGN_RIGHT);
    ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D10);
    ADC1_Cmd(ENABLE);
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
        uint8_t axis_val = (ADC1_GetConversionValue() + calibration_val) * 4 / 1023;
        if (channel_to_watch == JOYSTICK_X_CHANNEL) {
            x_buff = axis_val;
            new_x_available = TRUE;
            channel_to_watch = JOYSTICK_Y_CHANNEL;
        } else {  /* channel_to_watch == JOYSTICK_Y_CHANNEL */
            y_buff = 4 - axis_val;  /* потому что конструкция джойстика */
            new_y_available = TRUE;
            channel_to_watch = JOYSTICK_X_CHANNEL;
        }
        adc_select_channel(channel_to_watch);
        ADC1_StartConversion();
    }

    if (new_x_available && new_y_available) {
        joystick_data->direction = joystick_direction(x_buff, y_buff);
        joystick_data->x_abs = joystick_abs_deflection(x_buff);
        joystick_data->y_abs = joystick_abs_deflection(y_buff);
        new_x_available = new_y_available = FALSE;
    }
}

static JoystickDirection joystick_direction(uint8_t x, uint8_t y)
{
    JoystickDirection direction = MIDDLE;
    if (x == 2) {
        if (y > 2)
            direction = UP;
        else if (y < 2)
            direction = DOWN;
    } else {
        if (x > 2) {
            if (y > 2)
                direction = RIGHTUP;
            else if (y < 2)
                direction = RIGHTDOWN;
            else
                direction = RIGHT;
        } else if (x < 2) {
            if (y > 2)
                direction = LEFTUP;
            else if (y < 2)
                direction = LEFTDOWN;
            else
                direction = LEFT;
        }
    }
    return direction;
}

static JoystickAbsDeflection joystick_abs_deflection(uint8_t pos)
{
    if(pos < 1 || pos > 3)
        return HIGH_DEFLECTION;
    else if(pos < 2 || pos > 2)
        return LOW_DEFLECTION;
    else return NO_DEFLECTION;
}
