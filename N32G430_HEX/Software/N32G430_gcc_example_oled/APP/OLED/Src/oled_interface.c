/**
 * @file oled_interface.c
 * @brief OLED hardware abstraction layer implementation - supports both I2C and SPI
 */

#include "oled_interface.h"
#include "i2c.h"
#include "spi.h"
#include <string.h>

/* I2C protocol bytes */
#define OLED_I2C_CMD_MODE     0x00
#define OLED_I2C_DATA_MODE    0x40

/* Static buffers for I2C (need control byte prefix) */
static uint8_t i2c_cmd_buffer[2];
static uint8_t i2c_data_buffer[129];  /* 1 control byte + 128 data bytes */

/* Static buffer for SPI (no prefix needed) */
static uint8_t spi_data_buffer[128];

/* I2C Interface Functions */

static HAL_StatusTypeDef OLED_I2C_SendCmd(OLED_Interface_t *iface, uint8_t cmd)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)iface->handle;

    if (hi2c == NULL)
    {
        return HAL_ERROR;
    }

    i2c_cmd_buffer[0] = OLED_I2C_CMD_MODE;
    i2c_cmd_buffer[1] = cmd;

    return HAL_I2C_Master_Transmit(hi2c, iface->i2c_addr, i2c_cmd_buffer, 2, 100);
}

static HAL_StatusTypeDef OLED_I2C_SendData(OLED_Interface_t *iface, uint8_t *data, uint16_t len)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)iface->handle;

    if (hi2c == NULL || data == NULL)
    {
        return HAL_ERROR;
    }

    if (len > 128)
    {
        len = 128;
    }

    i2c_data_buffer[0] = OLED_I2C_DATA_MODE;
    memcpy(&i2c_data_buffer[1], data, len);

    return HAL_I2C_Master_Transmit(hi2c, iface->i2c_addr, i2c_data_buffer, len + 1, 200);
}

static HAL_StatusTypeDef OLED_I2C_SendDataDMA(OLED_Interface_t *iface, uint8_t *data, uint16_t len)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)iface->handle;

    if (hi2c == NULL || data == NULL)
    {
        return HAL_ERROR;
    }

    if (len > 128)
    {
        len = 128;
    }

    i2c_data_buffer[0] = OLED_I2C_DATA_MODE;
    memcpy(&i2c_data_buffer[1], data, len);

    return HAL_I2C_Master_Transmit_DMA(hi2c, iface->i2c_addr, i2c_data_buffer, len + 1);
}

static bool OLED_I2C_IsDMABusy(OLED_Interface_t *iface)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)iface->handle;

    if (hi2c == NULL)
    {
        return false;
    }

    return (hi2c->State != HAL_I2C_STATE_READY);
}

/* SPI Interface Functions */

static HAL_StatusTypeDef OLED_SPI_SendCmd(OLED_Interface_t *iface, uint8_t cmd)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;
    HAL_StatusTypeDef status;

    if (hspi == NULL)
    {
        return HAL_ERROR;
    }

    OLED_SPI_CS_Enable();
    OLED_SPI_SetDC_Cmd();

    status = HAL_SPI_Transmit(hspi, &cmd, 1, 100);

    OLED_SPI_CS_Disable();

    return status;
}

static HAL_StatusTypeDef OLED_SPI_SendData(OLED_Interface_t *iface, uint8_t *data, uint16_t len)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;
    HAL_StatusTypeDef status;

    if (hspi == NULL || data == NULL)
    {
        return HAL_ERROR;
    }

    if (len > 128)
    {
        len = 128;
    }

    OLED_SPI_CS_Enable();
    OLED_SPI_SetDC_Data();

    status = HAL_SPI_Transmit(hspi, data, len, 200);

    OLED_SPI_CS_Disable();

    return status;
}

static HAL_StatusTypeDef OLED_SPI_SendDataDMA(OLED_Interface_t *iface, uint8_t *data, uint16_t len)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;

    if (hspi == NULL || data == NULL)
    {
        return HAL_ERROR;
    }

    if (len > 128)
    {
        len = 128;
    }

    /* Copy to static buffer for DMA */
    memcpy(spi_data_buffer, data, len);

    OLED_SPI_CS_Enable();
    OLED_SPI_SetDC_Data();

    /* Note: CS should be disabled in DMA complete callback */
    return HAL_SPI_Transmit_DMA(hspi, spi_data_buffer, len);
}

static bool OLED_SPI_IsDMABusy(OLED_Interface_t *iface)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;

    if (hspi == NULL)
    {
        return false;
    }

    return (hspi->State != HAL_SPI_STATE_READY);
}

static void OLED_SPI_ResetDisplay(OLED_Interface_t *iface)
{
    (void)iface;
    OLED_SPI_Reset();
}

/* Common delay function */
static void OLED_DelayMs(uint32_t delay)
{
    HAL_Delay(delay);
}

/* Public Functions */

/**
 * @brief Initialize I2C interface for OLED
 */
void OLED_Interface_InitI2C(OLED_Interface_t *iface, void *hi2c, uint8_t i2c_addr, bool use_dma)
{
    if (iface == NULL)
    {
        return;
    }

    memset(iface, 0, sizeof(OLED_Interface_t));

    iface->type = OLED_INTERFACE_I2C;
    iface->handle = hi2c;
    iface->i2c_addr = i2c_addr;

    iface->send_cmd = OLED_I2C_SendCmd;
    iface->send_data = OLED_I2C_SendData;

    if (use_dma)
    {
        iface->send_data_dma = OLED_I2C_SendDataDMA;
        iface->is_dma_busy = OLED_I2C_IsDMABusy;
    }
    else
    {
        iface->send_data_dma = NULL;
        iface->is_dma_busy = NULL;
    }

    iface->delay_ms = OLED_DelayMs;
    iface->reset = NULL;  /* I2C OLED typically doesn't have reset pin */
}

/**
 * @brief Initialize SPI interface for OLED
 */
void OLED_Interface_InitSPI(OLED_Interface_t *iface, void *hspi, bool use_dma)
{
    if (iface == NULL)
    {
        return;
    }

    memset(iface, 0, sizeof(OLED_Interface_t));

    iface->type = OLED_INTERFACE_SPI;
    iface->handle = hspi;
    iface->i2c_addr = 0;

    iface->send_cmd = OLED_SPI_SendCmd;
    iface->send_data = OLED_SPI_SendData;

    if (use_dma)
    {
        iface->send_data_dma = OLED_SPI_SendDataDMA;
        iface->is_dma_busy = OLED_SPI_IsDMABusy;
    }
    else
    {
        iface->send_data_dma = NULL;
        iface->is_dma_busy = NULL;
    }

    iface->delay_ms = OLED_DelayMs;
    iface->reset = OLED_SPI_ResetDisplay;
}
