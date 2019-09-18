#include <stm8s.h>
#include "delay.h"
#include "debug.h"
#include "buttons.h"
#include "robot_interface.h"
#include "joystick.h"
#include "display.h"
#include "radio.h"
#include "battery.h"

void setup(void);

int main(void)
{
    static uint16_t buttons_events;
    static JoystickData joystick_data;
    static DataToRobot data_to_robot;
    static DataFromRobot data_from_robot;
    
    setup();
    delay_init();
    enableInterrupts();
    debug_init();
    display_init();
    buttons_init();
    joystick_init();
    battery_init();
    radio_init();
    
    while (1) {
        buttons_update(&buttons_events);
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
            uint8_t battery_voltage = battery_get_voltage();
            bool connection_error = radio_send_data(&data_to_robot);
            radio_check_for_incoming(&data_from_robot);
            display_update(&data_to_robot, &data_from_robot, connection_error, battery_voltage);
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

    UART2_DeInit();

    TIM2_DeInit();
    TIM3_DeInit();
    TIM4_DeInit();

    ADC1_DeInit();
    /*ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
              ADC1_CHANNEL_0, ADC1_PRESSEL_FCPU_D10,
              ADC1_EXTTRIG_GPIO, DISABLE,
              ADC1_ALIGN_RIGHT,
              ADC1_SCHMITTTRIG_CHANNEL0, DISABLE); */
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_0, ADC1_ALIGN_RIGHT);
    ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D10);
    ADC1_Cmd(ENABLE);

    SPI_DeInit();
    GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);  /* MOSI pin */
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);  /* SCK pin */
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_16, SPI_MODE_MASTER,
             SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE,
             SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 7);
    SPI_Cmd(ENABLE);
}
