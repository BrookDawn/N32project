/**
 * @file lcd_test_demo.h
 * @brief LCD ST7735S test demo header
 */

#ifndef LCD_TEST_DEMO_H
#define LCD_TEST_DEMO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize LCD test demo
 */
void LCD_TestDemo_Init(void);

/**
 * @brief LCD test demo main loop
 */
void LCD_TestDemo_Main(void);

/**
 * @brief LCD color fill test
 */
void LCD_TestDemo_ColorFill(void);

/**
 * @brief LCD text display test
 */
void LCD_TestDemo_Text(void);

/**
 * @brief LCD graphics test
 */
void LCD_TestDemo_Graphics(void);

#ifdef __cplusplus
}
#endif

#endif /* LCD_TEST_DEMO_H */
