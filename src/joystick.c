#include "joystick.h"

#define JOYSTICK_X_CHANNEL   ADC1_CHANNEL_0
#define JOYSTICK_Y_CHANNEL   ADC1_CHANNEL_1

static void adc_select_channel(ADC1_Channel_TypeDef);
static JoystickDirection joystick_direction(uint8_t, uint8_t);
static JoystickAbsDeflection joystick_abs_deflection(uint8_t);

void joystick_init(void)
{
#ifdef OPTIMISATION
    /* ConversionConfig */
    ADC1->CR2 |= (uint8_t)(ADC1_ALIGN_RIGHT);
    adc_select_channel(JOY_X_CHANNEL);
    /* PrescalerConfig  */
    ADC1->CR1 |= (uint8_t)(ADC1_PRESSEL_FCPU_D8);
    /* Enable ADC */
    ADC1->CR1 |= ADC1_CR1_ADON;
#else
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, JOYSTICK_X_CHANNEL, ADC1_ALIGN_RIGHT);
    ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D8);
    ADC1_Cmd(ENABLE);
#endif
}

void joystick_update(JoystickData* joystick_data)
{
    static ADC1_Channel_TypeDef channel_to_watch = JOYSTICK_X_CHANNEL;
    static bool new_x_available = FALSE, new_y_available = FALSE;
    static uint8_t x_buff, y_buff;
    
    if (ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
        uint8_t axis_val = ADC1_GetConversionValue() * 4 / 1023;
        if (channel_to_watch == JOYSTICK_X_CHANNEL) {
            x_buff = axis_val;
            new_x_available = TRUE;
            channel_to_watch = JOYSTICK_Y_CHANNEL;
        } else {  /* channel_to_watch == JOYSTICK_Y_CHANNEL */
            y_buff = axis_val;
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

static void adc_select_channel(ADC1_Channel_TypeDef channel)
{
    ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
    ADC1->CSR |= (uint8_t)(channel);
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
