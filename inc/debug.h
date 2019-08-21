#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stm8s.h>

#define LED_GPIO    GPIOE
#define LED_PIN     GPIO_PIN_5

#define led_on()    GPIO_WriteLow(LED_GPIO, LED_PIN)
#define led_off()   GPIO_WriteHigh(LED_GPIO, LED_PIN)
#define led_toggle()    GPIO_WriteReverse(LED_GPIO, LED_PIN)

void debug_init(void);
void uart_send_str(const char*);
void uart_send_int(int);

#define logs(str)   uart_send_str(str)
#define logi(n)     uart_send_int(n)

#endif
