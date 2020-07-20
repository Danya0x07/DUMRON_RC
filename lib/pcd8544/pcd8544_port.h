/**
 * @file    pcd8544_port.h
 * @brief   Платформозависимая часть библиотеки.
 *
 * Путём реализации интерфейса, объявленного в данном файле можно перенести
 * библиотеку на любую платформу.
 *
 * @note
 * В данном файле можно подключать любые заголовочные файлы, необходимые
 * для реализации объявленного интерфейса.
 */

#ifndef _PCD8544_PORT_H
#define _PCD8544_PORT_H

#include <halutils.h>
#include <config.h>

/**
 * @name    Макросы для управления пинами дисплея.
 * @{
 */
#define _ce_high()  GPIO_WriteHigh(LCD_GPORT, LCD_CE_GPIN)
#define _ce_low()   GPIO_WriteLow(LCD_GPORT, LCD_CE_GPIN)
#define _dc_high()  GPIO_WriteHigh(LCD_GPORT, LCD_DC_GPIN)
#define _dc_low()   GPIO_WriteLow(LCD_GPORT, LCD_DC_GPIN)
#define _rst_high() GPIO_WriteHigh(LCD_GPORT, LCD_RST_GPIN)
#define _rst_low()  GPIO_WriteLow(LCD_GPORT, LCD_RST_GPIN)
/** @} */

/**
 * @brief   Обменивается байтом по SPI.
 *
 * @note
 * Если в проекте используется несколько библиотек, работающих с SPI, то будет
 * целесообразно вынести соответствующую функцию в отдельный файл, а в этой
 * функции просто делегировать ей работу.
 */
static inline uint8_t _spi_send_byte(uint8_t byte)
{
    return spi_transfer_byte(byte);
}

#define _delay_ms(ms)   delay_ms(ms)

#define LOOKUP_TABLE_MEMORY_SPECIFIER

static inline uint8_t _lookup(uint8_t *addr)
{
    return *byte_addr;
}

#endif /* _PCD8544_PORT_H */