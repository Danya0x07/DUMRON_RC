#include "delay.h"

static volatile uint16_t time_count = 0;

void delay_init(void)
{
    TIM4_TimeBaseInit(TIM4_PRESCALER_64, 124);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);
}

void delay_ms(uint16_t ms)
{
    time_count = ms;
    while (time_count);
}

void tim4_interrupt_handler(void) __interrupt(23)
{
    if (time_count)
        time_count--;
    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}
