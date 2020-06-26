/**
 * DEPRECATED test,
 *
 * но я не хочу удалять этот файл, поскольку реализация delay_ms
 * на 8-битном таймере выглядит красиво.
 */

#include <stdint.h>
#include <unity.h>

/*
 * Симуляция миллисекундной задержки на 8-битном таймере.
 */
static uint16_t delay_ms(uint16_t ms)
{
    uint8_t ms_msb = ms >> 8;
    uint8_t ms_lsb = ms & 0xFF;
    uint8_t ovf = ms_msb;
    uint8_t counter = 0;

    uint16_t accumulator = 0;

    while (ms_msb--) {
        counter = 0;
        while (counter++ < 0xFF)
            accumulator++;
    }

    counter = 0;
    while (counter++ < ovf)
        accumulator++;

    counter = 0;
    while (counter++ < ms_lsb)
        accumulator++;

    return accumulator;
}

void test_delay(void)
{
    TEST_ASSERT_EQUAL(0x00FF, delay_ms(0x00FF));
    TEST_ASSERT_EQUAL(0x00F0, delay_ms(0x00F0));
    TEST_ASSERT_EQUAL(0x0001, delay_ms(0x0001));
    TEST_ASSERT_EQUAL(0x0017, delay_ms(0x0017));

    TEST_ASSERT_EQUAL(0x0201, delay_ms(0x0201));
    TEST_ASSERT_EQUAL(0x0200, delay_ms(0x0200));
    TEST_ASSERT_EQUAL(0x0101, delay_ms(0x0101));
    TEST_ASSERT_EQUAL(0x0100, delay_ms(0x0100));
    TEST_ASSERT_EQUAL(0xFFFF, delay_ms(0xFFFF));
    TEST_ASSERT_EQUAL(0x1010, delay_ms(0x1010));
    TEST_ASSERT_EQUAL(0x0101, delay_ms(0x0101));
}