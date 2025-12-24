#include "dma.h"
#include "n32g430_rcc.h"
#include "misc.h"

DMA_HandleTypeDef hdma_i2c1_tx;

static uint32_t DMA_GetTCFlag(uint8_t channel)
{
    static const uint32_t tc_flags[] = {
        0,
        DMA_CH1_INT_TXC,
        DMA_CH2_INT_TXC,
        DMA_CH3_INT_TXC,
        DMA_CH4_INT_TXC,
        DMA_CH5_INT_TXC,
        DMA_CH6_INT_TXC,
        DMA_CH7_INT_TXC,
        DMA_CH8_INT_TXC};
    if (channel < sizeof(tc_flags) / sizeof(tc_flags[0]))
    {
        return tc_flags[channel];
    }
    return 0;
}

static uint32_t DMA_GetERRFlag(uint8_t channel)
{
    static const uint32_t err_flags[] = {
        0,
        DMA_CH1_INT_ERR,
        DMA_CH2_INT_ERR,
        DMA_CH3_INT_ERR,
        DMA_CH4_INT_ERR,
        DMA_CH5_INT_ERR,
        DMA_CH6_INT_ERR,
        DMA_CH7_INT_ERR,
        DMA_CH8_INT_ERR};
    if (channel < sizeof(err_flags) / sizeof(err_flags[0]))
    {
        return err_flags[channel];
    }
    return 0;
}

void MX_DMA_Init(void)
{
    RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_DMA);

    DMA_Reset(DMA_CH1);
    DMA_Channel_Request_Remap(DMA_CH1, DMA_REMAP_I2C1_TX);
    DMA_Channel_Disable(DMA_CH1);
    DMA_Interrupts_Disable(DMA_CH1, DMA_INT_TXC | DMA_INT_ERR | DMA_INT_HTX);

    hdma_i2c1_tx.Instance = DMA_CH1;
    hdma_i2c1_tx.ChannelIndex = 1;
    hdma_i2c1_tx.RequestID = DMA_REMAP_I2C1_TX;
    hdma_i2c1_tx.State = HAL_DMA_STATE_READY;
    hdma_i2c1_tx.Parent = NULL;
    hdma_i2c1_tx.XferCpltCallback = NULL;
    hdma_i2c1_tx.XferErrorCallback = NULL;

    DMA_Interrupts_Enable(DMA_CH1, DMA_INT_TXC | DMA_INT_ERR);

    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);
}

void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
    uint32_t tc_flag = DMA_GetTCFlag(hdma->ChannelIndex);
    uint32_t err_flag = DMA_GetERRFlag(hdma->ChannelIndex);

    if (tc_flag && DMA_Interrupt_Status_Get(DMA, tc_flag) != RESET)
    {
        DMA_Interrupt_Status_Clear(DMA, tc_flag);
        hdma->State = HAL_DMA_STATE_READY;
        if (hdma->XferCpltCallback)
        {
            hdma->XferCpltCallback(hdma);
        }
    }

    if (err_flag && DMA_Interrupt_Status_Get(DMA, err_flag) != RESET)
    {
        DMA_Interrupt_Status_Clear(DMA, err_flag);
        hdma->State = HAL_DMA_STATE_ERROR;
        if (hdma->XferErrorCallback)
        {
            hdma->XferErrorCallback(hdma);
        }
    }
}
