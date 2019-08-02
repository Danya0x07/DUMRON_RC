#include <stm8s.h>
#include "buttons.h"
#include "debug.h"
#include "joystic.h"


void setup(void)
{
    CLK_DeInit();
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV2);

    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOE);
    GPIO_DeInit(GPIOF);

    ADC1_DeInit();
}


int main(void)
{
    setup();
    
    buttons_init();
    uart_init();
    joystic_init();

    while (1) {
        buttons_checkevents();
        if (buttons_events & BTN_ARMUP_PRESSED) {
            uart_write_str("aup");
        }
        if (buttons_events & BTN_ARMUP_PRESSED_2) {
            uart_write_str("aup2");
        }
        if (buttons_events & BTN_ARMUP_RELEASED) {
            uart_write_str("aur");
        }
        
        if (buttons_events & BTN_ARMDOWN_PRESSED) {
            uart_write_str("adp");
        }
        if (buttons_events & BTN_ARMDOWN_PRESSED_2) {
            uart_write_str("adp2");
        }
        if (buttons_events & BTN_ARMDOWN_RELEASED) {
            uart_write_str("adr");
        }
        
        if (buttons_events & BTN_CLAWSQUEEZE_PRESSED) {
            uart_write_str("csp");
        }
        if (buttons_events & BTN_CLAWRELEASE_PRESSED_2) {
            uart_write_str("csp2");
        }
        if (buttons_events & BTN_CLAWSQUEEZE_RELEASED) {
            uart_write_str("csr");
        }

        if (buttons_events & BTN_CLAWRELEASE_PRESSED) {
            uart_write_str("crp");
        }
        if (buttons_events & BTN_CLAWRELEASE_PRESSED_2) {
            uart_write_str("crp2");
        }
        if (buttons_events & BTN_CLAWRELEASE_RELEASED) {
            uart_write_str("crr");
        }

        if (buttons_events & BTN_KLAXON_PRESSED) {
            uart_write_str("kp");
        }
        if (buttons_events & BTN_KLAXON_PRESSED_2) {
            uart_write_str("kp2");
        }
        if (buttons_events & BTN_KLAXON_RELEASED) {
            uart_write_str("kr");
        }

        if (buttons_events & BTN_TOGGLELIGHTS_PRESSED) {
            uart_write_str("tp");
        }
        if (buttons_events & BTN_TOGGLELIGHTS_PRESSED_2) {
            uart_write_str("tp2");
        }
        if (buttons_events & BTN_TOGGLELIGHTS_RELEASED) {
            uart_write_str("tr");
        }

        joystic_update();
    }
}
