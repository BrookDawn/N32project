/**
 * @file lcd_interface.h
 * @brief LCD hardware abstraction layer - SPI interface for ST7735S LCD
 */

#ifndef LCD_INTERFACE_H
#define LCD_INTERFACE_H

#include "hal_compat.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LCD Interface structure
 */
typedef struct LCD_Interface {
    /* SPI handle pointer */
    void *handle;

    /* Send command byte */
    HAL_StatusTypeDef (*send_cmd)(struct LCD_Interface *iface, uint8_t cmd);

    /* Send data byte */
    HAL_StatusTypeDef (*send_data8)(struct LCD_Interface *iface, uint8_t data);

    /* Send 16-bit data (color) */
    HAL_StatusTypeDef (*send_data16)(struct LCD_Interface *iface, uint16_t data);

    /* Send data buffer */
    HAL_StatusTypeDef (*send_data)(struct LCD_Interface *iface, uint8_t *data, uint16_t len);

    /* Send data buffer via DMA (non-blocking) */
    HAL_StatusTypeDef (*send_data_dma)(struct LCD_Interface *iface, uint8_t *data, uint16_t len);

    /* Check if DMA transfer is busy */
    bool (*is_dma_busy)(struct LCD_Interface *iface);

    /* Delay function */
    void (*delay_ms)(uint32_t delay);

    /* Reset display */
    void (*reset)(struct LCD_Interface *iface);

    /* Backlight control */
    void (*backlight_on)(struct LCD_Interface *iface);
    void (*backlight_off)(struct LCD_Interface *iface);

} LCD_Interface_t;

/**
 * @brief Initialize SPI interface for LCD
 * @param iface: Interface structure to initialize
 * @param hspi: SPI handle pointer
 * @param use_dma: Whether to use DMA for data transfer
 */
void LCD_Interface_InitSPI(LCD_Interface_t *iface, void *hspi, bool use_dma);

/**
 * @brief Get the global LCD interface instance
 * @return Pointer to LCD interface
 */
LCD_Interface_t* LCD_Interface_GetInstance(void);

#ifdef __cplusplus
}
#endif

#endif /* LCD_INTERFACE_H */
