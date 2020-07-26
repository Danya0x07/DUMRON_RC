/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

/**
 * @file
 * @brief   PCD8544 LCD controller library interface.
 *
 * @todo    Добавить поддержку могучего языка;
 * @todo    Добавить масштабирование;
 * @todo    Добавить возможность работать с дисплеем через фреймбуфер;
 * @todo    Добавить рисование картинок.
 * @todo    Добавить рисование всякой геометрии;
 * @todo    Добавить "экономное" обновление дисплея;
 */

#ifndef _PCD8544_H
#define _PCD8544_H

#include "pcd8544_conf.h"

#ifndef PCD8544_USE_FRAMEBUFFER
#   error PCD8544_USE_FRAMEBUFFER macro not found in "pcd8544_conf.h".
#endif

/**
 * @brief   PCD8544 display modes.
 */
enum pcd8544_mode {
    PCD8544_MODE_BLANK = 0,
    PCD8544_MODE_ALL_ON = 1,
    PCD8544_MODE_NORMAL = 4,
    PCD8544_MODE_INVERTED = 5,
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
        .lookup = FALSE,
        .width_px = 3,
        .height_pg = 2
    };
 * @endcode
 *
 * @note
 * In case of AVR, if `bitmap_arr` is declared with `PROGMEM` qualifier,
 * `image.lookup` will be TRUE.
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
     * array, this field must be set to TRUE(1), otherwise FALSE(0).
     */
    bool lookup;
    /**
     * Width of the bitmap in pixels.
     */
    uint8_t width_px;
    /**
     * Height of the bitmap in pages.
     */
    uint8_t height_pg;
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
 * @note
 * This function also wakes PCD8544 out of power-down mode if it was selected.
 *
 * @param[in] config    Address of a PCD8544 configuration being applied.
 */
void pcd8544_configure(struct pcd8544_config *config);

/**
 * @brief   Control the PCD8544 power mode.
 *
 * Entering power-down mode can reduce current consumption of the PCD8544.
 *
 * @note
 * Before entering power-down mode, the PCD8544 DDRAM is filled with '0's to
 * ensure specified current consumption.
 * However, the framebuffer, if used, is not flushed.
 *
 * @param[in] pwr   1 to select chip-active mode, 0 to select power-down mode.
 */
void pcd8544_set_power(bool pwr);

/**
 * @brief   Set the PCD8544 display mode.
 *
 * @param[in] mode  PCD8544 display mode to set.
 */
void pcd8544_set_mode(enum pcd8544_mode mode);

/**
 * @brief   Setup drawing aspects.
 *
 * @param[in] inverse       Whether to invert bits of every byte being written
 *                          to PCD8544 DDRAM or framebuffer if used.
 * @param[in] font_size
 * @param[in] image_scale
 */
void pcd8544_setup_brush(bool inverse, uint8_t font_size, uint8_t image_scale);

/**
 * @brief
 */
void pcd8544_set_cursor(uint8_t column, uint8_t row);

/**
 *
 */
void pcd8544_set_addr(uint8_t x, uint8_t page);

/**
 * @brief   Print a single character on the display.
 *
 * @param[in] c     Character to print.
 */
void pcd8544_print_c(char c);

/**
 * @brief   Print a string on the display.
 *
 * If the string does not fit on a horizontal row, the overflowing part is
 * printed on the row below. If there is no rows below current one, the
 * overflowing part of the string is not printed at all.
 *
 * @param[in] s     String to print.
 */
void pcd8544_print_s(const char *s);

/**
 *
 */
void pcd8544_draw_img(uint8_t x, uint8_t page, const struct pcd8544_image *img);

/**
 *
 */
void pcd8544_draw_img_part(uint8_t x, uint8_t page,
                           const struct pcd8544_image *img,
                           uint8_t x0, uint8_t x1,
                           uint8_t y0, uint8_t y1);

/**
 * @brief   Fill the PCD8544 DDRAM, or framebuffer instead if used, with '0's.
 */
void pcd8544_clear(void);

#if (PCD8544_USE_FRAMEBUFFER > 0)
/**
 * @brief   Write contents of the framebuffer to PCD8544 DDRAM.
 *
 * If framebuffer is used, every function that writes any information to the
 * PCD8544 DDRAM writes it to the framebuffer instead. And the PCD8544 DDRAM can
 * be updated by calling this function.
 */
void pcd8544_update(void);
#endif

#endif /* _PCD8544_H */