/**
 * @file spi.c
 * @brief SPI HAL-compatible implementation for OLED display with DMA support.
 */

#include "spi.h"
#include "n32g430_rcc.h"
#include "misc.h"

#define SPI_TIMEOUT_MS 100U

SPI_HandleTypeDef hspi1;

static void SPI_DMA_TxCplt(DMA_HandleTypeDef *hdma);

/**
 * @brief Initialize SPI1 for OLED
 * @note  PA5: SCK (AF0), PA7: MOSI (AF0)
 *        PA6: RES (GPIO output)
 *        PB0: DC (GPIO output)
 *        PB1: CS (GPIO output)
 */
void MX_SPI1_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    SPI_InitType SPI_InitStructure;

    /* Enable GPIO and SPI clocks */
    RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOA | RCC_AHB_PERIPH_GPIOB);
    RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO | RCC_APB2_PERIPH_SPI1);

    /* Configure SPI1 pins: PA5 (SCK), PA7 (MOSI) - AF1 for SPI1 */
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin              = GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_InitStructure.GPIO_Alternate   = GPIO_AF1_SPI1;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);

    /* Configure RES pin: PA6 (GPIO output) */
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin              = GPIO_PIN_6;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);

    /* Configure DC pin: PB0 and CS pin: PB1 (GPIO output) */
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin              = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);

    /* Set default pin states */
    OLED_RES_HIGH();   /* RES high (not reset) */
    OLED_DC_HIGH();    /* DC high (data mode) */
    OLED_CS_HIGH();    /* CS high (not selected) */

    /* Reset SPI1 */
    SPI_I2S_Reset(SPI1);

    /* Configure SPI1 */
    SPI_Initializes_Structure(&SPI_InitStructure);
    SPI_InitStructure.DataDirection     = SPI_DIR_SINGLELINE_TX;        /* TX only */
    SPI_InitStructure.SpiMode           = SPI_MODE_MASTER;              /* Master mode */
    SPI_InitStructure.DataLen           = SPI_DATA_SIZE_8BITS;          /* 8-bit data */
    SPI_InitStructure.CLKPOL            = SPI_CLKPOL_HIGH;              /* CPOL=1 (SSD1306 Mode 3) */
    SPI_InitStructure.CLKPHA            = SPI_CLKPHA_SECOND_EDGE;       /* CPHA=1 (SSD1306 Mode 3) */
    SPI_InitStructure.NSS               = SPI_NSS_SOFT;                 /* Software NSS */
    SPI_InitStructure.BaudRatePres      = SPI_BR_PRESCALER_2;           /* APB2/2 = fastest */
    SPI_InitStructure.FirstBit          = SPI_FB_MSB;                   /* MSB first */
    SPI_InitStructure.CRCPoly           = 7;
    SPI_Initializes(SPI1, &SPI_InitStructure);

    /* Set internal NSS high to prevent mode fault */
    SPI_Set_Nss_Level(SPI1, SPI_NSS_HIGH);

    /* Enable SPI1 */
    SPI_ON(SPI1);

    /* Initialize SPI handle */
    hspi1.Instance          = SPI1;
    hspi1.State             = HAL_SPI_STATE_READY;
    hspi1.hdmatx            = &hdma_spi1_tx;
    hspi1.hdmatx->Parent    = &hspi1;
}

/**
 * @brief Deinitialize SPI
 */
void HAL_SPI_DeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi == NULL || hspi->Instance == NULL)
    {
        return;
    }

    SPI_OFF(hspi->Instance);
    SPI_I2S_Reset(hspi->Instance);
    hspi->State = HAL_SPI_STATE_RESET;
}

/**
 * @brief Blocking SPI transmit
 */
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi,
                                   uint8_t *pData,
                                   uint16_t Size,
                                   uint32_t Timeout)
{
    uint32_t tick_start;

    if (hspi == NULL || pData == NULL || Size == 0)
    {
        return HAL_ERROR;
    }
    if (hspi->State != HAL_SPI_STATE_READY)
    {
        return HAL_BUSY;
    }

    hspi->State = HAL_SPI_STATE_BUSY_TX;
    tick_start = HAL_GetTick();

    for (uint16_t i = 0; i < Size; i++)
    {
        /* Wait for TX buffer empty */
        while (SPI_I2S_Flag_Status_Get(hspi->Instance, SPI_I2S_FLAG_TE) == RESET)
        {
            if ((HAL_GetTick() - tick_start) >= Timeout)
            {
                hspi->State = HAL_SPI_STATE_ERROR;
                return HAL_TIMEOUT;
            }
        }
        SPI_I2S_Data_Transmit(hspi->Instance, pData[i]);
    }

    /* Wait for transmission complete (BUSY flag clear) */
    while (SPI_I2S_Flag_Status_Get(hspi->Instance, SPI_I2S_FLAG_BUSY) == SET)
    {
        if ((HAL_GetTick() - tick_start) >= Timeout)
        {
            hspi->State = HAL_SPI_STATE_ERROR;
            return HAL_TIMEOUT;
        }
    }

    hspi->State = HAL_SPI_STATE_READY;
    return HAL_OK;
}

