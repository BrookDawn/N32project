/**
 * @file lcd_buffer.c
 * @brief LCD buffering implementation with DMA acceleration (optimized for low RAM)
 */

#include "lcd_buffer.h"
#include "lcd_init.h"
#include "spi.h"
#include <string.h>

/* Partial buffer allocation (1/4 screen to fit in 32KB RAM) */
#define BUFFER_HEIGHT (LCD_H / 4)
static uint16_t buffer0[LCD_W * BUFFER_HEIGHT] __attribute__((aligned(4)));

/* LCD buffer context */
static LCD_BufferContext_t lcd_ctx = {
    .framebuffer = (uint8_t *)buffer0,
    .display_buffer = (uint8_t *)buffer0,
    .width = LCD_W,
    .height = LCD_H,
    .transfer_complete = true,
    .is_busy = false
};

/* Private helper functions */
static inline void swap_uint16(uint16_t *a, uint16_t *b) {
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

static inline uint16_t abs_int(int x) {
    return (x < 0) ? -x : x;
}

/**
 * @brief Initialize LCD buffering system
 */
HAL_StatusTypeDef LCD_Buffer_Init(void)
{
    lcd_ctx.framebuffer = (uint8_t *)buffer0;
    lcd_ctx.display_buffer = (uint8_t *)buffer0;
    lcd_ctx.transfer_complete = true;
    lcd_ctx.is_busy = false;

    /* Clear buffer */
    memset(buffer0, 0x00, sizeof(buffer0));

    return HAL_OK;
}

/**
 * @brief Get current framebuffer pointer
 */
uint16_t* LCD_Buffer_GetFramebuffer(void)
{
    return (uint16_t *)lcd_ctx.framebuffer;
}

/**
 * @brief Send buffer to LCD via DMA
 */
static HAL_StatusTypeDef LCD_Buffer_TransferDMA(uint16_t *buffer, uint32_t size)
{
    HAL_StatusTypeDef status;

    /* Set address window to full screen */
    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);

    /* Prepare for data transmission */
    LCD_CS_Clr();
    LCD_DC_Set();

    /* Start DMA transfer */
    status = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)buffer, size);

    if (status != HAL_OK) {
        LCD_CS_Set();
        return status;
    }

    lcd_ctx.transfer_complete = false;
    lcd_ctx.is_busy = true;

    return HAL_OK;
}

/**
 * @brief Swap buffers and trigger DMA transfer (Direct mode - no swap)
 */
HAL_StatusTypeDef LCD_Buffer_SwapBuffers(void)
{
    HAL_StatusTypeDef status;

    /* Wait for previous transfer to complete */
    LCD_Buffer_WaitComplete();

    /* Transfer full screen buffer to LCD via DMA (in chunks) */
    /* For now, use blocking transfer to avoid complexity */
    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);
    LCD_CS_Clr();
    LCD_DC_Set();

    /* Send full screen using repeated buffer writes */
    uint32_t total_pixels = LCD_W * LCD_H;
    uint32_t pixels_per_chunk = LCD_W * BUFFER_HEIGHT;

    for (uint32_t chunk = 0; chunk < (LCD_H / BUFFER_HEIGHT); chunk++) {
        status = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)lcd_ctx.framebuffer, pixels_per_chunk * 2);
        if (status != HAL_OK) {
            LCD_CS_Set();
            return status;
        }
        LCD_Buffer_WaitComplete();
    }

    LCD_CS_Set();
    return HAL_OK;
}

/**
 * @brief Wait for DMA transfer to complete
 */
void LCD_Buffer_WaitComplete(void)
{
    while (lcd_ctx.is_busy) {
        /* Wait for DMA to finish */
        __NOP();
    }
}

/**
 * @brief Check if DMA is busy
 */
bool LCD_Buffer_IsBusy(void)
{
    return lcd_ctx.is_busy;
}

/**
 * @brief DMA transfer complete callback
 */
void LCD_Buffer_DMAComplete(void)
{
    LCD_CS_Set();
    lcd_ctx.transfer_complete = true;
    lcd_ctx.is_busy = false;
}

/**
 * @brief Clear framebuffer with solid color (optimized for partial buffer)
 */
void LCD_Buffer_Clear(uint16_t color)
{
    uint16_t *fb = (uint16_t *)lcd_ctx.framebuffer;
    uint32_t pixels_in_buffer = LCD_W * BUFFER_HEIGHT;

    for (uint32_t i = 0; i < pixels_in_buffer; i++) {
        fb[i] = color;
    }
}

/**
 * @brief Fast fill rectangle
 */
void LCD_Buffer_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (x >= LCD_W || y >= LCD_H) return;

    /* Clip to screen bounds */
    if (x + w > LCD_W) w = LCD_W - x;
    if (y + h > LCD_H) h = LCD_H - y;

    uint16_t *fb = (uint16_t *)lcd_ctx.framebuffer;

    for (uint16_t row = 0; row < h; row++) {
        uint16_t *line = &fb[(y + row) * LCD_W + x];
        for (uint16_t col = 0; col < w; col++) {
            line[col] = color;
        }
    }
}

/**
 * @brief Draw pixel (clip to buffer region)
 */
void LCD_Buffer_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= LCD_W || y >= BUFFER_HEIGHT) return;

    uint16_t *fb = (uint16_t *)lcd_ctx.framebuffer;
    fb[y * LCD_W + x] = color;
}

/**
 * @brief Draw line (Bresenham algorithm)
 */
void LCD_Buffer_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int dx = abs_int(x2 - x1);
    int dy = abs_int(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        LCD_Buffer_DrawPixel(x1, y1, color);

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/**
 * @brief Draw circle outline (midpoint algorithm)
 */
void LCD_Buffer_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        LCD_Buffer_DrawPixel(x0 + x, y0 + y, color);
        LCD_Buffer_DrawPixel(x0 + y, y0 + x, color);
        LCD_Buffer_DrawPixel(x0 - y, y0 + x, color);
        LCD_Buffer_DrawPixel(x0 - x, y0 + y, color);
        LCD_Buffer_DrawPixel(x0 - x, y0 - y, color);
        LCD_Buffer_DrawPixel(x0 - y, y0 - x, color);
        LCD_Buffer_DrawPixel(x0 + y, y0 - x, color);
        LCD_Buffer_DrawPixel(x0 + x, y0 - y, color);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

/**
 * @brief Fill circle (scanline algorithm)
 */
void LCD_Buffer_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        /* Draw horizontal lines for each scanline */
        for (int i = x0 - x; i <= x0 + x; i++) {
            LCD_Buffer_DrawPixel(i, y0 + y, color);
            LCD_Buffer_DrawPixel(i, y0 - y, color);
        }
        for (int i = x0 - y; i <= x0 + y; i++) {
            LCD_Buffer_DrawPixel(i, y0 + x, color);
            LCD_Buffer_DrawPixel(i, y0 - x, color);
        }

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}
