#include "battery.h"
#include "halutils.h"
#include "config.h"

static uint8_t battery_measure(void)
{
    ADC1_Channel_TypeDef prev_channel = ADC1->CSR & 0x0F;
    uint8_t voltage;

    while (!adc_conversion_complete());
    (void)adc_read_value();

    adc_start_conversion(BATTERY_ADC_CH);

    while (!adc_conversion_complete());
    voltage = (uint32_t)adc_read_value() * 100 / 1023;

    adc_start_conversion(prev_channel);

    return voltage;
}

uint8_t battery_get_charge(void)
{
    static uint8_t counter = 0;
    static uint8_t voltage = 100;

    // Слишком часто проверять батарейку не надо.
    if (counter++ == 0) {
        voltage = battery_measure();
    }
    return voltage;
}