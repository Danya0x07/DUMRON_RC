/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

/**
 * @file    pcd8544_port.h
 * @brief   The platform-specific part of the PCD8544 LCD controller library.
 *
 * By implementing the interface declared in this file, you can port the library
 * to any platform.
 *
 * @note
 * In this file you can include any headers you need to implement the declared
 * interface.
 */

#ifndef _PCD8544_PORT_H
#define _PCD8544_PORT_H

#include <halutils.h>
#include <config.h>

/**
 * @name    Macros for managing pins connected to PCD8544.
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
 * @brief   Send a byte via SPI.
 *
 * @note
 * If the project uses several libraries that work with SPI, then it will be
 * advisable to move the corresponding function into a separate file, and
 * in this function, inlining it, simply delegate the work to that function.
 */
static inline void _spi_send_byte(uint8_t byte)
{
    spi_transfer_byte(byte);
}

/** Millisecond delay function. */
#define _delay_ms(ms)   delay_ms(ms)

/**
 * @brief   Look up a byte in the program memory.
 *
 * This function is used to retrieve bytes of bitmaps that are placed in
 * program memory. They are bitmaps from font tables and user created images,
 * which have the pcd8544_image.lookup field set to `true`.
 * If no additional operations are required to retrieve data from program
 * memory, the return value of this function must be `*pbyte`.
 *
 * For example, in case of AVR we should return `pgm_read_byte(pbyte)`.
 *
 * @param[in] pbyte     Address of a byte being looked up.
 *
 * @return  The value of the byte with the given address.
 */
static inline uint8_t _lookup(const uint8_t *pbyte)
{
    /* In case of AVR we should return pgm_read_byte(pbyte). */
    return *pbyte;
}

#endif  /* _PCD8544_PORT_H */