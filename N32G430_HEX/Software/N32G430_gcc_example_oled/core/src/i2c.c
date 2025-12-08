#include "i2c.h"
#include "n32g430_rcc.h"
#include "misc.h"

#define I2C_TIMEOUT_MS 100U

I2C_HandleTypeDef hi2c1;

static HAL_StatusTypeDef I2C_WaitForFlag(I2C_HandleTypeDef *hi2c, uint32_t flag, FlagStatus status, uint32_t timeout);
static void I2C_ClearADDRFlag(I2C_Module *I2Cx);
static void I2C_DMA_TxCplt(DMA_HandleTypeDef *hdma);

void MX_I2C1_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    I2C_InitType I2C_InitStructure;

    RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOB);
    RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO);
    RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_I2C1);

    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current = GPIO_DS_4MA;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF5_I2C1;
    GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);

    I2C_Reset(I2C1);
    I2C_Initializes_Structure(&I2C_InitStructure);
    I2C_InitStructure.ClkSpeed = 1000000; /* 尽量提高传输速率，加快OLED刷新 */
    I2C_InitStructure.AckEnable = I2C_ACKEN;
    I2C_InitStructure.AddrMode = I2C_ADDR_MODE_7BIT;
    I2C_Initializes(I2C1, &I2C_InitStructure);
    I2C_ON(I2C1);

    hi2c1.Instance = I2C1;
    hi2c1.State = HAL_I2C_STATE_READY;
    hi2c1.hdmatx = &hdma_i2c1_tx;
    hi2c1.hdmatx->Parent = &hi2c1;
}

void HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == NULL || hi2c->Instance == NULL)
    {
        return;
    }

    I2C_OFF(hi2c->Instance);
    I2C_Reset(hi2c->Instance);
    hi2c->State = HAL_I2C_STATE_RESET;
}

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
                                          uint16_t DevAddress,
                                          uint8_t *pData,
                                          uint16_t Size,
                                          uint32_t Timeout)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (hi2c == NULL || pData == NULL || Size == 0)
    {
        return HAL_ERROR;
    }
    if (hi2c->State != HAL_I2C_STATE_READY)
    {
        return HAL_BUSY;
    }

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->DevAddress = DevAddress;

    if (I2C_WaitForFlag(hi2c, I2C_FLAG_BUSY, RESET, Timeout) != HAL_OK)
    {
        status = HAL_TIMEOUT;
        goto error_exit;
    }

    I2C_Generate_Start_Enable(hi2c->Instance);
    if (I2C_WaitForFlag(hi2c, I2C_FLAG_STARTBF, SET, Timeout) != HAL_OK)
    {
        status = HAL_TIMEOUT;
        goto error_exit;
    }

    I2C_7bit_Addr_Send(hi2c->Instance, DevAddress, I2C_DIRECTION_SEND);
    if (I2C_WaitForFlag(hi2c, I2C_FLAG_ADDRF, SET, Timeout) != HAL_OK)
    {
        status = HAL_TIMEOUT;
        goto error_exit;
    }

    I2C_ClearADDRFlag(hi2c->Instance);

    for (uint16_t i = 0; i < Size; i++)
    {
        if (I2C_WaitForFlag(hi2c, I2C_FLAG_TXDATE, SET, Timeout) != HAL_OK)
        {
            status = HAL_TIMEOUT;
            goto error_exit;
        }
        I2C_Data_Send(hi2c->Instance, pData[i]);
    }

    if (I2C_WaitForFlag(hi2c, I2C_FLAG_BYTEF, SET, Timeout) != HAL_OK)
    {
        status = HAL_TIMEOUT;
        goto error_exit;
    }

    I2C_Generate_Stop_Enable(hi2c->Instance);
    hi2c->State = HAL_I2C_STATE_READY;
    return HAL_OK;

error_exit:
    I2C_Generate_Stop_Enable(hi2c->Instance);
    hi2c->State = HAL_I2C_STATE_ERROR;
    return status;
}

HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                              uint16_t DevAddress,
                                              uint8_t *pData,
                                              uint16_t Size)
{
    DMA_InitType DMA_InitStructure;

    if (hi2c == NULL || hi2c->hdmatx == NULL || pData == NULL || Size == 0)
    {
        return HAL_ERROR;
    }
    if (hi2c->State != HAL_I2C_STATE_READY)
    {
        return HAL_BUSY;
    }

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->DevAddress = DevAddress;
    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;

    if (I2C_WaitForFlag(hi2c, I2C_FLAG_BUSY, RESET, I2C_TIMEOUT_MS) != HAL_OK)
    {
        hi2c->State = HAL_I2C_STATE_ERROR;
        I2C_Generate_Stop_Enable(hi2c->Instance);
        return HAL_TIMEOUT;
    }

    I2C_Generate_Start_Enable(hi2c->Instance);
    if (I2C_WaitForFlag(hi2c, I2C_FLAG_STARTBF, SET, I2C_TIMEOUT_MS) != HAL_OK)
    {
        hi2c->State = HAL_I2C_STATE_ERROR;
        I2C_Generate_Stop_Enable(hi2c->Instance);
        return HAL_TIMEOUT;
    }

    I2C_7bit_Addr_Send(hi2c->Instance, DevAddress, I2C_DIRECTION_SEND);
    if (I2C_WaitForFlag(hi2c, I2C_FLAG_ADDRF, SET, I2C_TIMEOUT_MS) != HAL_OK)
    {
        hi2c->State = HAL_I2C_STATE_ERROR;
        I2C_Generate_Stop_Enable(hi2c->Instance);
        return HAL_TIMEOUT;
    }
    I2C_ClearADDRFlag(hi2c->Instance);

    DMA_Channel_Disable(hi2c->hdmatx->Instance);
    DMA_Structure_Initializes(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr = (uint32_t)&hi2c->Instance->DAT;
    DMA_InitStructure.MemAddr = (uint32_t)pData;
    DMA_InitStructure.Direction = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize = Size;
    DMA_InitStructure.PeriphInc = DMA_PERIPH_INC_MODE_DISABLE;
    DMA_InitStructure.MemoryInc = DMA_MEM_INC_MODE_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
    DMA_InitStructure.MemDataSize = DMA_MEM_DATA_WIDTH_BYTE;
    DMA_InitStructure.CircularMode = DMA_CIRCULAR_MODE_DISABLE;
    DMA_InitStructure.Priority = DMA_CH_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem = DMA_MEM2MEM_DISABLE;
    DMA_Initializes(hi2c->hdmatx->Instance, &DMA_InitStructure);

    hi2c->hdmatx->Parent = hi2c;
    hi2c->hdmatx->State = HAL_DMA_STATE_BUSY;
    hi2c->hdmatx->XferCpltCallback = I2C_DMA_TxCplt;
    hi2c->hdmatx->XferErrorCallback = NULL;

    DMA_Interrupts_Enable(hi2c->hdmatx->Instance, DMA_INT_TXC | DMA_INT_ERR);
    DMA_Channel_Enable(hi2c->hdmatx->Instance);

    I2C_DMA_Last_Transfer_Enable(hi2c->Instance);
    I2C_DMA_Transfer_Enable(hi2c->Instance);

    return HAL_OK;
}

static HAL_StatusTypeDef I2C_WaitForFlag(I2C_HandleTypeDef *hi2c, uint32_t flag, FlagStatus status, uint32_t timeout)
{
    uint32_t tick_start = HAL_GetTick();
    while (I2C_Flag_Status_Get(hi2c->Instance, flag) != status)
    {
        if ((HAL_GetTick() - tick_start) >= timeout)
        {
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}

static void I2C_ClearADDRFlag(I2C_Module *I2Cx)
{
    __IO uint32_t temp;
    temp = I2Cx->STS1;
    temp = I2Cx->STS2;
    (void)temp;
}

static void I2C_DMA_TxCplt(DMA_HandleTypeDef *hdma)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)hdma->Parent;
    if (hi2c == NULL)
    {
        return;
    }

    I2C_DMA_Transfer_Disable(hi2c->Instance);
    I2C_DMA_Last_Transfer_Disable(hi2c->Instance);
    DMA_Channel_Disable(hdma->Instance);

    (void)I2C_WaitForFlag(hi2c, I2C_FLAG_BYTEF, SET, I2C_TIMEOUT_MS);
    I2C_Generate_Stop_Enable(hi2c->Instance);

    hdma->State = HAL_DMA_STATE_READY;
    hi2c->State = HAL_I2C_STATE_READY;
    HAL_I2C_MasterTxCpltCallback(hi2c);
}

__attribute__((weak)) void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    (void)hi2c;
}
