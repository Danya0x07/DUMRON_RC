#include "radio.h"
#include "halutils.h"
#include "display.h"
#include "emitters.h"

#include <nrf24l01.h>

static uint8_t radio_find_free_channel(void);

void radio_init(void)
{
    uint8_t address[3] = {0xC7, 0x68, 0xAC};

    struct nrf24l01_tx_config config = {
        .address = address,
        .addr_size = NRF24L01_ADDRS_3BYTE,
        .crc_mode = NRF24L01_CRC_1BYTE,
        .datarate = NRF24L01_DATARATE_1MBPS,
        .power = NRF24L01_POWER_0DBM,
        .retr_delay = NRF24L01_RETR_DELAY_1000US,
        .retr_count = NRF24L01_RETR_COUNT_3,
        .mode = NRF24L01_TX_MODE_ACK_PAYLOAD,
        .en_irq = 0,
        .rf_channel = RADIO_INITIAL_CHANNEL
    };

    delay_ms(NRF24L01_PWR_ON_DELAY_MS);

    if (nrf24l01_tx_configure(&config) < 0) {
        display_transceiver_missing();
        led_blink(3, 300);
    }
}

void radio_send(data_to_robot_t *outcoming)
{
    if (nrf24l01_full_tx_fifo())  {
        nrf24l01_flush_tx_fifo();
    }
    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_MAX_RT) {
        nrf24l01_flush_tx_fifo();
        nrf24l01_clear_interrupts(NRF24L01_IRQ_MAX_RT);
    }
    nrf24l01_tx_write_pld(outcoming, sizeof(data_to_robot_t));
    nrf24l01_tx_transmit();
}

bool radio_check_ack(data_from_robot_t *incoming)
{
    bool conn_ok = FALSE;

    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_RX_DR) {
        do {
            if (nrf24l01_read_pld_size() != sizeof(data_from_robot_t)) {
                nrf24l01_flush_rx_fifo();
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
                break;
            }
            nrf24l01_read_pld(incoming, sizeof(data_from_robot_t));
            nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
            conn_ok = TRUE;
        } while (nrf24l01_data_in_rx_fifo());
    }
    return conn_ok;
}

bool radio_is_time_to_communicate(void)
{
    bool time_to_communicate = tim3_get_counter() >= 122;

    if (time_to_communicate)
        tim3_set_counter(0);

    return time_to_communicate;
}

bool radio_out_data_is_new(const data_to_robot_t *outcoming)
{
    static data_to_robot_t cache;

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

static uint8_t radio_find_free_channel(void)
{
    const uint8_t num_areas = 18;
    const uint8_t area_length = NRF24L01_CHANNELS / num_areas;
    
    uint8_t noise_buffer[NRF24L01_CHANNELS];
    uint8_t average_noise = 0xFF;
    uint8_t cleanest_channel = RADIO_INITIAL_CHANNEL;
    uint8_t i, j;

    nrf24l01_measure_noise(noise_buffer, 0, NRF24L01_CHANNELS - 1);

    for (i = 0; i < num_areas; i++) {
        uint8_t cleanest_channel_of_area = RADIO_INITIAL_CHANNEL;
        uint16_t average_noise_of_area = 0;
        uint8_t min_noise_of_area = 0xFF;

        for (j = 0; j < area_length; j++) {
            uint8_t channel = i * area_length + j;
            uint8_t noise = noise_buffer[channel];

            if (noise < min_noise_of_area) {
                min_noise_of_area = noise;
                cleanest_channel_of_area = channel;
            }
            average_noise_of_area += noise;
        }
        average_noise_of_area /= area_length;

        if (average_noise_of_area < average_noise) {
            average_noise = average_noise_of_area;
            cleanest_channel = cleanest_channel_of_area;
        }

    }
    return cleanest_channel;
}