#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#define led_on()    GPIO_WriteLow(GPIOE, GPIO_PIN_5)
#define led_off()   GPIO_WriteHigh(GPIOE, GPIO_PIN_5)
#define led_toggle() GPIO_WriteReverse(GPIOE, GPIO_PIN_5)

void debug_init(void);
void uart_send_str(const char*);
void uart_send_int(int);

#define logs(str)   uart_send_str(str)
#define logi(n)     uart_send_int(n)

#endif
