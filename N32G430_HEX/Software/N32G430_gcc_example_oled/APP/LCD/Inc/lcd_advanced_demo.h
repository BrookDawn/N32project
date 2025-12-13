/**
 * @file lcd_advanced_demo.h
 * @brief Advanced LCD demo with animations and effects
 */

#ifndef LCD_ADVANCED_DEMO_H
#define LCD_ADVANCED_DEMO_H

#include "hal_compat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize advanced LCD demo
 */
void LCD_AdvancedDemo_Init(void);

/**
 * @brief Main loop for advanced demo
 */
void LCD_AdvancedDemo_Main(void);

#ifdef __cplusplus
}
#endif

#endif /* LCD_ADVANCED_DEMO_H */
