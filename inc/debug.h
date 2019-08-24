#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

void debug_init(void);
void uart_send_str(const char*);
void uart_send_int(int);

#define logs(str)   uart_send_str(str)
#define logi(n)     uart_send_int(n)

#endif
