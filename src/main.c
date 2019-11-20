#include <stm8s.h>
#include "delay.h"
#include "debug.h"
#include "buttons.h"
#include "robot_interface.h"
#include "joystick.h"
#include "display.h"
#include "radio.h"
#include "battery.h"


#define MAX_SAFE_TO_FALL_DISTANCE   12

static uint16_t buttons_events;
static JoystickData joystick_data;
static DataToRobot data_to_robot;
static DataFromRobot data_from_robot;

void setup(void);

int main(void)
{
    setup();

    debug_init();
    display_init();
    buttons_init();
    radio_init();
    joystick_start();

    while (1) {
        buttons_events = buttons_get_events();
        joystick_update(&joystick_data);
        /* button arm up */
        if (buttons_events & BTN_ARMUP_PRESSED) {
            data_to_robot.control_reg |= ROBOT_CFLAG_ARM_UP;
            led_on();
        } else if (buttons_events & BTN_ARMUP_RELEASED) {
            data_to_robot.control_reg &= ~ROBOT_CFLAG_ARM_UP;
            led_off();
        }
        /* button arm down */
        if (buttons_events & BTN_ARMDOWN_PRESSED) {
            data_to_robot.control_reg |= ROBOT_CFLAG_ARM_DOWN;
            led_on();
        } else if (buttons_events & BTN_ARMDOWN_RELEASED) {
            data_to_robot.control_reg &= ~ROBOT_CFLAG_ARM_DOWN;
            led_off();
        }
        /* button claw squeeze */
        if (buttons_events & BTN_CLAWSQUEEZE_PRESSED) {
            data_to_robot.control_reg |= ROBOT_CFLAG_CLAW_SQUEEZE;
            led_on();
        } else if (buttons_events & BTN_CLAWSQUEEZE_RELEASED) {
            data_to_robot.control_reg &= ~ROBOT_CFLAG_CLAW_SQUEEZE;
            led_off();
        }
        /* button claw release */
        if (buttons_events & BTN_CLAWRELEASE_PRESSED) {
            data_to_robot.control_reg |= ROBOT_CFLAG_CLAW_RELEASE;
            led_on();
        } else if (buttons_events & BTN_CLAWRELEASE_RELEASED) {
            data_to_robot.control_reg &= ~ROBOT_CFLAG_CLAW_RELEASE;
            led_off();
        }
        /* button klaxon */
        if (buttons_events & BTN_KLAXON_PRESSED) {
            data_to_robot.control_reg |= ROBOT_CFLAG_KLAXON_EN;
            led_on();
        } else if (buttons_events & BTN_KLAXON_RELEASED) {
            data_to_robot.control_reg &= ~ROBOT_CFLAG_KLAXON_EN;
            led_off();
        }
        /* button togglelights */
        if (buttons_events & BTN_TOGGLELIGHTS_PRESSED) {
            data_to_robot.control_reg ^= ROBOT_CFLAG_LIGHTS_EN;
            led_toggle();
        } else if (buttons_events & BTN_TOGGLELIGHTS_PRESSED_2) {
            led_toggle();
        }

        joystick_data_to_robot_movement(&joystick_data, &data_to_robot);

        if (radio_is_time_to_update_io_data() || radio_data_to_robot_is_new(&data_to_robot)) {
            static bool was_beep = FALSE;
            bool ack_received;
            uint8_t battery_voltage = battery_get_voltage();

            radio_send_data(&data_to_robot);
            ack_received = radio_check_for_incoming(&data_from_robot);

            if (data_from_robot.back_distance > MAX_SAFE_TO_FALL_DISTANCE) {
                if (!was_beep) {
                    buzzer_peep(1, 80);
                    was_beep = TRUE;
                }
            } else {
                was_beep = FALSE;
            }

            display_update(&data_to_robot, &data_from_robot, ack_received, battery_voltage);
        }
    }
}

void setup(void)
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
    /* Неиспользуемые пока пины */
    GPIO_Init(GPIOA, GPIO_PIN_1 | GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);
    /* Пины SPI (MOSI & SCK) */
    GPIO_Init(GPIOC, GPIO_PIN_6 | GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
    /* UART для отладки */
    UART2_DeInit();
    UART2_Init(9600,
               UART2_WORDLENGTH_8D,
               UART2_STOPBITS_1,
               UART2_PARITY_NO,
               UART2_SYNCMODE_CLOCK_DISABLE,
               UART2_MODE_TXRX_ENABLE);
    /* Таймер для регулярного измерения батарейки */
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_32768, 60000);
    TIM2_Cmd(ENABLE);
    /* Таймер для регулярной посылки радиосообщения */
    TIM3_DeInit();
    TIM3_TimeBaseInit(TIM3_PRESCALER_32768, 1000);
    TIM3_Cmd(ENABLE);
    /* Таймер для delay ms*/
    TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_PRESCALER_64, 124);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);
    /* АЦП для измерения джойстика и батарейки */
    ADC1_DeInit();
    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
              ADC1_CHANNEL_0, ADC1_PRESSEL_FCPU_D10,
              ADC1_EXTTRIG_TIM, DISABLE,
              ADC1_ALIGN_RIGHT,
              ADC1_SCHMITTTRIG_CHANNEL0, DISABLE);
    /* SPI для общения с радиомодулем и дисплеем. */
    SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_16, SPI_MODE_MASTER,
             SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE,
             SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 7);
    SPI_Cmd(ENABLE);
    /* Разрешаем прерывания */
    enableInterrupts();
}
