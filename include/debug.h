#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stm8s.h>

void debug_init(void);
void uart_send_str(const char*);
void uart_send_int(int);
#define logs(str)   uart_send_str(str)
#define logi(n)     uart_send_int(n)

#define led_on()    GPIO_WriteLow(GPIOE, GPIO_PIN_5)
#define led_off()   GPIO_WriteHigh(GPIOE, GPIO_PIN_5)
#define led_toggle() GPIO_WriteReverse(GPIOE, GPIO_PIN_5)
void led_blink(uint8_t times, uint16_t delay);

#define buzzer_on()     GPIO_WriteHigh(GPIOD, GPIO_PIN_7)
#define buzzer_off()    GPIO_WriteLow(GPIOD, GPIO_PIN_7)
#define buzzer_toggle() GPIO_WriteReverse(GPIOD, GPIO_PIN_7)
void buzzer_peep(uint8_t times, uint8_t delay);

#endif
