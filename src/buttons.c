#include "buttons.h"
#include "delay.h"

uint32_t buttons_events = 0;

typedef enum {PULLDOWN, PULLUP = !PULLDOWN} ButtonMode;

/*
 * Если кнопка btnx подтянута резистором к vcc,
 * но btnx.mode == PULLDOWN, то btn_pressed(&btnx) == TRUE
 * при отпускании этой кнопки.
 */
#define ON_PRESS   PULLUP
#define ON_RELEASE PULLDOWN

static struct Button {
    GPIO_TypeDef* reg;
    uint8_t pin;
    ButtonMode mode;
    bool last_state;
} btn_armup,
  btn_armdown,
  btn_clawsqueeze,
  btn_clawrelease,
  btn_klaxon,
  btn_togglelights;

static bool btn_pressed(struct Button* btn);
static bool btn_pressed_again(struct Button* btn);
 

void buttons_init(void)
{
    btn_armup.reg = GPIOB;
    btn_armup.pin = GPIO_PIN_2;
    btn_armup.mode = PULLUP;
    btn_armup.last_state = FALSE;
    GPIOB->DDR &= ~GPIO_PIN_2;

    btn_armdown.reg = GPIOB;
    btn_armdown.pin = GPIO_PIN_3;
    btn_armdown.mode = PULLUP;
    btn_armdown.last_state = FALSE;
    GPIOB->DDR &= ~GPIO_PIN_3;

    btn_clawsqueeze.reg = GPIOB;
    btn_clawsqueeze.pin = GPIO_PIN_4;
    btn_clawsqueeze.mode = PULLUP;
    btn_clawsqueeze.last_state = FALSE;
    GPIOB->DDR &= ~GPIO_PIN_4;

    btn_clawrelease.reg = GPIOB;
    btn_clawrelease.pin = GPIO_PIN_5;
    btn_clawrelease.mode = PULLUP;
    btn_clawrelease.last_state = FALSE;
    GPIOB->DDR &= ~GPIO_PIN_5;

    btn_klaxon.reg = GPIOC;
    btn_klaxon.pin = GPIO_PIN_1;
    btn_klaxon.mode = PULLUP;
    btn_klaxon.last_state = FALSE;
    GPIOC->DDR &= ~GPIO_PIN_1;

    btn_togglelights.reg = GPIOF;
    btn_togglelights.pin = GPIO_PIN_4;
    btn_togglelights.mode = PULLUP;
    btn_togglelights.last_state = FALSE;
    GPIOF->DDR &= ~GPIO_PIN_4;
}

void buttons_update(void)
{
    buttons_events = 0;
    
    if (btn_pressed(&btn_armup)) {
        if (btn_pressed_again(&btn_armup)) {
            buttons_events |= BTN_ARMUP_PRESSED_2;
        } else if (btn_armup.mode == ON_PRESS) {
            buttons_events |= BTN_ARMUP_PRESSED;
            btn_armup.mode = ON_RELEASE;
        } else {
            buttons_events |= BTN_ARMUP_RELEASED;
            btn_armup.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_armdown)) {
        if (btn_pressed_again(&btn_armdown)) {
            buttons_events |= BTN_ARMDOWN_PRESSED_2;
        } else if (btn_armdown.mode == ON_PRESS) {
            buttons_events |= BTN_ARMDOWN_PRESSED;
            btn_armdown.mode = ON_RELEASE;
        } else {
            buttons_events |= BTN_ARMDOWN_RELEASED;
            btn_armdown.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_clawsqueeze)) {
        if (btn_pressed_again(&btn_clawsqueeze)) {
            buttons_events |= BTN_CLAWSQUEEZE_PRESSED_2;
        } else if (btn_clawsqueeze.mode == ON_PRESS) {
            buttons_events |= BTN_CLAWSQUEEZE_PRESSED;
            btn_clawsqueeze.mode = ON_RELEASE;
        } else {
            buttons_events |= BTN_CLAWSQUEEZE_RELEASED;
            btn_clawsqueeze.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_clawrelease)) {
        if (btn_pressed_again(&btn_clawrelease)) {
            buttons_events |= BTN_CLAWRELEASE_PRESSED_2;
        } else if (btn_clawrelease.mode == ON_PRESS) {
            buttons_events |= BTN_CLAWRELEASE_PRESSED;
            btn_clawrelease.mode = ON_RELEASE;
        } else {
            buttons_events |= BTN_CLAWRELEASE_RELEASED;
            btn_clawrelease.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_klaxon)) {
        if (btn_pressed_again(&btn_klaxon)) {
            buttons_events |= BTN_KLAXON_PRESSED_2;
        } else if (btn_klaxon.mode == ON_PRESS) {
            buttons_events |= BTN_KLAXON_PRESSED;
            btn_klaxon.mode = ON_RELEASE;
        } else {
            buttons_events |= BTN_KLAXON_RELEASED;
            btn_klaxon.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_togglelights)) {
        if (btn_togglelights.mode == ON_PRESS) {
            buttons_events |= BTN_TOGGLELIGHTS_PRESSED;
            btn_togglelights.mode = ON_RELEASE;
        } else {
            buttons_events |= BTN_TOGGLELIGHTS_RELEASED;
            btn_togglelights.mode = ON_PRESS;
        }
    }
}

static bool btn_pressed(struct Button* btn)
{
    bool pressed = FALSE;
    bool current_state = btn->reg->IDR & btn->pin;
    if (btn->last_state != current_state) {
        delay_ms(5);
        current_state = btn->reg->IDR & btn->pin;
    }

    if (!btn->last_state && current_state) pressed = !(bool)btn->mode;
    else if (btn->last_state && !current_state) pressed = (bool)btn->mode;
    btn->last_state = current_state;
    return pressed;
}

static bool btn_pressed_again(struct Button* btn)
{
    register uint8_t i;
    for (i = 0; i < 125; i++) {
        delay_ms(1);
        if (btn_pressed(btn)) {
            return TRUE;
        }
    }
    return FALSE;
}
