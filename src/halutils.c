#include "halutils.h"

void tim2_set_counter(uint16_t value)
{
    TIM2->CNTRH = (uint8_t)(value >> 8);
    TIM2->CNTRL = (uint8_t)value;
}

uint16_t tim2_get_counter(void)
{
    uint16_t tmp = (uint16_t)TIM2->CNTRH << 8;
    return tmp | TIM2->CNTRL;
}

void tim3_set_counter(uint16_t value)
{
    TIM3->CNTRH = (uint8_t)(value >> 8);
    TIM3->CNTRL = (uint8_t)value;
}

uint16_t tim3_get_counter(void)
{
    uint16_t tmp = (uint16_t)TIM3->CNTRH << 8;
    return tmp | TIM3->CNTRL;
}

void adc_start_conversion(ADC1_Channel_TypeDef channel)
{
    ADC1->CSR = channel;
    ADC1->CR1 |= ADC1_CR1_ADON;
}

bool adc_conversion_complete(void)
{
    return (ADC1->CSR & ADC1_FLAG_EOC) != 0;
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

uint8_t spi_transfer_byte(uint8_t byte)
{
    while (!(SPI->SR & SPI_FLAG_TXE));
    SPI->DR = byte;
    while (!(SPI->SR & SPI_FLAG_RXNE));
    return SPI->DR;
}

void spi_transfer_bytes(uint8_t *in, const uint8_t *out, uint8_t len)
{
    if (in == NULL) {
        while (len--)
            spi_transfer_byte(*out++);
    } else if (out == NULL) {
        while (len--)
            *in++ = spi_transfer_byte(0);
    } else {
        while (len--)
            *in++ = spi_transfer_byte(*out++);
    }
}

void delay_ms(uint16_t ms)
{
    while (ms--) {
        // задержка на 1ms
        tim4_set_counter(0);
        while (tim4_get_counter() < 125)
            ;
    }
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