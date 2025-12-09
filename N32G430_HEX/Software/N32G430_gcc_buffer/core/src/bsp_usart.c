/**
 * @file bsp_usart.c
 * @author brookdawn
 * @brief 串口初始化
 * @version 0.1
 * @date 2025-07-06
 * @copyright Copyright (c) 2025
 */

#include "n32g430.h"
#include "n32g430_dma.h"
#include "bsp_usart.h"
#include "bsp_delay.h"

#include <stddef.h>
#include <string.h>

/* 接收缓冲区 */
static uint8_t usart1_rx_buffer[USART1_RX_BUFFER_SIZE];
static uint16_t rx_head = 0;
static uint16_t rx_tail = 0;
static uint16_t rx_count = 0;

/* USART2 DMA缓冲区和状态 */
static uint8_t usart2_tx_dma_buffer[USART2_TX_DMA_BUFFER_SIZE];
static uint8_t usart2_rx_dma_buffer[USART2_RX_DMA_BUFFER_SIZE];
static volatile uint16_t usart2_rx_write_index = 0;
static volatile uint16_t usart2_rx_read_index = 0;
static volatile bool usart2_tx_busy = false;
static volatile uint16_t usart2_last_dma_counter = 0;

/**
 *\*\name   USART1_Init.
 *\*\fun    Initialize USART1.
 *\*\param  none.
 *\*\return none.
**/
void USART1_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* 使能GPIOA和USART1时钟 */
    RCC_AHB_Peripheral_Clock_Enable(USART1_GPIO_CLK);
    RCC_APB2_Peripheral_Clock_Enable(USART1_CLK);

    /* 配置USART1 TX引脚 (PA9) */
    GPIO_InitStructure.Pin              = USART1_TX_PIN;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_InitStructure.GPIO_Alternate   = GPIO_AF5_USART1;
    GPIO_Peripheral_Initialize(USART1_GPIO_PORT, &GPIO_InitStructure);

    /* 配置USART1 RX引脚 (PA10) */
    GPIO_InitStructure.Pin              = USART1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_InitStructure.GPIO_Alternate   = GPIO_AF5_USART1;
    GPIO_Peripheral_Initialize(USART1_GPIO_PORT, &GPIO_InitStructure);

    /* USART1配置 */
    USART_InitStructure.BaudRate        = USART1_BAUDRATE;
    USART_InitStructure.WordLength      = USART1_WORDLENGTH;
    USART_InitStructure.StopBits        = USART1_STOPBITS;
    USART_InitStructure.Parity          = USART1_PARITY;
    USART_InitStructure.HardwareFlowControl = USART1_HARDWAREFLOWCTL;
    USART_InitStructure.Mode            = USART1_MODE;
    USART_Initializes(USART1, &USART_InitStructure);

    /* 使能USART1接收中断 */
    USART_Interrput_Enable(USART1, USART_INT_RXDNE);

    /* 配置USART1中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);

    /* 使能USART1 */
    USART_Enable(USART1);

    /* 初始化接收缓冲区 */
    rx_head = 0;
    rx_tail = 0;
    rx_count = 0;
}

/**
 *\*\name   USART1_SendByte.
 *\*\fun    Send one byte via USART1.
 *\*\param  data: byte to send.
 *\*\return none.
**/
void USART1_SendByte(uint8_t data)
{
    /* 等待发送数据寄存器为空 */
    while(USART_Flag_Status_Get(USART1, USART_FLAG_TXDE) == RESET);

    /* 发送数据 */
    USART_Data_Send(USART1, data);
}

/**
 *\*\name   USART1_SendString.
 *\*\fun    Send string via USART1.
 *\*\param  str: string to send.
 *\*\return none.
**/
void USART1_SendString(const char* str)
{
    while(*str)
    {
        USART1_SendByte(*str++);
    }
}

