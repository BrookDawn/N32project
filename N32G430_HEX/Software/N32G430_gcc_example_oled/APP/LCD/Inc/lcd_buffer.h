/**
 * @file lcd_buffer.h
 * @brief LCD double buffering and DMA-accelerated drawing
 * @author N32G430 LCD Driver
 * @version 1.0
 */

#ifndef LCD_BUFFER_H
#define LCD_BUFFER_H

#include "lcd_init.h"
#include "hal_compat.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Frame buffer configuration */
#define LCD_BUFFER_SIZE         (LCD_W * LCD_H * 2)  /* 16-bit color, full screen */
#define LCD_USE_DOUBLE_BUFFER   0                     /* Disable double buffering to save RAM */

/* Color definitions (RGB565) */
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_ORANGE    0xFC00
#define COLOR_PURPLE    0x8010
#define COLOR_GRAY      0x8410
#define COLOR_DARK_GRAY 0x4208

/**
 * @brief LCD buffer context
 */
typedef struct {
    uint8_t *framebuffer;           /* Current drawing buffer */
    uint8_t *display_buffer;        /* Buffer being displayed */
    uint16_t width;
    uint16_t height;
    volatile bool transfer_complete;
    volatile bool is_busy;
} LCD_BufferContext_t;

/* Public API */

/**
 * @brief Initialize LCD buffering system
 * @return HAL_OK on success
 */
HAL_StatusTypeDef LCD_Buffer_Init(void);

/**
 * @brief Get the current framebuffer for drawing
 * @return Pointer to framebuffer (RGB565 format)
 */
uint16_t* LCD_Buffer_GetFramebuffer(void);

/**
 * @brief Swap buffers and trigger DMA transfer to LCD
 * @note Non-blocking, use LCD_Buffer_WaitComplete() to wait
 * @return HAL_OK on success
 */
HAL_StatusTypeDef LCD_Buffer_SwapBuffers(void);

/**
 * @brief Wait for DMA transfer to complete
 */
void LCD_Buffer_WaitComplete(void);

/**
 * @brief Check if DMA transfer is busy
 * @return true if busy, false if ready
 */
bool LCD_Buffer_IsBusy(void);

/**
 * @brief Clear framebuffer with a solid color
 * @param color: RGB565 color value
 */
void LCD_Buffer_Clear(uint16_t color);

/**
 * @brief Fast fill rectangle in framebuffer
 * @param x, y: Top-left coordinates
 * @param w, h: Width and height
 * @param color: RGB565 color
 */
void LCD_Buffer_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Draw a pixel in framebuffer
 * @param x, y: Coordinates
 * @param color: RGB565 color
 */
void LCD_Buffer_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief Draw a line in framebuffer (Bresenham algorithm)
 * @param x1, y1: Start coordinates
 * @param x2, y2: End coordinates
 * @param color: RGB565 color
 */
void LCD_Buffer_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

/**
 * @brief Draw a circle in framebuffer (midpoint algorithm)
 * @param x0, y0: Center coordinates
 * @param r: Radius
 * @param color: RGB565 color
 */
void LCD_Buffer_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

/**
 * @brief Fill a circle in framebuffer
 * @param x0, y0: Center coordinates
 * @param r: Radius
 * @param color: RGB565 color
 */
void LCD_Buffer_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

/**
 * @brief DMA transfer complete callback (called from interrupt)
 */
void LCD_Buffer_DMAComplete(void);

#ifdef __cplusplus
}
#endif

#endif /* LCD_BUFFER_H */
