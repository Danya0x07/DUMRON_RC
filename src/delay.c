#include "delay.h"

void delay_ms(uint16_t ms)
{
    TIM1_SetCounter(0);
    while (TIM1_GetCounter() < ms)
        ;
}
