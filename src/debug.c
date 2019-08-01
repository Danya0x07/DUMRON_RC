#include "debug.h"

void uart_init(void)
{
    UART2_DeInit();
    UART2_Init(9600,
               UART2_WORDLENGTH_8D,
               UART2_STOPBITS_1,
               UART2_PARITY_NO,
               UART2_SYNCMODE_CLOCK_DISABLE,
               UART2_MODE_TXRX_ENABLE);
}

void uart_write_byte(uint8_t byte)
{
    while (!UART2_GetFlagStatus(UART2_FLAG_TXE));
    UART2_SendData8(byte);
}

void uart_write_str(const char* str)
{
    while (*str) {
        uart_write_byte(*str++);
    }
}