/**
 *\*\name   USART1_SendArray.
 *\*\fun    Send array via USART1.
 *\*\param  data: array to send.
 *\*\param  length: length of array.
 *\*\return none.
**/
void USART1_SendArray(uint8_t* data, uint16_t length)
{
    uint16_t i;
    for(i = 0; i < length; i++)
    {
        USART1_SendByte(data[i]);
    }
}

/**
 *\*\name   USART1_ReceiveByte.
 *\*\fun    Receive one byte from USART1.
 *\*\param  none.
 *\*\return received byte.
**/
uint8_t USART1_ReceiveByte(void)
{
    /* 等待接收数据 */
    while(USART_Flag_Status_Get(USART1, USART_FLAG_RXDNE) == RESET);

    /* 返回接收到的数据 */
    return (uint8_t)USART_Data_Receive(USART1);
}

/**
 *\*\name   USART1_ReceiveString.
 *\*\fun    Receive string from USART1 buffer.
 *\*\param  buffer: buffer to store received string.
 *\*\param  max_length: maximum length of buffer.
 *\*\return actual length of received string.
**/
uint16_t USART1_ReceiveString(char* buffer, uint16_t max_length)
{
    uint16_t length = 0;

    /* 从缓冲区读取数据 */
    while(rx_count > 0 && length < max_length - 1)
    {
        buffer[length] = usart1_rx_buffer[rx_tail];
        rx_tail = (rx_tail + 1) % USART1_RX_BUFFER_SIZE;
        rx_count--;
        length++;
    }

    buffer[length] = '\0';
    return length;
}

/**
 *\*\name   USART1_EnableInterrupt.
 *\*\fun    Enable USART1 interrupt.
 *\*\param  none.
 *\*\return none.
**/
void USART1_EnableInterrupt(void)
{
    USART_Interrput_Enable(USART1, USART_INT_RXDNE);
}

/**
 *\*\name   USART1_DisableInterrupt.
 *\*\fun    Disable USART1 interrupt.
 *\*\param  none.
 *\*\return none.
**/
void USART1_DisableInterrupt(void)
{
    USART_Interrput_Disable(USART1, USART_INT_RXDNE);
}

/**
 *\*\name   USART1_IRQHandler.
 *\*\fun    USART1 interrupt handler.
 *\*\param  none.
 *\*\return none.
**/
void USART1_IRQHandler(void)
{
    if(USART_Interrupt_Status_Get(USART1, USART_INT_RXDNE) != RESET)
    {
        /* 清除中断标志 */
        USART_Interrupt_Status_Clear(USART1, USART_INT_RXDNE);

        /* 检查缓冲区是否已满 */
        if(rx_count < USART1_RX_BUFFER_SIZE)
        {
            /* 读取接收到的数据并存储到缓冲区 */
            usart1_rx_buffer[rx_head] = (uint8_t)USART_Data_Receive(USART1);
            rx_head = (rx_head + 1) % USART1_RX_BUFFER_SIZE;
            rx_count++;
        }
        else
        {
            /* 缓冲区已满，丢弃数据 */
            USART_Data_Receive(USART1);
        }
    }
}

