#include "emitters.h"
#include "delay.h"

#define led_on()        GPIO_WriteLow(LED_GPORT, LED_GPIN)
#define led_off()       GPIO_WriteHigh(LED_GPORT, LED_GPIN)
#define led_toggle()    GPIO_WriteReverse(LED_GPORT, LED_GPIN)

#define buzzer_on()     GPIO_WriteHigh(BUZZER_GPORT, BUZZER_GPIN)
#define buzzer_off()    GPIO_WriteLow(BUZZER_GPORT, BUZZER_GPIN)
#define buzzer_toggle() GPIO_WriteReverse(BUZZER_GPORT, BUZZER_GPIN)

void led_blink(uint8_t times, uint16_t delay)
{
    while (times--) {
        led_on();
        delay_ms(delay);
        led_off();
        delay_ms(delay);
    }
}

void buzzer_beep(uint8_t times, uint16_t delay)
{
    while (times--) {
        buzzer_on();
        delay_ms(delay);
        buzzer_off();
        delay_ms(delay);
    }
}
