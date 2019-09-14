#include "debug.h"
#include "delay.h"

extern char* itoa(int, unsigned char);

void debug_init(void)
{
    UART2_Init(9600,
               UART2_WORDLENGTH_8D,
               UART2_STOPBITS_1,
               UART2_PARITY_NO,
               UART2_SYNCMODE_CLOCK_DISABLE,
               UART2_MODE_TXRX_ENABLE);
    GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW);
}

void uart_send_byte(uint8_t byte)
{
    while (!UART2_GetFlagStatus(UART2_FLAG_TXE));
    UART2_SendData8(byte);
}

void uart_send_str(const char* str)
{
    while (*str) {
        while (!UART2_GetFlagStatus(UART2_FLAG_TXE));
        UART2_SendData8(*str++);
    }
}

void uart_send_int(int n)
{
    uart_send_str(itoa(n, 10));
}

void led_blink(uint8_t times, uint16_t delay)
{
    while (times--) {
        led_on();
        delay_ms(delay);
        led_off();
        delay_ms(delay);
    }
}
