#include "delay.h"

static void delay_1ms(void)
{
    TIM4_SetCounter(0);
    while (TIM4_GetCounter() < 125)
        ;
}

void delay_ms(uint16_t ms)
{
    while (ms--)
        delay_1ms();
}
