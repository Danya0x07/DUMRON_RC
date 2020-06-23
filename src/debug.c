#include <stm8s.h>

extern char *itoa(int, unsigned char);

static inline void uart_send_byte(uint8_t byte)
{
    while (!UART2_GetFlagStatus(UART2_FLAG_TXE));
    UART2_SendData8(byte);
}

void debug_logs(const char *str)
{
    while (*str) {
        uart_send_byte(*str++);
    }
}

void debug_logi(int n)
{
    debug_logs(itoa(n, 10));
}

void debug_logx(int n)
{
    debug_logs(itoa(n, 16));
}