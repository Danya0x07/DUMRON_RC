#include "joystic.h"

#define JOY_X_CHANNEL   ADC1_CHANNEL_0
#define JOY_Y_CHANNEL   ADC1_CHANNEL_1

struct Joystic joystic;

static void adc_select_channel(ADC1_Channel_TypeDef);
static JoysticDirection joystic_direction(uint8_t, uint8_t);
static uint8_t joystic_axis_abs(uint8_t);

void joystic_init(void)
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
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, JOY_X_CHANNEL, ADC1_ALIGN_RIGHT);
    ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D8);
    ADC1_Cmd(ENABLE);
#endif
}

void joystic_update(void)
{
    static ADC1_Channel_TypeDef watched_channel = JOY_X_CHANNEL;
    static bool new_x_available = FALSE, new_y_available = FALSE;
    static uint8_t x_buff, y_buff;
    
    if (ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
        uint8_t axis_val = ADC1_GetConversionValue() * 4 / 1023;
        if (watched_channel == JOY_X_CHANNEL) {
            x_buff = axis_val;
            new_x_available = TRUE;
            watched_channel = JOY_Y_CHANNEL;
        } else {  /* watched_channel == JOY_Y_CHANNEL */
            y_buff = axis_val;
            new_y_available = TRUE;
            watched_channel = JOY_X_CHANNEL;
        }
        adc_select_channel(watched_channel);
        ADC1_StartConversion();
    }

    if (new_x_available && new_y_available) {
        joystic.direction = joystic_direction(x_buff, y_buff);
        joystic.x_abs = joystic_axis_abs(x_buff);
        joystic.y_abs = joystic_axis_abs(y_buff);
        new_x_available = new_y_available = FALSE;
    }
}

static void adc_select_channel(ADC1_Channel_TypeDef channel)
{
    ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
    ADC1->CSR |= (uint8_t)(channel);
}

static JoysticDirection joystic_direction(uint8_t x, uint8_t y)
{
    JoysticDirection direction = NONE;
    if(x == 2) {
        if(y > 2)
            direction = FORWARD;
        else if(y < 2)
            direction = BACKWARD;
        else
            direction = NONE;
    } else {
        if(x > 2) {
            if(y > 2)
                direction = RIGHTFRONT;
            else if(y < 2)
                direction = RIGHTBACK;
            else
                direction = RIGHTWARD;
        } else if(x < 2) {
            if(y > 2)
                direction = LEFTFRONT;
            else if(y < 2)
                direction = LEFTBACK;
            else
                direction = LEFTWARD;
        }
    }
    return direction;
}

static uint8_t joystic_axis_abs(uint8_t pos)
{
    if(pos < 1 || pos > 3)
        return 2;
    else if(pos < 2 || pos > 2)
        return 1;
    else return 0;
}