/**
 *\*\name   USART2_Init.
 *\*\fun    Initialize USART2 with DMA for Modbus master.
 *\*\param  none.
 *\*\return none.
**/
void USART2_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;
    DMA_InitType DMA_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* 使能时钟 */
    RCC_AHB_Peripheral_Clock_Enable(USART2_GPIO_CLK | RCC_AHB_PERIPH_DMA);
    RCC_APB1_Peripheral_Clock_Enable(USART2_CLK);

    /* TX: PA2 */
    GPIO_InitStructure.Pin              = USART2_TX_PIN;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_InitStructure.GPIO_Alternate   = USART2_GPIO_AF;
    GPIO_Peripheral_Initialize(USART2_GPIO_PORT, &GPIO_InitStructure);

    /* RX: PA3 */
    GPIO_InitStructure.Pin              = USART2_RX_PIN;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_InitStructure.GPIO_Alternate   = USART2_GPIO_AF;
    GPIO_Peripheral_Initialize(USART2_GPIO_PORT, &GPIO_InitStructure);

    /* USART2配置 */
    USART_InitStructure.BaudRate        = USART2_BAUDRATE;
    USART_InitStructure.WordLength      = USART2_WORDLENGTH;
    USART_InitStructure.StopBits        = USART2_STOPBITS;
    USART_InitStructure.Parity          = USART2_PARITY;
    USART_InitStructure.HardwareFlowControl = USART2_HARDWAREFLOWCTL;
    USART_InitStructure.Mode            = USART2_MODE;
    USART_Initializes(USART2, &USART_InitStructure);

    /* 配置DMA通道1用于USART2_TX */
    DMA_Reset(USART2_TX_DMA_CH);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&USART2->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)usart2_tx_dma_buffer;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize        = 0;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_MODE_DISABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_MODE_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_BYTE;
    DMA_InitStructure.CircularMode   = DMA_CIRCULAR_MODE_DISABLE;
    DMA_InitStructure.Priority       = DMA_CH_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_MEM2MEM_DISABLE;
    DMA_Initializes(USART2_TX_DMA_CH, &DMA_InitStructure);
    DMA_Channel_Request_Remap(USART2_TX_DMA_CH, DMA_REMAP_USART2_TX);
    DMA_Interrupts_Enable(USART2_TX_DMA_CH, DMA_INT_TXC);

    /* 配置DMA通道2用于USART2_RX (循环模式) */
    DMA_Reset(USART2_RX_DMA_CH);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&USART2->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)usart2_rx_dma_buffer;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = USART2_RX_DMA_BUFFER_SIZE;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_MODE_DISABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_MODE_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_BYTE;
    DMA_InitStructure.CircularMode   = DMA_CIRCULAR_MODE_ENABLE;
    DMA_InitStructure.Priority       = DMA_CH_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_MEM2MEM_DISABLE;
    DMA_Initializes(USART2_RX_DMA_CH, &DMA_InitStructure);
    DMA_Channel_Request_Remap(USART2_RX_DMA_CH, DMA_REMAP_USART2_RX);
    DMA_Channel_Enable(USART2_RX_DMA_CH);

    /* 配置DMA TX中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_TX_DMA_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);

    /* 配置USART2空闲中断用于检测接收完成 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);

    /* 使能USART2空闲中断 */
    USART_Interrput_Enable(USART2, USART_INT_IDLEF);

    /* 使能USART2的DMA */
    USART_DMA_Transfer_Enable(USART2, USART_DMAREQ_TX | USART_DMAREQ_RX);

    /* 使能USART2 */
    USART_Enable(USART2);

    /* 初始化状态变量 */
    usart2_rx_write_index = 0;
    usart2_rx_read_index = 0;
    usart2_tx_busy = false;
    usart2_last_dma_counter = USART2_RX_DMA_BUFFER_SIZE;
}

void USART2_SendByte(uint8_t data)
{
    while(USART_Flag_Status_Get(USART2, USART_FLAG_TXDE) == RESET)
    {
    }
    USART_Data_Send(USART2, data);
}

void USART2_SendArray(const uint8_t* data, uint16_t length)
{
    for(uint16_t i = 0; i < length; i++)
    {
        USART2_SendByte(data[i]);
    }
    /* 等待发送完成 */
    while(USART_Flag_Status_Get(USART2, USART_FLAG_TXC) == RESET)
    {
    }
}

/**
 *\*\name   USART2_SendArray_DMA.
 *\*\fun    Send array via USART2 using DMA.
 *\*\param  data: array to send.
 *\*\param  length: length of array.
 *\*\return none.
**/
void USART2_SendArray_DMA(const uint8_t* data, uint16_t length)
{
    if(data == NULL || length == 0)
    {
        return;
    }

    /* 等待上次DMA发送完成 */
    while(usart2_tx_busy)
    {
    }

    /* 限制长度 */
    if(length > USART2_TX_DMA_BUFFER_SIZE)
    {
        length = USART2_TX_DMA_BUFFER_SIZE;
    }

    /* 复制数据到DMA缓冲区 */
    memcpy(usart2_tx_dma_buffer, data, length);

    /* 标记发送忙 */
    usart2_tx_busy = true;

    /* 配置DMA传输 */
    DMA_Channel_Disable(USART2_TX_DMA_CH);
    DMA_Current_Data_Transfer_Number_Set(USART2_TX_DMA_CH, length);
    DMA_Channel_Enable(USART2_TX_DMA_CH);
}

