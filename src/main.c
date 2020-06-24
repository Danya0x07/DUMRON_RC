#include <stm8s.h>

#include "config.h"
#include "delay.h"
#include "debug.h"
#include "protocol.h"
#include "buttons.h"
#include "joystick.h"
#include "display.h"
#include "radio.h"
#include "battery.h"
#include "emitters.h"

static void system_setup(void);

int main(void)
{
    static data_to_robot_s out_data;
    static data_from_robot_s in_data;
    static btn_events_s btn_events;

    system_setup();

    display_init();
    radio_init();

    for (;;) {
        buttons_get_events(&btn_events);
        joystick_update();

        // button arm up
        if (btn_events.arm_up == BTN_EV_PRESSED) {
            out_data.ctrl.bf.arm_ctrl = ARM_UP;
        }
        else if (btn_events.arm_up == BTN_EV_RELEASED) {
            out_data.ctrl.bf.arm_ctrl = ARM_STOP;
        }

        // button arm down
        if (btn_events.arm_down == BTN_EV_PRESSED) {
            out_data.ctrl.bf.arm_ctrl = ARM_DOWN;
        }
        else if (btn_events.arm_down == BTN_EV_RELEASED) {
            out_data.ctrl.bf.arm_ctrl = ARM_STOP;
        }

        // button claw squeeze
        if (btn_events.claw_squeeze == BTN_EV_PRESSED) {
            out_data.ctrl.bf.claw_ctrl = CLAW_SQUEESE;
        }
        else if (btn_events.claw_squeeze == BTN_EV_RELEASED) {
            out_data.ctrl.bf.claw_ctrl = CLAW_STOP;
        }

        // button claw release
        if (btn_events.claw_release == BTN_EV_PRESSED) {
            out_data.ctrl.bf.claw_ctrl = CLAW_RELEASE;
        }
        else if (btn_events.claw_release == BTN_EV_RELEASED) {
            out_data.ctrl.bf.claw_ctrl = CLAW_STOP;
        }

        // button buzzer
        out_data.ctrl.bf.buzzer_en = btn_events.buzzer == BTN_EV_PRESSED;

        // button togglelights
        if (btn_events.toggle_lights == BTN_EV_PRESSED) {
            out_data.ctrl.bf.lights_en = !out_data.ctrl.bf.lights_en;
        }

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

            display_update(&out_data, &in_data, ack_received,
                           battery_charge);
        }
    }
}

static void system_setup(void)
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

    // UART для отладки.
    UART2_DeInit();
    UART2_Init(9600,
               UART2_WORDLENGTH_8D,
               UART2_STOPBITS_1,
               UART2_PARITY_NO,
               UART2_SYNCMODE_CLOCK_DISABLE,
               UART2_MODE_TXRX_ENABLE);

    // Таймер для регулярного измерения батарейки.
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_32768, 60000);
    TIM2_Cmd(ENABLE);

    // Таймер для регулярной посылки радиосообщения.
    TIM3_DeInit();
    TIM3_TimeBaseInit(TIM3_PRESCALER_32768, 1000);
    TIM3_Cmd(ENABLE);

    // Таймер для delay_ms.
    TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_PRESCALER_64, 124);
    TIM4_Cmd(ENABLE);

    // АЦП для измерения джойстика и батарейки.
    ADC1_DeInit();
    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
              ADC1_CHANNEL_0, ADC1_PRESSEL_FCPU_D10,
              ADC1_EXTTRIG_TIM, DISABLE,
              ADC1_ALIGN_RIGHT,
              ADC1_SCHMITTTRIG_CHANNEL0, DISABLE);
    ADC1_StartConversion();

    // SPI для общения с радиомодулем и дисплеем.
    SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_16, SPI_MODE_MASTER,
             SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE,
             SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 7);
    SPI_Cmd(ENABLE);

    // Разрешаем прерывания.
    enableInterrupts();
}