/**
 * @brief DMA SPI transmit
 */
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi,
                                       uint8_t *pData,
                                       uint16_t Size)
{
    DMA_InitType DMA_InitStructure;

    if (hspi == NULL || hspi->hdmatx == NULL || pData == NULL || Size == 0)
    {
        return HAL_ERROR;
    }
    if (hspi->State != HAL_SPI_STATE_READY)
    {
        return HAL_BUSY;
    }

    hspi->State = HAL_SPI_STATE_BUSY_TX;
    hspi->pBuffPtr = pData;
    hspi->XferSize = Size;

    /* Configure DMA */
    DMA_Channel_Disable(hspi->hdmatx->Instance);
    DMA_Structure_Initializes(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr        = (uint32_t)&hspi->Instance->DAT;
    DMA_InitStructure.MemAddr           = (uint32_t)pData;
    DMA_InitStructure.Direction         = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize           = Size;
    DMA_InitStructure.PeriphInc         = DMA_PERIPH_INC_MODE_DISABLE;
    DMA_InitStructure.MemoryInc         = DMA_MEM_INC_MODE_ENABLE;
    DMA_InitStructure.PeriphDataSize    = DMA_PERIPH_DATA_WIDTH_BYTE;
    DMA_InitStructure.MemDataSize       = DMA_MEM_DATA_WIDTH_BYTE;
    DMA_InitStructure.CircularMode      = DMA_CIRCULAR_MODE_DISABLE;
    DMA_InitStructure.Priority          = DMA_CH_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem           = DMA_MEM2MEM_DISABLE;
    DMA_Initializes(hspi->hdmatx->Instance, &DMA_InitStructure);

    hspi->hdmatx->Parent                = hspi;
    hspi->hdmatx->State                 = HAL_DMA_STATE_BUSY;
    hspi->hdmatx->XferCpltCallback      = SPI_DMA_TxCplt;
    hspi->hdmatx->XferErrorCallback     = NULL;

    /* Enable DMA interrupts and channel */
    DMA_Interrupts_Enable(hspi->hdmatx->Instance, DMA_INT_TXC | DMA_INT_ERR);
    DMA_Channel_Enable(hspi->hdmatx->Instance);

    /* Enable SPI DMA TX request */
    SPI_I2S_DMA_Transfer_Enable(hspi->Instance, SPI_I2S_DMA_TX);

    return HAL_OK;
}

/**
 * @brief DMA TX complete callback
 */
static void SPI_DMA_TxCplt(DMA_HandleTypeDef *hdma)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)hdma->Parent;
    if (hspi == NULL)
    {
        return;
    }

    /* Wait for SPI transmission complete */
    uint32_t tick_start = HAL_GetTick();
    while (SPI_I2S_Flag_Status_Get(hspi->Instance, SPI_I2S_FLAG_BUSY) == SET)
    {
        if ((HAL_GetTick() - tick_start) >= SPI_TIMEOUT_MS)
        {
            break;
        }
    }

    /* Disable SPI DMA TX request */
    SPI_I2S_DMA_Transfer_Disable(hspi->Instance, SPI_I2S_DMA_TX);
    DMA_Channel_Disable(hdma->Instance);

    hdma->State = HAL_DMA_STATE_READY;
    hspi->State = HAL_SPI_STATE_READY;
    HAL_SPI_TxCpltCallback(hspi);
}

/**
 * @brief SPI TX complete callback (weak)
 */
__attribute__((weak)) void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    (void)hspi;
}

/* OLED SPI control functions */

/**
 * @brief Reset OLED display
 */
void OLED_SPI_Reset(void)
{
    OLED_RES_LOW();
    HAL_Delay(10);
    OLED_RES_HIGH();
    HAL_Delay(10);
}

/**
 * @brief Set DC pin low for command mode
 */
void OLED_SPI_SetDC_Cmd(void)
{
    OLED_DC_LOW();
}

/**
 * @brief Set DC pin high for data mode
 */
void OLED_SPI_SetDC_Data(void)
{
    OLED_DC_HIGH();
}

/**
 * @brief Enable CS (active low)
 */
void OLED_SPI_CS_Enable(void)
{
    OLED_CS_LOW();
}

/**
 * @brief Disable CS
 */
void OLED_SPI_CS_Disable(void)
{
    OLED_CS_HIGH();
}
