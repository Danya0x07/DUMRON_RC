#include "delay.h"

static void delay_ms_8bit(uint8_t ms)
{
    TIM4_SetCounter(0);
    while (TIM4_GetCounter() < ms)
        ;
}

void delay_ms(uint16_t ms)
{
    uint8_t ms_msb = ms >> 8;
    uint8_t ms_lsb = ms & 0xFF;
    uint8_t ovf = ms_msb;

    while (ms_msb--)
        delay_ms_8bit(0xFF);

    delay_ms_8bit(ovf);
    delay_ms_8bit(ms_lsb);
}