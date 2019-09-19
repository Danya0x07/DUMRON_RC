#include "battery.h"
#include "debug.h"

#define BATTERY_CHANNEL ADC1_CHANNEL_2

static uint8_t battery_measure(void);

void battery_init(void)
{
    TIM2_TimeBaseInit(TIM2_PRESCALER_32768, 60000);
    TIM2_Cmd(ENABLE);
}

uint8_t battery_get_voltage(void)
{
    static uint8_t voltage = 100;
    if (TIM2_GetCounter() > 300) {
        TIM2_SetCounter(0);
        voltage = battery_measure();
        led_blink(1, 200);
    }
    return voltage;
}

static uint8_t battery_measure(void)
{
    uint8_t old_csr = ADC1->CSR & 0x0F;
    uint8_t voltage;
    while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC));
    (void) ADC1_GetConversionValue();
    
    /*ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, BATTERY_CHANNEL, ADC1_ALIGN_RIGHT);*/
    ADC1->CSR &= 0xF0;
    ADC1->CSR |= BATTERY_CHANNEL;
    ADC1_StartConversion();
    while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC));
    voltage = ADC1_GetConversionValue() * 100 / 1023;

    ADC1->CSR &= 0xF0;
    ADC1->CSR |= old_csr;
    ADC1_StartConversion();
    
    return voltage;
}
