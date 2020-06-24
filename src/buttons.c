#include <stm8s.h>

#include "buttons.h"
#include "delay.h"

typedef GPIO_TypeDef *gpio_port_t;
typedef uint8_t gpio_pin_t;

typedef enum {PULLDOWN, PULLUP} button_mode_e;

/*
 * Если кнопка btnx подтянута резистором к vcc,
 * но btnx.mode == PULLDOWN, то btn_pressed(&btnx) == TRUE
 * при отпускании этой кнопки.
 */
#define TRIGGER_ON_PRESS    PULLUP
#define TRIGGER_ON_RELEASE  PULLDOWN

typedef struct {
    gpio_port_t gport;
    gpio_pin_t gpin;
    button_mode_e mode;
    BitStatus last_status;
} button_s;

static button_s btn_armup = {
    .gport = GPIOC,
    .gpin = GPIO_PIN_2,
    .mode = PULLUP,
    .last_status = RESET
};

static button_s btn_armdown = {
    .gport = GPIOB,
    .gpin = GPIO_PIN_3,
    .mode = PULLUP,
    .last_status = RESET
};

static button_s btn_clawsqueeze = {
    .gport = GPIOB,
    .gpin = GPIO_PIN_4,
    .mode = PULLUP,
    .last_status = RESET
};

static button_s btn_clawrelease = {
    .gport = GPIOB,
    .gpin = GPIO_PIN_5,
    .mode = PULLUP,
    .last_status = RESET
};

static button_s btn_buzzer = {
    .gport = GPIOC,
    .gpin = GPIO_PIN_1,
    .mode = PULLUP,
    .last_status = RESET
};

static button_s btn_togglelights = {
    .gport = GPIOF,
    .gpin = GPIO_PIN_4,
    .mode = PULLUP,
    .last_status = RESET
};

static bool btn_pressed(button_s *btn);
static bool btn_pressed_again(button_s *btn);

void buttons_get_events(btn_events_s *ev)
{

    if (btn_pressed(&btn_armup)) {
        if (btn_armup.mode == TRIGGER_ON_PRESS) {
            ev->arm_up = BTN_EV_PRESSED;
            btn_armup.mode = TRIGGER_ON_RELEASE;
        } else {
            ev->arm_up = BTN_EV_RELEASED;
            btn_armup.mode = TRIGGER_ON_PRESS;
        }
    } else {
        ev->arm_up = BTN_EV_NONE;
    }

    if (btn_pressed(&btn_armdown)) {
        if (btn_armdown.mode == TRIGGER_ON_PRESS) {
            ev->arm_down = BTN_EV_PRESSED;
            btn_armdown.mode = TRIGGER_ON_RELEASE;
        } else {
            ev->arm_down = BTN_EV_RELEASED;
            btn_armdown.mode = TRIGGER_ON_PRESS;
        }
    } else {
        ev->arm_down = BTN_EV_NONE;
    }

    if (btn_pressed(&btn_clawsqueeze)) {
        if (btn_clawsqueeze.mode == TRIGGER_ON_PRESS) {
            ev->claw_squeeze = BTN_EV_PRESSED;
            btn_clawsqueeze.mode = TRIGGER_ON_RELEASE;
        } else {
            ev->claw_squeeze = BTN_EV_RELEASED;
            btn_clawsqueeze.mode = TRIGGER_ON_PRESS;
        }
    } else {
        ev->claw_squeeze = BTN_EV_NONE;
    }

    if (btn_pressed(&btn_clawrelease)) {
        if (btn_clawrelease.mode == TRIGGER_ON_PRESS) {
            ev->claw_release = BTN_EV_PRESSED;
            btn_clawrelease.mode = TRIGGER_ON_RELEASE;
        } else {
            ev->claw_release = BTN_EV_RELEASED;
            btn_clawrelease.mode = TRIGGER_ON_PRESS;
        }
    } else {
        ev->claw_release = BTN_EV_NONE;
    }

    if (btn_pressed(&btn_buzzer)) {
        if (btn_buzzer.mode == TRIGGER_ON_PRESS) {
            ev->buzzer = BTN_EV_PRESSED;
            btn_buzzer.mode = TRIGGER_ON_RELEASE;
        } else {
            ev->buzzer = BTN_EV_RELEASED;
            btn_buzzer.mode = TRIGGER_ON_PRESS;
        }
    } else {
        ev->buzzer = BTN_EV_NONE;
    }

    if (btn_pressed(&btn_togglelights)) {
        if (btn_pressed_again(&btn_togglelights)) {
            ev->toggle_lights = BTN_EV_DOUBLE_PRESSED;
        } else {
            ev->toggle_lights = BTN_EV_PRESSED;
        }
    } else {
        ev->toggle_lights = BTN_EV_NONE;
    }
}

static bool btn_pressed(button_s *btn)
{
    bool pressed = FALSE;
    BitStatus current_status = GPIO_ReadInputPin(btn->gport, btn->gpin);

    if (btn->last_status != current_status) {
        delay_ms(5);
        current_status = GPIO_ReadInputPin(btn->gport, btn->gpin);
    }

    if (!btn->last_status && current_status)
        pressed = !(bool)btn->mode;
    else if (btn->last_status && !current_status)
        pressed = (bool)btn->mode;

    btn->last_status = current_status;

    return pressed;
}

static bool btn_pressed_again(button_s *btn)
{
    uint8_t i;

    for (i = 0; i < 200; i++) {
        delay_ms(1);
        if (btn_pressed(btn)) {
            return TRUE;
        }
    }
    return FALSE;
}