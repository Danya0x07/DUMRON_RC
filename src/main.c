#include <stm8s.h>

#include "config.h"
#include "halutils.h"
#include "debug.h"
#include "protocol.h"
#include "keypad.h"
#include "joystick.h"
#include "display.h"
#include "radio.h"
#include "battery.h"
#include "emitters.h"

static inline void system_setup(void);

int main(void)
{
    static data_to_robot_t out_data;
    static data_from_robot_t in_data;

    system_setup();

    display_init();
    radio_init();

    display_splash_screen();

    for (;;) {
        keypad_update();
        joystick_update();

        keypad_form_robot_state(&out_data);
        joystick_form_robot_movement(&out_data);

        if (radio_is_time_to_communicate() || radio_out_data_is_new(&out_data)) {
            static bool was_beep = FALSE;
            bool ack_received;
            uint8_t battery_charge = battery_get_charge();

            radio_send(&out_data);
            ack_received = radio_check_ack(&in_data);

            if (in_data.status.bf.back_distance == DIST_CLIFF) {
                if (!was_beep) {
                    buzzer_beep(1, 80);
                    was_beep = TRUE;
                }
            } else {
                was_beep = FALSE;
            }

            display_update(&out_data, &in_data, ack_received, battery_charge);
        }
    }
}

static inline void system_setup(void)
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

    // Пины SPI (MOSI & SCK).
    GPIO_Init(GPIOC, GPIO_PIN_6 | GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);

    // Пины излучателей (buzzer & lights).
    GPIO_Init(LED_GPORT, LED_GPIN, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(BUZZER_GPORT, BUZZER_GPIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    // Пины дисплея.
    GPIO_Init(LCD_GPORT, LCD_RST_GPIN | LCD_CE_GPIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(LCD_GPORT, LCD_DC_GPIN | LCD_BL_GPIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_WriteHigh(LCD_GPORT, LCD_BL_GPIN);

    // Пины кнопок.
    GPIO_Init(BTN_ARMUP_GPORT, BTN_ARMUP_GPIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(BTN_ARMDOWN_GPORT, BTN_ARMDOWN_GPIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(BTN_CLAWSQUEEZE_GPORT, BTN_CLAWSQUEEZE_GPIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(BTN_CLAWRELEASE_GPORT, BTN_CLAWRELEASE_GPIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(BTN_BUZZER_GPORT, BTN_BUZZER_GPIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(BTN_TOGGLELIGHTS_GPORT, BTN_TOGGLELIGHTS_GPIN, GPIO_MODE_IN_FL_NO_IT);

    // Пины радиомодуля.
    GPIO_Init(RADIO_GPORT, RADIO_CSN_GPIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(RADIO_GPORT, RADIO_CE_GPIN, GPIO_MODE_OUT_PP_LOW_FAST);

    // Не используемые пока пины.
    GPIO_Init(GPIOA, GPIO_PIN_1 | GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);

#ifdef DEBUG  // UART для отладки.
    UART2_DeInit();
    UART2_Init(9600,
               UART2_WORDLENGTH_8D,
               UART2_STOPBITS_1,
               UART2_PARITY_NO,
               UART2_SYNCMODE_CLOCK_DISABLE,
               UART2_MODE_TXRX_ENABLE);
#endif

    // Таймер для регулярного измерения батарейки.
    TIM2->PSCR = TIM2_PRESCALER_32768;
    TIM2->ARRH = 0xFF;
    TIM2->ARRL = 0xFF;
    TIM2->CR1 = TIM2_CR1_CEN;

    // Таймер для регулярной посылки радиосообщения.
    TIM3->PSCR = TIM3_PRESCALER_32768;
    TIM3->ARRH = 0xFF;
    TIM3->ARRL = 0xFF;
    TIM3->CR1 = TIM3_CR1_CEN;

    // Таймер для delay_ms.
    TIM4->PSCR = TIM4_PRESCALER_64;
    TIM4->ARR = 0xFF;
    TIM4->CR1 |= TIM4_CR1_CEN;

    // АЦП для измерения джойстика и батарейки.
    ADC1->CR1 = ADC1_PRESSEL_FCPU_D10;
    ADC1->CR2 = ADC1_ALIGN_RIGHT;
    ADC1->TDRL = 0x07;
    ADC1->CR1 |= ADC1_CR1_ADON;  // включение АЦП
    adc_start_conversion(JOYSTICK_X_ADC_CH);

    // SPI для общения с радиомодулем и дисплеем.
    SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_16, SPI_MODE_MASTER,
             SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE,
             SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 7);
    SPI_Cmd(ENABLE);

    // Разрешаем прерывания.
    enableInterrupts();
}