#include "buttons.h"
#include "halutils.h"

bool button_is_hold(const button_t *btn)
{
    return !!GPIO_ReadInputPin(btn->gport, btn->gpin) != btn->mode;
}

bool button_pressed(button_t *btn)
{
    bool pressed = FALSE;
    BitStatus current_status = !!GPIO_ReadInputPin(btn->gport, btn->gpin);

    if (btn->_last_status != current_status) {
        delay_ms(5);
        current_status = !!GPIO_ReadInputPin(btn->gport, btn->gpin);
    }

    if (!btn->_last_status && current_status)
        pressed = !(bool)btn->mode;
    else if (btn->_last_status && !current_status)
        pressed = (bool)btn->mode;

    btn->_last_status = current_status;

    return pressed;
}

bool button_pressed_again(button_t *btn)
{
    uint8_t i;

    for (i = 0; i < 200; i++) {
        delay_ms(1);
        if (button_pressed(btn)) {
            return TRUE;
        }
    }
    return FALSE;
}