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
#include <stm8s.h>
#include <stddef.h>

/** Use framebuffer (1), or not (0) */
#define PCD8544_USE_FRAMEBUFFER 1

#endif /* _PCD8544_CONF_H */