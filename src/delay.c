#include "delay.h"

static volatile uint16_t time_count = 0;

void delay_ms(uint16_t ms)
{
    time_count = ms;
    while (time_count);
}

void delay_interrupt_handler(void) __interrupt(23)
{
    if (time_count)
        time_count--;
    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}
