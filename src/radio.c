#include "radio.h"
#include "delay.h"
#include "emitters.h"

#include <nrf24l01.h>

void radio_init(void)
{
    uint8_t address[4] = {0xC7, 0x68, 0xAC, 0x35};

    struct nrf24l01_tx_config config = {
        .address = address,
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

    delay_ms(NRF24L01_PWR_ON_DELAY_MS);

    if (nrf24l01_tx_configure(&config) < 0) {
        led_blink(3, 300);
    }
}

void radio_send(data_to_robot_s *outcoming)
{
    if (nrf24l01_full_tx_fifo())  {
        nrf24l01_flush_tx_fifo();
    }
    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_MAX_RT) {
        nrf24l01_flush_tx_fifo();
        nrf24l01_clear_interrupts(NRF24L01_IRQ_MAX_RT);
    }
    nrf24l01_tx_write_pld(outcoming, sizeof(data_to_robot_s));
    nrf24l01_tx_transmit();
}

bool radio_check_ack(data_from_robot_s *incoming)
{
    bool conn_ok = FALSE;

    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_RX_DR) {
        do {
            if (nrf24l01_read_pld_size() != sizeof(data_from_robot_s)) {
                nrf24l01_flush_rx_fifo();
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
                break;
            }
            nrf24l01_read_pld(incoming, sizeof(data_from_robot_s));
            nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
            conn_ok = TRUE;
        } while (nrf24l01_data_in_rx_fifo());
    }
    return conn_ok;
}

bool radio_is_time_to_communicate(void)
{
    bool time_to_communicate = TIM3_GetCounter() >= 122;

    if (time_to_communicate)
        TIM3_SetCounter(0);

    return time_to_communicate;
}

bool radio_out_data_is_new(const data_to_robot_s *outcoming)
{
    static data_to_robot_s cache;

    bool data_is_new = (
        cache.ctrl.reg    != outcoming->ctrl.reg   ||
        cache.speed_left  != outcoming->speed_left ||
        cache.speed_right != outcoming->speed_right
    );
    if (data_is_new) {
        cache.ctrl.reg    = outcoming->ctrl.reg;
        cache.speed_left  = outcoming->speed_left;
        cache.speed_right = outcoming->speed_right;
    }

    return data_is_new;
}
