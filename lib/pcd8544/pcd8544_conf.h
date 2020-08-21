/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

/**
 * @file
 * @brief   PCD8544 LCD controller library configuration file.
 *
 * This file contains configuration parameters that should be adjusted for your
 * particular application requirements.
 */

#ifndef _PCD8544_CONF_H
#define _PCD8544_CONF_H

/*
 * Headers that provide standard integer types and bool type.
 * They are here because standard libraries from some vendors provide their
 * standard types, so there might be conflicts with standard C headers and we
 * need an ability to replace them with custom headers.
 */
#include <stddef.h>
#include <stm8s.h>

/** Use framebuffer (1), or not (0) */
#define PCD8544_USE_FRAMEBUFFER 0

/**
 * @brief   Possible languages for the text printed on PCD8544 LCD.
 *
 * This library supports multiple languages, but only in single-byte encodings.
 * This means that you need to save source files containing national literals in
 * the appropriate single-byte encodings. For example, cp866 or windows-1251 or
 * KOI8-R for russian language.
 *
 * @see _get_character_bitmap()
 */
enum pcd8544_language {
    PCD8544_LANG_EN,
    PCD8544_LANG_RU
    /* ... More can be added as needed. */
};

#endif /* _PCD8544_CONF_H */