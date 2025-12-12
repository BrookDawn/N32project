/**
 * @file spi.h
 * @brief Minimal SPI HAL-compatible interface for driving the OLED via SPI + DMA.
 */

#ifndef SPI_H
#define SPI_H

#include "hal_compat.h"
#include "n32g430_spi.h"
#include "n32g430_gpio.h"
#include "dma.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPI state enumeration
 */
typedef enum
{
    HAL_SPI_STATE_RESET   = 0x00U,
    HAL_SPI_STATE_READY   = 0x01U,
    HAL_SPI_STATE_BUSY    = 0x02U,
    HAL_SPI_STATE_BUSY_TX = 0x12U,
    HAL_SPI_STATE_ERROR   = 0x03U
} HAL_SPI_StateTypeDef;

/**
 * @brief SPI Handle structure definition
 */
typedef struct
{
    SPI_Module *Instance;
    HAL_SPI_StateTypeDef State;
    DMA_HandleTypeDef *hdmatx;
    uint8_t *pBuffPtr;
    uint16_t XferSize;
} SPI_HandleTypeDef;

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

/**
 * @brief Initialize SPI1 for OLED
 * @note  PA5: SCK, PA7: MOSI (SPI1 AF0)
 *        PA6: RES (GPIO output)
 *        PB0: DC (GPIO output)
 *        PB1: CS (GPIO output)
 */
void MX_SPI1_Init(void);

/**
 * @brief Deinitialize SPI
 * @param hspi: SPI handle pointer
 */
void HAL_SPI_DeInit(SPI_HandleTypeDef *hspi);

/**
 * @brief Blocking SPI transmit
 * @param hspi: SPI handle pointer
 * @param pData: Data buffer pointer
 * @param Size: Data size
 * @param Timeout: Timeout in milliseconds
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi,
                                   uint8_t *pData,
                                   uint16_t Size,
                                   uint32_t Timeout);

/**
 * @brief DMA SPI transmit
 * @param hspi: SPI handle pointer
 * @param pData: Data buffer pointer
 * @param Size: Data size
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi,
                                       uint8_t *pData,
                                       uint16_t Size);

/**
 * @brief SPI DMA transmit complete callback (weak function)
 * @param hspi: SPI handle pointer
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);

/* OLED SPI control pin macros */
/* RES pin: PA6 */
#define OLED_RES_GPIO_PORT    GPIOA
#define OLED_RES_GPIO_PIN     GPIO_PIN_6
#define OLED_RES_HIGH()       GPIO_Pins_Set(OLED_RES_GPIO_PORT, OLED_RES_GPIO_PIN)
#define OLED_RES_LOW()        GPIO_Pins_Reset(OLED_RES_GPIO_PORT, OLED_RES_GPIO_PIN)

/* DC pin: PB0 */
#define OLED_DC_GPIO_PORT     GPIOB
#define OLED_DC_GPIO_PIN      GPIO_PIN_0
#define OLED_DC_HIGH()        GPIO_Pins_Set(OLED_DC_GPIO_PORT, OLED_DC_GPIO_PIN)
#define OLED_DC_LOW()         GPIO_Pins_Reset(OLED_DC_GPIO_PORT, OLED_DC_GPIO_PIN)

/* CS pin: PB1 */
#define OLED_CS_GPIO_PORT     GPIOB
#define OLED_CS_GPIO_PIN      GPIO_PIN_1
#define OLED_CS_HIGH()        GPIO_Pins_Set(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PIN)
#define OLED_CS_LOW()         GPIO_Pins_Reset(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PIN)

/* OLED SPI control functions */
void OLED_SPI_Reset(void);
void OLED_SPI_SetDC_Cmd(void);
void OLED_SPI_SetDC_Data(void);
void OLED_SPI_CS_Enable(void);
void OLED_SPI_CS_Disable(void);

#ifdef __cplusplus
}
#endif

#endif /* SPI_H */
