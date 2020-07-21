/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

/**
 * @file    pcd8544.h
 * @brief   Главный заголовочный файл библиотеки.
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

enum pcd8544_mode {
    PCD8544_MODE_BLANK = 0,
    PCD8544_MODE_FILLED = 1,
    PCD8544_MODE_NORMAL = 4,
    PCD8544_MODE_INVERTED = 5,
};

struct pcd8544_config {
    uint8_t brightness;
    uint8_t contrast;
    uint8_t temperature_coeff;
};

struct pcd8544_image {
    const uint8_t *bitmap;
    uint8_t width_px;
    uint8_t height_pg;
    bool lookup;
};

/**
 * @brief Перезагружает дисплей.
 * @note  Эту функцию НЕОБХОДИМО вызвать не позже чем через
 *        100 миллисекунд после подачи питания на дисплей.
 * @note  Эта функция платформозависимая.
 */
void pcd8544_reset(void);

void pcd8544_set_power(bool pwr);

void pcd8544_configure(struct pcd8544_config *config);

void pcd8544_set_mode(enum pcd8544_mode mode);

void pcd8544_set_brush(bool inverse);

/**
 * @brief Устанавливает курсор в указанную символьную позицию.
 * @note  0 <= x <= 13, отсчёт слева направо;
 * @note  0 <= y <= 5   отсчёт сверху вниз;
 */
void pcd8544_set_cursor(uint8_t column, uint8_t row);

void pcd8544_set_addr(uint8_t x, uint8_t page);

void pcd8544_print_c(char c);

void pcd8544_print_s(const char *s);

void pcd8544_draw_img(uint8_t x, uint8_t page, const struct pcd8544_image *img);

void pcd8544_clear(void);

#endif /* _PCD8544_H */