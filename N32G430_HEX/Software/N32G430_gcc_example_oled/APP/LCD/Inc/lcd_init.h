/**
 * @file lcd_init.h
 * @brief LCD ST7735S initialization header - supports SPI1 hardware interface
 */

#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "main.h"
#include "spi.h"

/* LCD orientation configuration */
#define USE_HORIZONTAL 3  // 0: 128x160, 1: 160x128, 2: 128x160, 3: 160x128

#if USE_HORIZONTAL==0 || USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 128
#endif

/* ============================================================================
 * LCD Pin Control Macros - Using SPI1 shared pins
 * ============================================================================
 *
 * SPI1 pins (hardware SPI):
 *   SCK:  PA5 (AF1)
 *   MOSI: PA7 (AF1)
 *
 * Control pins (GPIO):
 *   RES:  PA6 (GPIO output)
 *   DC:   PB0 (GPIO output)
 *   CS:   PB1 (GPIO output)
 *   BL:   PB10 (GPIO output, backlight)
 */

/* RES pin control - PA6 */
#define LCD_RES_Clr()   DISPLAY_RES_LOW()
#define LCD_RES_Set()   DISPLAY_RES_HIGH()

/* DC pin control - PB0 */
#define LCD_DC_Clr()    DISPLAY_DC_LOW()
#define LCD_DC_Set()    DISPLAY_DC_HIGH()

/* CS pin control - PB1 */
#define LCD_CS_Clr()    DISPLAY_CS_LOW()
#define LCD_CS_Set()    DISPLAY_CS_HIGH()

/* Backlight control - PB10 */
#define LCD_BLK_Set()   LCD_BL_HIGH()
#define LCD_BLK_Clr()   LCD_BL_LOW()

/* Function declarations */
void LCD_GPIO_Init(void);
void LCD_Writ_Bus(u8 dat);
void LCD_WR_DATA8(u8 dat);
void LCD_WR_DATA(u16 dat);
void LCD_WR_DATA_DMA(u8 *data, u16 len);  /* DMA批量传输 */
void LCD_WR_REG(u8 dat);
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_Init(void);

#endif
