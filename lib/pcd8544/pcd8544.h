/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

/**
 * @file
 * @brief   PCD8544 LCD controller library interface.
 */

#ifndef _PCD8544_H
#define _PCD8544_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pcd8544_conf.h"

#ifndef PCD8544_USE_FRAMEBUFFER
#   error PCD8544_USE_FRAMEBUFFER macro not found in pcd8544_conf.h.
#endif

/**
 * @brief   PCD8544 display modes.
 */
enum pcd8544_mode {
    PCD8544_MODE_BLANK = 0,   /**< all pixels off */
    PCD8544_MODE_ALL_ON = 1,  /**< all pixels on */
    PCD8544_MODE_NORMAL = 4,  /**< normal mode */
    PCD8544_MODE_INVERTED = 5,  /**< inverted mode */
};

/**
 * @brief   PCD8544 configuration structure.
 *
 * @note
 * See datasheet section 'instructions' for more information.
 */
struct pcd8544_config {
    uint8_t bias;       /**< [0 - 7] */
    uint8_t contrast;   /**< [0 - 0x7F] */
    uint8_t tempcoeff;  /**< [0 - 3] */
};

/**
 * @name    PCD8544 default configuration values.
 * @{
 */
#define PCD8544_DEFAULT_BRIGHTNESS  0x38
#define PCD8544_DEFAULT_CONTRAST    0x03
#define PCD8544_DEFAULT_TEMPCOEFF   0
/** @} */

/**
 * @brief   PCD8544 image structure.
 *
 * Declaration example:
 * @code
    const uint8_t bitmap_arr[2][3] = {
        {0x40, 0x80, 0x40},
        {0x02, 0x01, 0x02}
    };

    struct pcd8544_image image = {
        .bitmap = (const uint8_t *)bitmap_arr,
        .lookup = false,
        .width_px = 3,  // sizeof(bitmap[0])
        .height_pg = 2  // sizeof(bitmap) / sizeof(bitmap[0])
    };
 * @endcode
 *
 * @note
 * In case of AVR, if `bitmap_arr` is declared with `PROGMEM` qualifier,
 * `image.lookup` will be `true`.
 */
struct pcd8544_image {
    /**
     * A pointer to one- or two-dismentional array with image bitmap.
     * Each row of the array represents one page of PCD8544's memory.
     * See the 'horizontal addressing' section in the datasheet.
     *
     * @note
     * Type cast might be required when initializing this field.
     */
    const uint8_t *bitmap;
    /**
     * If the @ref bitmap pointer points to array that is located in flash
     * memory and additional operations are needed to retrieve a byte of that
     * array, this field must be set to 1, otherwise 0.
     */
    bool lookup;
    uint8_t width_px;   /**< Width of the bitmap in pixels. */
    uint8_t height_pg;  /**< Height of the bitmap in pages. */
};

/**
 * @brief   Reset the PCD8544 LCD controller.
 *
 * @warning
 * This function must be called within 30 ms after LCD hardware is powered up.
 */
void pcd8544_reset(void);

/**
 * @brief   Configure the PCD8544 LCD controller.
 *
 * @param[in] config    Address of a PCD8544 configuration being applied.
 *
 * @note
 * This function also wakes PCD8544 out of power-down mode if it was selected.
 */
void pcd8544_configure(struct pcd8544_config *config);

/**
 * @brief   Control the PCD8544 power mode.
 *
 * Entering power-down mode can reduce current consumption of the PCD8544.
 *
 * @param[in] pwr   1 to select chip-active mode, 0 to select power-down mode.
 *
 * @note
 * Before entering power-down mode, the PCD8544 DDRAM is filled with '0's to
 * ensure specified current consumption.
 * However, the framebuffer, if used, is not flushed.
 */
void pcd8544_set_power(bool pwr);

/**
 * @brief   Set the PCD8544 display mode.
 *
 * @param[in] mode  PCD8544 display mode to set.
 */
void pcd8544_set_mode(enum pcd8544_mode mode);

/**
 * @brief   Setup aspects of drawing and printing.
 *
 * @param[in] inverse       Whether to invert bits of every byte being written
 *                          to PCD8544 DDRAM or framebuffer if used.
 *
 * @param[in] font_size     Text font size [1 - 3].
 * @param[in] image_scale   Image scaling factor [1 - 6].
 *
 * @note
 * The selected settings are valid until the next call to this function.
 */
void pcd8544_setup_brush(bool inverse, uint8_t font_size, uint8_t image_scale);

/**
 * @brief   Set text cursor position on the display.
 *
 * The coordinate range of cursor depends on the font size selected last time.
 *
 * @param[in] col   Vertical column, [0 - 13] for font_size == 1,
 *                  [0 - 6] for font_size == 2, [0 - 4] for font_size == 3.
 *
 * @param[in] row   Horizontal row, [0 - 5] for font_size == 1,
 *                  [0 - 2] for font_size == 2, [0 - 1] for font_size == 3.
 *
 * @see pcd8544_setup_brush()
 */
void pcd8544_set_cursor(uint8_t col, uint8_t row);

