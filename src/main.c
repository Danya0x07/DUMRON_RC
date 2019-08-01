#include <stm8s.h>
#include "buttons.h"

int main(void)
{
    CLK_DeInit();
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV2);
    
    buttons_init();

    while (1) {
        buttons_checkevents();
        if (buttons_events & BTN_ARMUP_PRESSED) {

        }
        if (buttons_events & BTN_ARMUP_PRESSED_2) {

        }
        if (buttons_events & BTN_ARMUP_RELEASED) {

        }
        
        if (buttons_events & BTN_ARMDOWN_PRESSED) {

        }
        if (buttons_events & BTN_ARMDOWN_PRESSED_2) {

        }
        if (buttons_events & BTN_ARMDOWN_RELEASED) {

        }
        
        if (buttons_events & BTN_CLAWSQUEEZE_PRESSED) {

        }
        if (buttons_events & BTN_CLAWRELEASE_PRESSED_2) {

        }
        if (buttons_events & BTN_CLAWSQUEEZE_RELEASED) {

        }

        if (buttons_events & BTN_CLAWRELEASE_PRESSED) {

        }
        if (buttons_events & BTN_CLAWRELEASE_PRESSED_2) {

        }
        if (buttons_events & BTN_CLAWRELEASE_RELEASED) {

        }

        if (buttons_events & BTN_KLAXON_PRESSED) {

        }
        if (buttons_events & BTN_KLAXON_PRESSED_2) {

        }
        if (buttons_events & BTN_KLAXON_RELEASED) {

        }

        if (buttons_events & BTN_TOGGLELIGHTS_PRESSED) {

        }
        if (buttons_events & BTN_TOGGLELIGHTS_PRESSED_2) {

        }
        if (buttons_events & BTN_TOGGLELIGHTS_RELEASED) {

        }
        
    }
}
