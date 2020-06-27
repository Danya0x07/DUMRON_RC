#ifndef _BUTTONS_H
#define _BUTTONS_H

#include <stm8s.h>

/** Подтяжка кнопки. */
typedef enum {
    BUTTON_MODE_PULLDOWN = 0,
    BUTTON_MODE_PULLUP
} button_mode_t;

typedef struct button {
    GPIO_TypeDef *gport;
    uint8_t gpin;
    button_mode_t mode;
    BitStatus _last_status;  // для обработки дребезга
} button_t;

/** Возвращает TRUE, если кнопка *btn зажата в данный момент, иначе FALSE. */
bool button_is_hold(const button_t *btn);

/**
 * Возвращает TRUE, если кнопка *btn была переведена из ненажатого состояния в
 * нажатое.
 * После первого срабатывания, вернувшего TRUE, возвращает FALSE до тех пор,
 * пока кнопка не будет отпущена и нажата снова.
 * Интересный факт: если перепутать атрибут mode кнопки, то TRUE будет
 * возвращаться при отпускании, а не при нажатии.
 */
bool button_pressed(button_t *btn);

/**
 * Возвращает TRUE, если в течение небольшого промежутка времени
 * button_pressed() вернула TRUE, иначе FALSE.
 * Предназначена для обнаружения многократных нажатий.
 */
bool button_pressed_again(button_t *btn);

#endif  /* _BUTTONS_H */