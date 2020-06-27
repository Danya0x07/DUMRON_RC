#include "keypad.h"
#include "buttons.h"

static button_t armup_btn = {
    .gport = GPIOC,
    .gpin = GPIO_PIN_2,
    .mode = BUTTON_MODE_PULLUP,
    ._last_status = RESET
};

static button_t armdown_btn = {
    .gport = GPIOB,
    .gpin = GPIO_PIN_3,
    .mode = BUTTON_MODE_PULLUP,
    ._last_status = RESET
};

static button_t clawsqueeze_btn = {
    .gport = GPIOB,
    .gpin = GPIO_PIN_4,
    .mode = BUTTON_MODE_PULLUP,
    ._last_status = RESET
};

static button_t clawrelease_btn = {
    .gport = GPIOB,
    .gpin = GPIO_PIN_5,
    .mode = BUTTON_MODE_PULLUP,
    ._last_status = RESET
};

static button_t buzzer_btn = {
    .gport = GPIOC,
    .gpin = GPIO_PIN_1,
    .mode = BUTTON_MODE_PULLUP,
    ._last_status = RESET,
};

static button_t lights_btn = {
    .gport = GPIOF,
    .gpin = GPIO_PIN_4,
    .mode = BUTTON_MODE_PULLUP,
    ._last_status = RESET,
};

typedef enum {
    NONE = 0,
    PRESSED,
    DOUBLEPRESSED,
    RELEASED
} btn_event_t;

static struct {
    btn_event_t armup_ev;
    btn_event_t armdown_ev;
    btn_event_t clawsqueeze_ev;
    btn_event_t clawrelease_ev;
    bool buzzer_en;
    bool lights_en;
} keypad_status;

/*
 * Если кнопка btnx подтянута резистором к vcc,
 * но btnx.mode == BUTTON_MODE_PULLDOWN, то button_pressed(&btnx) == TRUE
 * при отпускании этой кнопки.
 */
#define TRIGGER_ON_PRESS    BUTTON_MODE_PULLUP
#define TRIGGER_ON_RELEASE  BUTTON_MODE_PULLDOWN

static btn_event_t get_btn_event(button_t *btn, bool support_dblclick)
{
    btn_event_t ev = NONE;

    if (button_pressed(btn)) {
        if (btn->mode == TRIGGER_ON_PRESS) {
            /*
             * Проверка повторного нажатия увеличивает время реакции на
             * одиночное нажатие.
             */
            if (support_dblclick && button_pressed_again(btn))
                ev = DOUBLEPRESSED;
            else
                ev = PRESSED;
            btn->mode = TRIGGER_ON_RELEASE;
        } else {  // btn->mode == TRIGGER_ON_RELEASE;
            ev = RELEASED;
            btn->mode = TRIGGER_ON_PRESS;
        }
    }

    return ev;
}

void keypad_update()
{
    keypad_status.armup_ev = get_btn_event(&armup_btn, FALSE);
    keypad_status.armdown_ev = get_btn_event(&armdown_btn, FALSE);
    keypad_status.clawsqueeze_ev = get_btn_event(&clawsqueeze_btn, FALSE);
    keypad_status.clawrelease_ev = get_btn_event(&clawrelease_btn, FALSE);
    keypad_status.buzzer_en = button_is_hold(&buzzer_btn);
    keypad_status.lights_en = button_pressed(&lights_btn);
}

void keypad_form_robot_state(data_to_robot_t *dtr)
{
    if (keypad_status.armup_ev == PRESSED) {
        dtr->ctrl.bf.arm_ctrl = ARMCTL_UP;
    }
    else if (keypad_status.armup_ev == RELEASED) {
        dtr->ctrl.bf.arm_ctrl = ARMCTL_STOP;
    }

    if (keypad_status.armdown_ev == PRESSED) {
        dtr->ctrl.bf.arm_ctrl = ARMCTL_DOWN;
    }
    else if (keypad_status.armdown_ev == RELEASED) {
        dtr->ctrl.bf.arm_ctrl = ARMCTL_STOP;
    }

    if (keypad_status.clawsqueeze_ev == PRESSED) {
        dtr->ctrl.bf.claw_ctrl = CLAWCTL_SQUEEZE;
    }
    else if (keypad_status.clawsqueeze_ev == RELEASED) {
        dtr->ctrl.bf.claw_ctrl = CLAWCTL_STOP;
    }

    if (keypad_status.clawrelease_ev == PRESSED) {
        dtr->ctrl.bf.claw_ctrl = CLAWCTL_RELEASE;
    }
    else if (keypad_status.clawrelease_ev == RELEASED) {
        dtr->ctrl.bf.claw_ctrl = CLAWCTL_STOP;
    }

    dtr->ctrl.bf.buzzer_en = keypad_status.buzzer_en;

    if (keypad_status.lights_en) {
        dtr->ctrl.bf.lights_en = !dtr->ctrl.bf.lights_en;
    }
}