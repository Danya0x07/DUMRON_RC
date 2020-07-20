/**
 * @file    nrf24l01_port.h
 * @brief   Платформозависимая часть библиотеки.
 *
 * Путём реализации интерфейса, объявленного в данном файле можно перенести
 * библиотеку на любую платформу.
 *
 * @note
 * В данном файле можно подключать любые заголовочные файлы, необходимые
 * для реализации объявленного интерфейса.
 */

#ifndef _NRF24L01_PORT_H
#define _NRF24L01_PORT_H

#include <halutils.h>
#include <config.h>

/**
 * @name    Макросы для управления выводами трансивера.
 * @{
 */
#define _csn_high() GPIO_WriteHigh(RADIO_GPORT, RADIO_CSN_GPIN)
#define _csn_low()  GPIO_WriteLow(RADIO_GPORT, RADIO_CSN_GPIN)
#define _ce_high()  GPIO_WriteHigh(RADIO_GPORT, RADIO_CE_GPIN)
#define _ce_low()   GPIO_WriteLow(RADIO_GPORT, RADIO_CE_GPIN)
/** @} */

/**
 * @brief   Обменивается байтом по SPI.
 *
 * @note
 * Если в проекте используется несколько библиотек, работающих с SPI, то будет
 * целесообразно вынести соответствующую функцию в отдельный файл, а в этой
 * функции просто делегировать ей работу.
 *
 * @bug
 * Если функция, которой данная функция/макрос делегирует обмен байтом по SPI,
 * находится в отдельной единице трансляции, то как минимум для
 * arm-none-eabi-gcc v6.3.1 имеет место баг, вызванный оптимизацией компилятора.
 * Баг проявляется на уровнях оптимизации, отличных от -O0 и -Og.
 * Симптомы: некорректные значения при передаче/приёме данных по SPI.
 */
static inline uint8_t _spi_transfer_byte(uint8_t byte)
{
    return spi_transfer_byte(byte);
}

/**
 * @brief   Обменивается массивом байт по SPI.
 *
 * @param[in] out   Буфер исходящих данных. NULL, если не исопльзуется.
 * @param[out] in   Буфер входящих данных. NULL, если не исопльзуется.
 * @param len   Количество байт для обмена.
 *
 * @note
 * Если в проекте используется несколько библиотек, работающих с SPI, то будет
 * целесообразно вынести соответствующую функцию в отдельный файл, а в этой
 * функции просто делегировать ей работу.
 *
 * @warning
 * in и out не могут быть NULL одновременно.
 */
static inline void _spi_transfer_bytes(uint8_t *in, const uint8_t *out,
                                       uint8_t len)
{
    spi_transfer_bytes(in, out, len);
}

/** Функция миллисекундной задержки. */
#define _delay_ms(ms)   delay_ms((ms))

#endif /* _NRF24L01_PORT_H */
