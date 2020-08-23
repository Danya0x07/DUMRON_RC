/**
 * Макросы для ножек МК.
 */

#ifndef _CONFIG_H
#define _CONFIG_H

/* Последовательный канал для отладки. */
#define UART_GPORT  GPIOD
#define UART_TX_GPIN    GPIO_PIN_5
#define UART_RX_GPIN    GPIO_PIN_6

/* Отладочный светодиод. */
#define LED_GPORT   GPIOE
#define LED_GPIN    GPIO_PIN_5

/* Пьезобуззер. */
#define BUZZER_GPORT    GPIOD
#define BUZZER_GPIN     GPIO_PIN_7

/* Кнопки. */
#define BTN_ARMUP_GPORT     GPIOC
#define BTN_ARMUP_GPIN      GPIO_PIN_2
#define BTN_ARMDOWN_GPORT   GPIOB
#define BTN_ARMDOWN_GPIN    GPIO_PIN_3
#define BTN_CLAWSQUEEZE_GPORT   GPIOB
#define BTN_CLAWSQUEEZE_GPIN    GPIO_PIN_4
#define BTN_CLAWRELEASE_GPORT   GPIOB
#define BTN_CLAWRELEASE_GPIN    GPIO_PIN_5
#define BTN_BUZZER_GPORT    GPIOC
#define BTN_BUZZER_GPIN     GPIO_PIN_1
#define BTN_TOGGLELIGHTS_GPORT  GPIOF
#define BTN_TOGGLELIGHTS_GPIN   GPIO_PIN_4

/* Радиомодуль. */
#define RADIO_GPORT     GPIOC
#define RADIO_CE_GPIN   GPIO_PIN_4
#define RADIO_CSN_GPIN  GPIO_PIN_3

/* Дисплей. */
#define LCD_GPORT    GPIOD
#define LCD_RST_GPIN GPIO_PIN_3
#define LCD_CE_GPIN  GPIO_PIN_2
#define LCD_DC_GPIN  GPIO_PIN_0
#define LCD_BL_GPIN  GPIO_PIN_4

/* Джойстик. */
#define JOYSTICK_X_ADC_CH   ADC1_CHANNEL_0
#define JOYSTICK_Y_ADC_CH   ADC1_CHANNEL_1

/* Делитель напряжения для измерения заряда батареек. */
#define BATTERY_ADC_CH  ADC1_CHANNEL_2

#endif /* _CONFIG_H */