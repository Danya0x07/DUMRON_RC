#include <stdint.h>
#include <stdlib.h>
#include <unity.h>

#define NRF24L01_CHANNELS   42

static uint8_t noise_buffer_mock_1[7][6] = {
    {0xAA, 0xCC, 0xFF, 0xDD, 0xBB, 0x33},
    {0xAA, 0xCC, 0xFF, 0xDD, 0xBB, 0x33},
    {0xAA, 0xCC, 0xFF, 0xDD, 0xBB, 0x33},
    {0x44, 0x33, 0x22, 0x00, 0x00, 0x00},
    {0xAA, 0xCC, 0xFF, 0xDD, 0xBB, 0x33},
    {0xAA, 0xCC, 0xFF, 0xDD, 0xBB, 0x33},
    {0xAA, 0xCC, 0xFF, 0xDD, 0xBB, 0x33}
};

uint8_t noise_buffer[NRF24L01_CHANNELS] = {0};

static uint8_t find_cleanest_channel(void)
{
    const uint8_t num_areas = 7;
    const uint8_t area_length = NRF24L01_CHANNELS / num_areas;
    uint8_t average_noise = 0xFF;
    uint8_t free_channel;
    uint8_t i, j;

    // nrf24l01_measure_noise(noise_buff, 0, NRF24L01_CHANNELS - 1);

    for (i = 0; i < num_areas; i++) {
        uint8_t cleanest_rf_ch;
        uint8_t min_noise = 0xFF;
        uint16_t average = 0;

        for (j = 0; j < area_length; j++) {
            uint8_t channel = i * area_length + j;
            uint8_t noise = noise_buffer[channel];

            if (noise < min_noise) {
                min_noise = noise;
                cleanest_rf_ch = channel;
            }
            average += noise;
        }
        average /= area_length;

        if (average < average_noise) {
            average_noise = average;
            free_channel = cleanest_rf_ch;
        }

    }
    return free_channel;
}

void test_find_clear_rf_ch(void)
{
    TEST_ASSERT_EQUAL(0, find_cleanest_channel());

    memcpy(noise_buffer, noise_buffer_mock_1, NRF24L01_CHANNELS);
    TEST_ASSERT_EQUAL(21, find_cleanest_channel());
}