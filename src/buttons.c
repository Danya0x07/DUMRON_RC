#include "buttons.h"
#include "delay.h"

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
    BitStatus last_status;
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
    btn_armup.last_status = RESET;
    GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);

    btn_armdown.reg = GPIOB;
    btn_armdown.pin = GPIO_PIN_3;
    btn_armdown.mode = PULLUP;
    btn_armdown.last_status = RESET;
    GPIO_Init(GPIOB, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);

    btn_clawsqueeze.reg = GPIOB;
    btn_clawsqueeze.pin = GPIO_PIN_4;
    btn_clawsqueeze.mode = PULLUP;
    btn_clawsqueeze.last_status = RESET;
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);

    btn_clawrelease.reg = GPIOB;
    btn_clawrelease.pin = GPIO_PIN_5;
    btn_clawrelease.mode = PULLUP;
    btn_clawrelease.last_status = RESET;
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);

    btn_klaxon.reg = GPIOC;
    btn_klaxon.pin = GPIO_PIN_1;
    btn_klaxon.mode = PULLUP;
    btn_klaxon.last_status = RESET;
    GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);

    btn_togglelights.reg = GPIOF;
    btn_togglelights.pin = GPIO_PIN_4;
    btn_togglelights.mode = PULLUP;
    btn_togglelights.last_status = RESET;
    GPIO_Init(GPIOF, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
}

void buttons_update(uint16_t* event_register)
{
    *event_register = 0;
    
    if (btn_pressed(&btn_armup)) {
        if (btn_armup.mode == ON_PRESS) {
            *event_register |= BTN_ARMUP_PRESSED;
            btn_armup.mode = ON_RELEASE;
        } else {
            *event_register |= BTN_ARMUP_RELEASED;
            btn_armup.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_armdown)) {
        if (btn_armdown.mode == ON_PRESS) {
            *event_register |= BTN_ARMDOWN_PRESSED;
            btn_armdown.mode = ON_RELEASE;
        } else {
            *event_register |= BTN_ARMDOWN_RELEASED;
            btn_armdown.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_clawsqueeze)) {
        if (btn_clawsqueeze.mode == ON_PRESS) {
            *event_register |= BTN_CLAWSQUEEZE_PRESSED;
            btn_clawsqueeze.mode = ON_RELEASE;
        } else {
            *event_register |= BTN_CLAWSQUEEZE_RELEASED;
            btn_clawsqueeze.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_clawrelease)) {
        if (btn_clawrelease.mode == ON_PRESS) {
            *event_register |= BTN_CLAWRELEASE_PRESSED;
            btn_clawrelease.mode = ON_RELEASE;
        } else {
            *event_register |= BTN_CLAWRELEASE_RELEASED;
            btn_clawrelease.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_klaxon)) {
        if (btn_klaxon.mode == ON_PRESS) {
            *event_register |= BTN_KLAXON_PRESSED;
            btn_klaxon.mode = ON_RELEASE;
        } else {
            *event_register |= BTN_KLAXON_RELEASED;
            btn_klaxon.mode = ON_PRESS;
        }
    }

    if (btn_pressed(&btn_togglelights)) {
        if (btn_pressed_again(&btn_togglelights)) {
            *event_register |= BTN_TOGGLELIGHTS_PRESSED_2;
        } else {
            *event_register |= BTN_TOGGLELIGHTS_PRESSED;
        }
    }
}

static bool btn_pressed(struct Button* btn)
{
    bool pressed = FALSE;
    BitStatus current_status = GPIO_ReadInputPin(btn->reg, btn->pin);
    if (btn->last_status != current_status) {
        delay_ms(5);
        current_status = GPIO_ReadInputPin(btn->reg, btn->pin);
    }

    if (!btn->last_status && current_status) pressed = !(bool)btn->mode;
    else if (btn->last_status && !current_status) pressed = (bool)btn->mode;
    btn->last_status = current_status;
    return pressed;
}

static bool btn_pressed_again(struct Button* btn)
{
    register uint8_t i;
    for (i = 0; i < 200; i++) {
        delay_ms(1);
        if (btn_pressed(btn)) {
            return TRUE;
        }
    }
    return FALSE;
}
