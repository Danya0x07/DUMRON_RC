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
        buttons_update();
        if (buttons_events & BTN_ARMUP_PRESSED) {
            uart_write_str("aup\n");
        }
        if (buttons_events & BTN_ARMUP_PRESSED_2) {
            uart_write_str("aup2\n");
        }
        if (buttons_events & BTN_ARMUP_RELEASED) {
            uart_write_str("aur\n");
        }
        
        if (buttons_events & BTN_ARMDOWN_PRESSED) {
            uart_write_str("adp\n");
        }
        if (buttons_events & BTN_ARMDOWN_PRESSED_2) {
            uart_write_str("adp2\n");
        }
        if (buttons_events & BTN_ARMDOWN_RELEASED) {
            uart_write_str("adr\n");
        }
        
        if (buttons_events & BTN_CLAWSQUEEZE_PRESSED) {
            uart_write_str("csp\n");
        }
        if (buttons_events & BTN_CLAWRELEASE_PRESSED_2) {
            uart_write_str("csp2\n");
        }
        if (buttons_events & BTN_CLAWSQUEEZE_RELEASED) {
            uart_write_str("csr\n");
        }

        if (buttons_events & BTN_CLAWRELEASE_PRESSED) {
            uart_write_str("crp\n");
        }
        if (buttons_events & BTN_CLAWRELEASE_PRESSED_2) {
            uart_write_str("crp2\n");
        }
        if (buttons_events & BTN_CLAWRELEASE_RELEASED) {
            uart_write_str("crr\n");
        }

        if (buttons_events & BTN_KLAXON_PRESSED) {
            uart_write_str("kp\n");
        }
        if (buttons_events & BTN_KLAXON_PRESSED_2) {
            uart_write_str("kp2\n");
        }
        if (buttons_events & BTN_KLAXON_RELEASED) {
            uart_write_str("kr\n");
        }

        if (buttons_events & BTN_TOGGLELIGHTS_PRESSED) {
            uart_write_str("tp\n");
            uart_write_byte(joystic.direction);
            uart_write_byte(joystic.x_abs);
            uart_write_byte(joystic.y_abs);
            uart_write_byte('\n');
        }
        if (buttons_events & BTN_TOGGLELIGHTS_PRESSED_2) {
            uart_write_str("tp2\n");
        }
        if (buttons_events & BTN_TOGGLELIGHTS_RELEASED) {
            uart_write_str("tr\n");
        }

        joystic_update();
    }
}
