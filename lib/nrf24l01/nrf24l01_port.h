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

#include <stm8s.h>
#include <../../include/delay.h>

/* Пины модуля */
#define NRF_GPIO    GPIOC
#define NRF_PIN_CE  GPIO_PIN_4
#define NRF_PIN_CSN GPIO_PIN_3

/**
 * @name    Макросы для управления выводами трансивера.
 * @{
 */
#define _csn_high() GPIO_WriteHigh(NRF_GPIO, NRF_PIN_CSN)
#define _csn_low()  GPIO_WriteLow(NRF_GPIO, NRF_PIN_CSN)
#define _ce_high()  GPIO_WriteHigh(NRF_GPIO, NRF_PIN_CE)
#define _ce_low()   GPIO_WriteLow(NRF_GPIO, NRF_PIN_CE)
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
static uint8_t _spi_transfer_byte(uint8_t byte)
{
    while (!SPI_GetFlagStatus(SPI_FLAG_TXE));
    SPI_SendData(byte);
    while (!SPI_GetFlagStatus(SPI_FLAG_RXNE));
    return SPI_ReceiveData();
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
static void _spi_transfer_bytes(uint8_t *in, const uint8_t *out,
                                       uint8_t len)
{
    if (in == NULL) {
        while (len--)
            _spi_transfer_byte(*out++);
    } else if (out == NULL) {
        while (len--)
            *in++ = _spi_transfer_byte(0);
    } else {
        while (len--)
            *in++ = _spi_transfer_byte(*out++);
    }
}

/** Функция миллисекундной задержки. */
#define _delay_ms(ms)   delay_ms((ms))

#endif /* _NRF24L01_PORT_H */
