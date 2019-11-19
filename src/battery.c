#include "battery.h"
#include "debug.h"

#define BATTERY_CHANNEL ADC1_CHANNEL_2

static uint8_t battery_measure(void);

uint8_t battery_get_voltage(void)
{
    static uint8_t voltage = 100;
    if (TIM2_GetCounter() > 300) {
        TIM2_SetCounter(0);
        voltage = battery_measure();
        /*led_blink(1, 200);*/
    }
    return voltage;
}

static uint8_t battery_measure(void)
{
    ADC1_Channel_TypeDef prev_channel = ADC1->CSR & 0x0F;
    uint8_t voltage;

    while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC));
    (void) ADC1_GetConversionValue();

    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
              BATTERY_CHANNEL, ADC1_PRESSEL_FCPU_D10,
              ADC1_EXTTRIG_TIM, DISABLE,
              ADC1_ALIGN_RIGHT,
              ADC1_SCHMITTTRIG_CHANNEL2, DISABLE);
    ADC1_StartConversion();

    while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC));
    voltage = (uint32_t) ADC1_GetConversionValue() * 100 / 1023;

    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, prev_channel, ADC1_ALIGN_RIGHT);
    ADC1_StartConversion();

    return voltage;
}
