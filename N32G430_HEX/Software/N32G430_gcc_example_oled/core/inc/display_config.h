/**
 * @file display_config.h
 * @brief 显示设备配置 - 选择SPI1驱动OLED或LCD
 */

#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 显示设备类型选择
 *
 * 通过定义以下宏来选择SPI1驱动的显示设备：
 *
 * DISPLAY_USE_OLED - 使用OLED显示屏 (SSD1306/SH1106)
 * DISPLAY_USE_LCD  - 使用LCD显示屏 (ST7735S)
 *
 * 默认使用OLED，如需使用LCD，请在编译时定义 DISPLAY_USE_LCD
 * 或在包含此头文件前定义
 */

// #define DISPLAY_USE_OLED
// #define DISPLAY_USE_LCD
 
/* 显示设备类型枚举 */
typedef enum {
    DISPLAY_TYPE_OLED = 0,
    DISPLAY_TYPE_LCD
} DisplayType_t;

/* 默认配置 - 如果两个都没定义，默认使用OLED */
#if !defined(DISPLAY_USE_OLED) && !defined(DISPLAY_USE_LCD)
#define DISPLAY_USE_OLED
#endif

/* 确保只能选择一种设备 */
#if defined(DISPLAY_USE_OLED) && defined(DISPLAY_USE_LCD)
#error "Cannot use both OLED and LCD at the same time. Please define only one."
#endif

/* 引脚配置 */

/* 公共引脚 (SPI1) */
/* SCK:  PA5 (AF1) */
/* MOSI: PA7 (AF1) */
/* RES:  PA6 (GPIO) */
/* DC:   PB0 (GPIO) */
/* CS:   PB1 (GPIO) */

/* LCD特有引脚 */
/* BL:   PB10 (GPIO) - 仅LCD使用背光控制 */

#ifdef DISPLAY_USE_LCD
#define DISPLAY_HAS_BACKLIGHT   1
#else
#define DISPLAY_HAS_BACKLIGHT   0
#endif

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_CONFIG_H */
