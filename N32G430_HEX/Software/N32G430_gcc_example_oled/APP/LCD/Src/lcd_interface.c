/**
 * @file lcd_interface.c
 * @brief LCD hardware abstraction layer implementation - SPI interface for ST7735S
 */

#include "lcd_interface.h"
#include "spi.h"
#include <string.h>

/* Static buffer for SPI data */
static uint8_t spi_data_buffer[256];

/* Global LCD interface instance */
static LCD_Interface_t g_lcd_interface;

/* LCD SPI Interface Functions */

static HAL_StatusTypeDef LCD_SPI_SendCmd(LCD_Interface_t *iface, uint8_t cmd)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;
    HAL_StatusTypeDef status;

    if (hspi == NULL)
    {
        return HAL_ERROR;
    }

    LCD_SPI_CS_Enable();
    LCD_SPI_SetDC_Cmd();

    status = HAL_SPI_Transmit(hspi, &cmd, 1, 100);

    LCD_SPI_CS_Disable();

    return status;
}

static HAL_StatusTypeDef LCD_SPI_SendData8(LCD_Interface_t *iface, uint8_t data)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;
    HAL_StatusTypeDef status;

    if (hspi == NULL)
    {
        return HAL_ERROR;
    }

    LCD_SPI_CS_Enable();
    LCD_SPI_SetDC_Data();

    status = HAL_SPI_Transmit(hspi, &data, 1, 100);

    LCD_SPI_CS_Disable();

    return status;
}

static HAL_StatusTypeDef LCD_SPI_SendData16(LCD_Interface_t *iface, uint16_t data)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;
    HAL_StatusTypeDef status;
    uint8_t buf[2];

    if (hspi == NULL)
    {
        return HAL_ERROR;
    }

    buf[0] = (uint8_t)(data >> 8);
    buf[1] = (uint8_t)(data & 0xFF);

    LCD_SPI_CS_Enable();
    LCD_SPI_SetDC_Data();

    status = HAL_SPI_Transmit(hspi, buf, 2, 100);

    LCD_SPI_CS_Disable();

    return status;
}

static HAL_StatusTypeDef LCD_SPI_SendData(LCD_Interface_t *iface, uint8_t *data, uint16_t len)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;
    HAL_StatusTypeDef status;

    if (hspi == NULL || data == NULL)
    {
        return HAL_ERROR;
    }

    LCD_SPI_CS_Enable();
    LCD_SPI_SetDC_Data();

    status = HAL_SPI_Transmit(hspi, data, len, 500);

    LCD_SPI_CS_Disable();

    return status;
}

static HAL_StatusTypeDef LCD_SPI_SendDataDMA(LCD_Interface_t *iface, uint8_t *data, uint16_t len)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;

    if (hspi == NULL || data == NULL)
    {
        return HAL_ERROR;
    }

    if (len > sizeof(spi_data_buffer))
    {
        len = sizeof(spi_data_buffer);
    }

    /* Copy to static buffer for DMA */
    memcpy(spi_data_buffer, data, len);

    LCD_SPI_CS_Enable();
    LCD_SPI_SetDC_Data();

    /* Note: CS should be disabled in DMA complete callback */
    return HAL_SPI_Transmit_DMA(hspi, spi_data_buffer, len);
}

static bool LCD_SPI_IsDMABusy(LCD_Interface_t *iface)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)iface->handle;

    if (hspi == NULL)
    {
        return false;
    }

    return (hspi->State != HAL_SPI_STATE_READY);
}

static void LCD_SPI_ResetDisplay(LCD_Interface_t *iface)
{
    (void)iface;
    LCD_SPI_Reset();
}

static void LCD_SPI_BacklightOn(LCD_Interface_t *iface)
{
    (void)iface;
    LCD_BL_ON();
}

static void LCD_SPI_BacklightOff(LCD_Interface_t *iface)
{
    (void)iface;
    LCD_BL_OFF();
}

/* Common delay function */
static void LCD_DelayMs(uint32_t delay)
{
    HAL_Delay(delay);
}

/* Public Functions */

/**
 * @brief Initialize SPI interface for LCD
 */
void LCD_Interface_InitSPI(LCD_Interface_t *iface, void *hspi, bool use_dma)
{
    if (iface == NULL)
    {
        return;
    }

    memset(iface, 0, sizeof(LCD_Interface_t));

    iface->handle = hspi;

    iface->send_cmd = LCD_SPI_SendCmd;
    iface->send_data8 = LCD_SPI_SendData8;
    iface->send_data16 = LCD_SPI_SendData16;
    iface->send_data = LCD_SPI_SendData;

    if (use_dma)
    {
        iface->send_data_dma = LCD_SPI_SendDataDMA;
        iface->is_dma_busy = LCD_SPI_IsDMABusy;
    }
    else
    {
        iface->send_data_dma = NULL;
        iface->is_dma_busy = NULL;
    }

    iface->delay_ms = LCD_DelayMs;
    iface->reset = LCD_SPI_ResetDisplay;
    iface->backlight_on = LCD_SPI_BacklightOn;
    iface->backlight_off = LCD_SPI_BacklightOff;
}

/**
 * @brief Get the global LCD interface instance
 */
LCD_Interface_t* LCD_Interface_GetInstance(void)
{
    return &g_lcd_interface;
}
