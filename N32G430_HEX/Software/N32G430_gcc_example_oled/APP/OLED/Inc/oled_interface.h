/**
 * @file oled_interface.h
 * @brief OLED hardware abstraction layer - supports both I2C and SPI interfaces
 */

#ifndef OLED_INTERFACE_H
#define OLED_INTERFACE_H

#include "hal_compat.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Interface type selection */
typedef enum {
    OLED_INTERFACE_I2C = 0,
    OLED_INTERFACE_SPI
} OLED_InterfaceType_t;

/* Forward declaration */
struct OLED_Interface;

/**
 * @brief Hardware interface function pointers
 */
typedef struct OLED_Interface {
    OLED_InterfaceType_t type;

    /* Send command byte */
    HAL_StatusTypeDef (*send_cmd)(struct OLED_Interface *iface, uint8_t cmd);

    /* Send data buffer */
    HAL_StatusTypeDef (*send_data)(struct OLED_Interface *iface, uint8_t *data, uint16_t len);

    /* Send data buffer via DMA (non-blocking) */
    HAL_StatusTypeDef (*send_data_dma)(struct OLED_Interface *iface, uint8_t *data, uint16_t len);

    /* Check if DMA transfer is busy */
    bool (*is_dma_busy)(struct OLED_Interface *iface);

    /* Delay function */
    void (*delay_ms)(uint32_t delay);

    /* Reset display (SPI only, can be NULL for I2C) */
    void (*reset)(struct OLED_Interface *iface);

    /* Interface-specific data (I2C handle, SPI handle, etc.) */
    void *handle;

    /* I2C specific */
    uint8_t i2c_addr;

} OLED_Interface_t;

/**
 * @brief Initialize I2C interface for OLED
 * @param iface: Interface structure to initialize
 * @param hi2c: I2C handle pointer
 * @param i2c_addr: I2C slave address
 * @param use_dma: Whether to use DMA for data transfer
 */
void OLED_Interface_InitI2C(OLED_Interface_t *iface, void *hi2c, uint8_t i2c_addr, bool use_dma);

/**
 * @brief Initialize SPI interface for OLED
 * @param iface: Interface structure to initialize
 * @param hspi: SPI handle pointer
 * @param use_dma: Whether to use DMA for data transfer
 */
void OLED_Interface_InitSPI(OLED_Interface_t *iface, void *hspi, bool use_dma);

#ifdef __cplusplus
}
#endif

#endif /* OLED_INTERFACE_H */
