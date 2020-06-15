/**
 * Здесь "бизнес-логика" радиосвязи.
 */

#include "radio.h"
#include "delay.h"
#include "debug.h"

#include <nrf24l01.h>

/* Пины модуля */
#define NRF_GPIO    GPIOC
#define NRF_PIN_CE  GPIO_PIN_4
#define NRF_PIN_CSN GPIO_PIN_3

void radio_init(void)
{
    uint8_t tgtaddress[4] = {0xC7, 0x68, 0xAC, 0x35};

    struct nrf24l01_tx_config config = {
        .address = tgtaddress,
        .addr_size = NRF24L01_ADDRS_4BYTE,
        .crc_mode = NRF24L01_CRC_1BYTE,
        .datarate = NRF24L01_DATARATE_1MBPS,
        .power = NRF24L01_POWER_0DBM,
        .retr_delay = NRF24L01_RETR_DELAY_1000US,
        .retr_count = NRF24L01_RETR_COUNT_3,
        .mode = NRF24L01_TX_MODE_ACK_PAYLOAD,
        .en_irq = 0,
        .rf_channel = 112
    };

    GPIO_Init(NRF_GPIO, NRF_PIN_CSN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(NRF_GPIO, NRF_PIN_CE, GPIO_MODE_OUT_PP_LOW_FAST);

    delay_ms(100);  /* Ожидание после подачи питания. */

    if (nrf24l01_tx_configure(&config) < 0) {
        led_blink(3, 300);
    }
}

void radio_send_data(DataToRobot* data_to_robot)
{
    if (nrf24l01_full_tx_fifo())  {
        nrf24l01_flush_tx_fifo();
    }
    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_MAX_RT) {
        nrf24l01_flush_tx_fifo();
        nrf24l01_clear_interrupts(NRF24L01_IRQ_MAX_RT);
    }
    nrf24l01_tx_write_pld(data_to_robot, sizeof(DataToRobot));
    nrf24l01_tx_transmit();
}

bool radio_check_for_incoming(DataFromRobot* data_from_robot)
{
    bool conn_ok = FALSE;
    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_RX_DR) {
        uint8_t pld_size;
        do {
            pld_size = nrf24l01_read_pld_size();
            if (pld_size != sizeof(DataFromRobot)) {
                nrf24l01_flush_rx_fifo();
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
                break;
            }
            nrf24l01_read_pld(data_from_robot, sizeof(DataFromRobot));
            nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
            conn_ok = TRUE;
        } while (nrf24l01_data_in_rx_fifo());
    }
    return conn_ok;
}

bool radio_is_time_to_update_io_data(void)
{
    bool is_time_to_update_io_data = TIM3_GetCounter() >= 122;
    if (is_time_to_update_io_data)
        TIM3_SetCounter(0);
    return is_time_to_update_io_data;
}

bool radio_data_to_robot_is_new(const DataToRobot* data_to_robot)
{
    static DataToRobot buffer_to_robot;

    bool data_is_new = (
        buffer_to_robot.ctrl.reg    != data_to_robot->ctrl.reg   ||
        buffer_to_robot.speed_left  != data_to_robot->speed_left  ||
        buffer_to_robot.speed_right != data_to_robot->speed_right
    );
    if (data_is_new) {
        buffer_to_robot.ctrl.reg    = data_to_robot->ctrl.reg;
        buffer_to_robot.speed_left  = data_to_robot->speed_left;
        buffer_to_robot.speed_right = data_to_robot->speed_right;
    }
    return data_is_new;
}
