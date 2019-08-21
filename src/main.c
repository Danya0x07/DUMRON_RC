#include <stm8s.h>
#include "delay.h"
#include "buttons.h"
#include "debug.h"
#include "joystick.h"
#include "msgprotocol.h"
#include "display.h"

JoystickData joystick_data;
DataToRobot data_to_robot;

void setup(void);

int main(void)
{
    setup();
    delay_init();
    enableInterrupts();
    debug_init();
    display_init();
    buttons_init();
    joystick_init();
    display_test("Hello Nokia.");
    
    while (1) {
        buttons_update();
        /* button arm up */
        if (buttons_events & BTN_ARMUP_PRESSED) {
            data_to_robot.periph_state |= ARM_UP;
            logs("aup\n");
        } else if (buttons_events & BTN_ARMUP_RELEASED) {
            data_to_robot.periph_state &= ~ARM_UP;
            logs("aur\n");
        }
        /* button arm down */
        if (buttons_events & BTN_ARMDOWN_PRESSED) {
            data_to_robot.periph_state |= ARM_DOWN;
            logs("adp\n");
        } else if (buttons_events & BTN_ARMDOWN_RELEASED) {
            data_to_robot.periph_state &= ~ARM_DOWN;
            logs("adr\n");
        }
        /* button claw squeeze */
        if (buttons_events & BTN_CLAWSQUEEZE_PRESSED) {
            data_to_robot.periph_state |= CLAW_SQUEEZE;
            logs("csp\n");
        } else if (buttons_events & BTN_CLAWSQUEEZE_RELEASED) {
            data_to_robot.periph_state &= ~CLAW_SQUEEZE;
            logs("csr\n");
        }
        /* button claw release */
        if (buttons_events & BTN_CLAWRELEASE_PRESSED) {
            data_to_robot.periph_state |= CLAW_RELEASE;
            /*logs("crp\n");*/
            logi(joystick_data.direction);
            logi(joystick_data.x_abs);
            logi(joystick_data.y_abs);
            /*logi('\n');*/
        }/* else if (buttons_events & BTN_CLAWRELEASE_RELEASED) {
            data_to_robot.periph_state &= ~CLAW_RELEASE;
            logs("crr\n");
        }*/
        /* button klaxon */
        if (buttons_events & BTN_KLAXON_PRESSED) {
            data_to_robot.periph_state |= KLAXON_EN;
            logs("kp\n");
        } else if (buttons_events & BTN_KLAXON_RELEASED) {
            data_to_robot.periph_state &= ~KLAXON_EN;
            logs("kr\n");
        }
        /* button togglelights */
        if (buttons_events & BTN_TOGGLELIGHTS_PRESSED) {
            data_to_robot.periph_state ^= LIGHTS_EN;
            logs("tp\n");
        } else if (buttons_events & BTN_TOGGLELIGHTS_PRESSED_2) {
            logs("tp2\n");
        }

        joystick_update(&joystick_data);
        
        robot_calc_movement(&data_to_robot, &joystick_data);
        if (robot_data_is_new(&data_to_robot)) {
            /* TODO: Отправка данных data_to_robot роботу. */
            robot_data_update_buffer(&data_to_robot);
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
    GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(GPIOD, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_SLOW);

    ADC1_DeInit();

    TIM4_DeInit();

    SPI_DeInit();
    GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);  /* MOSI pin */
    GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT);  /* MISO pin */
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);  /* SCK pin */
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_16, SPI_MODE_MASTER,
             SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE,
             SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 7);
    SPI_Cmd(ENABLE);
}