/**
 * @brief   Set language of the text printed on the display.
 *
 * The selected language determines from which font table and how bitmaps of
 * non-ASCII characters will be retrieved.
 *
 * @param[in] lang  Language to set for further printing.
 *
 * @see pcd8544_font.h
 */
void pcd8544_set_txt_language(enum pcd8544_language lang);

/**
 * @brief   Print a single character on the display.
 *
 * @param[in] c     Character to print.
 *
 * @see pcd8544_set_cursor()
 */
void pcd8544_print_c(char c);

/**
 * @brief   Print a string on the display.
 *
 * If the string does not fit on a horizontal row, the overflowing part is
 * printed on the row below. If there is no rows below current one, the
 * overflowing part of the string is not printed.
 *
 * @param[in] s     String to print.
 *
 * @see pcd8544_set_cursor()
 */
void pcd8544_print_s(const char *s);

/**
 * @brief   Print a string on the display in a virtual frame.
 *
 * With this function you can print a string in a limited rectangular area
 * of the display.
 *
 * @param[in] x0    Left border of the virtual frame [0 - 83].
 * @param[in] x1    Right border of the virtual frame [0 - 83].
 * @param[in] p1    Bottom border (page) of the virtual frame [0 - 5].
 * @param[in] s     String to print.
 *
 * @note
 * The p0 (top border) parameter is taken from last call to pcd8544_set_cursor()
 * and it musst be less then p1, otherwise result is incorrect. And x0 < x1.
 *
 * @see pcd8544_set_cursor()
 */
void pcd8544_print_s_f(uint8_t x0, uint8_t x1, uint8_t p1, const char *s);

/**
 * @brief   Draw an image on the display.
 *
 * Printing scaled images is useful. For example, to print a simple
 * logo that has no high-resolution details, we can save it to pcd8544_image
 * with width_px == 14 and height_pg == 1, and then scale it 6 times by calling
 * pcd8544_setup_brush(). Thus, it will fit the entire display, but it's bitmap
 * will take 14 instead of 504 bytes of memory.
 *
 * @param[in] x0    X coordinate of the left corner of the image [0 - 83].
 * @param[in] p0    A page from which to start drawing the image [0 - 5].
 *
 * @note
 * if the image does not fit on the display, the part that does not fit is
 * not drawn.
 */
void pcd8544_draw_img(uint8_t x0, uint8_t p0, const struct pcd8544_image *img);

/**
 * @brief   Clear the area of the display.
 *
 * @param[in] x0    X coordinate of the left corner of the area [0 - 83].
 * @param[in] p0    A page where the area starts [0 - 5].
 * @param[in] x1    X coordinate of the right corner of the area[0 - 83].
 * @param[in] p1    A page where the area ends [0 - 5]
 *
 * @note
 * x0 <= x1 and p0 <= p1, otherwise nothing is drawn.
 * If the last time pcd8544_setup_brush() was called with `inverse` argument
 * equal to 1, the clear color is inverted.
 */
void pcd8544_clear_area(uint8_t x0, uint8_t p0, uint8_t x1, uint8_t p1);

/**
 * @brief   Fill the PCD8544 DDRAM, or framebuffer instead if used, with '0's.
 *
 * @note
 * If the last time pcd8544_setup_brush() was called with `inverse` argument
 * equal to 1, the clear color is inverted.
 */
void pcd8544_clear(void);

#if (PCD8544_USE_FRAMEBUFFER == 1)
/**
 * @brief   Write contents of the framebuffer to PCD8544 DDRAM.
 *
 * If framebuffer is used, every function that writes any information to the
 * PCD8544 DDRAM writes it to the framebuffer instead. And the PCD8544 DDRAM can
 * be updated by calling this function.
 */
void pcd8544_update(void);

/**
 * @brief   Set the state of a pixel on the display.
 *
 * @param[in] x     X coordinate of a pixel [0 - 83].
 * @param[in] y     Y coordinate of a pixel [0 - 47].
 * @param[in] state The new state of the pixel (1 - filled, 0 - cleared).
 */
void pcd8544_draw_pixel(uint8_t x, uint8_t y, bool state);

/**
 * @brief   Draw a horizontal line on the display.
 *
 * @param[in] y     Y coordinate of the line [0 - 47].
 * @param[in] x0    The start X coordinate of the line [0 - 83].
 * @param[in] x1    The end X coordinate of the line [0 - 83].
 *
 * @note
 * x0 <= x1, otherwise nothing is drawn.
 */
void pcd8544_draw_hline(uint8_t y, uint8_t x0, uint8_t x1);

/**
 * @brief   Draw a vertical line on the display.
 *
 * @param[in] x     X coordinate of the line [0 - 83].
 * @param[in] y0    The start Y coordinate of the line [0 - 47].
 * @param[in] y1    The end Y coordinate of the line [0 - 47].
 *
 * @note
 * y0 <= y1, otherwise nothing is drawn.
 */
void pcd8544_draw_vline(uint8_t x, uint8_t y0, uint8_t y1);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _PCD8544_H */