/**
 *\*\name   USART2_IsTxBusy.
 *\*\fun    Check if USART2 TX DMA is busy.
 *\*\param  none.
 *\*\return true if busy, false otherwise.
**/
bool USART2_IsTxBusy(void)
{
    return usart2_tx_busy;
}

/**
 *\*\name   USART2_GetRxCount.
 *\*\fun    Get number of received bytes in buffer.
 *\*\param  none.
 *\*\return number of bytes available.
**/
uint16_t USART2_GetRxCount(void)
{
    uint16_t current_counter = DMA_Current_Data_Transfer_Number_Get(USART2_RX_DMA_CH);
    uint16_t write_index = USART2_RX_DMA_BUFFER_SIZE - current_counter;

    if(write_index >= usart2_rx_read_index)
    {
        return write_index - usart2_rx_read_index;
    }
    else
    {
        return USART2_RX_DMA_BUFFER_SIZE - usart2_rx_read_index + write_index;
    }
}

/**
 *\*\name   USART2_GetRxData.
 *\*\fun    Get received data from buffer.
 *\*\param  buffer: buffer to store received data.
 *\*\param  max_length: maximum length of buffer.
 *\*\return actual length of received data.
**/
uint16_t USART2_GetRxData(uint8_t* buffer, uint16_t max_length)
{
    if(buffer == NULL || max_length == 0)
    {
        return 0;
    }

    uint16_t current_counter = DMA_Current_Data_Transfer_Number_Get(USART2_RX_DMA_CH);
    uint16_t write_index = USART2_RX_DMA_BUFFER_SIZE - current_counter;
    uint16_t count = 0;

    while(usart2_rx_read_index != write_index && count < max_length)
    {
        buffer[count++] = usart2_rx_dma_buffer[usart2_rx_read_index];
        usart2_rx_read_index = (usart2_rx_read_index + 1) % USART2_RX_DMA_BUFFER_SIZE;
    }

    return count;
}

void USART2_FlushRxBuffer(void)
{
    uint16_t current_counter = DMA_Current_Data_Transfer_Number_Get(USART2_RX_DMA_CH);
    usart2_rx_read_index = USART2_RX_DMA_BUFFER_SIZE - current_counter;
}

/**
 *\*\name   USART2_IRQHandler.
 *\*\fun    USART2 interrupt handler for IDLE detection.
 *\*\param  none.
 *\*\return none.
**/
void USART2_IRQHandler(void)
{
    if(USART_Interrupt_Status_Get(USART2, USART_INT_IDLEF) != RESET)
    {
        /* 清除空闲中断标志：先读STS再读DAT */
        (void)USART2->STS;
        (void)USART2->DAT;

        /* 更新写索引 */
        uint16_t current_counter = DMA_Current_Data_Transfer_Number_Get(USART2_RX_DMA_CH);
        usart2_rx_write_index = USART2_RX_DMA_BUFFER_SIZE - current_counter;
    }
}

/**
 *\*\name   DMA_Channel1_IRQHandler.
 *\*\fun    DMA Channel1 interrupt handler for USART2 TX complete.
 *\*\param  none.
 *\*\return none.
**/
void DMA_Channel1_IRQHandler(void)
{
    if(DMA_Interrupt_Status_Get(DMA, DMA_CH1_INT_TXC) != RESET)
    {
        DMA_Interrupt_Status_Clear(DMA, DMA_CH1_INT_TXC);
        DMA_Channel_Disable(USART2_TX_DMA_CH);
        usart2_tx_busy = false;
    }
}
