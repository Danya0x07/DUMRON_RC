#include "halutils.h"

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


#define MAX_NUMBER_OF_DIGITS 16

char *itoa(int value, unsigned char radix)
{
    static char string[MAX_NUMBER_OF_DIGITS + 1];
    char buffer[MAX_NUMBER_OF_DIGITS + 1];
    const char hex_digit[] = "0123456789ABCDEF";
    char *pstr = string;
    char *pbuff = buffer;

    if (value < 0) {
        *pstr++ = '-';
        value = -value;
    }

    *pbuff++ = '\0';

    // заполняем buffer строчным представлением числа в обратном порядке
    do {
        *pbuff++ = hex_digit[value % radix];
        value /= radix;
    } while (value != 0);

    // копируем buffer в string задом наперёд
    do {
        *pstr++ = *--pbuff;
    } while (*pbuff != '\0');

    return string;
}


void adc_start_conversion(ADC1_Channel_TypeDef channel)
{
    ADC1->CSR = channel;
    ADC1->CR1 |= ADC1_CR1_ADON;
}

uint16_t adc_read_value(void)
{
    uint16_t temph = 0;
    uint8_t templ = 0;

    templ = ADC1->DRL;
    temph = ADC1->DRH;
    temph = temph << 8 | templ;

    return temph;
}
