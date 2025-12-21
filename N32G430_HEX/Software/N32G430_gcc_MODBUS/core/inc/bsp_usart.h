/**
 * @file bsp_usart.h
 * @author brookdawn
 * @brief 串口初始化
 * @version 0.1
 * @date 2025-07-06
 * @copyright Copyright (c) 2025
 */

#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g430.h"
#include <stdbool.h>

/* USART1 引脚定义 */
#define USART1_TX_PIN                GPIO_PIN_9
#define USART1_RX_PIN                GPIO_PIN_10
#define USART1_GPIO_PORT             GPIOA
#define USART1_GPIO_CLK              RCC_AHB_PERIPH_GPIOA
#define USART1_CLK                   RCC_APB2_PERIPH_USART1

/* USART1 配置参数 */
#define USART1_BAUDRATE              115200
#define USART1_WORDLENGTH            USART_WL_8B
#define USART1_STOPBITS              USART_STPB_1
#define USART1_PARITY                USART_PE_NO
#define USART1_HARDWAREFLOWCTL       USART_HFCTRL_NONE
#define USART1_MODE                  (USART_MODE_RX | USART_MODE_TX)

/* 接收缓冲区大小 */
#define USART1_RX_BUFFER_SIZE        256

/* USART1 DMA配置 (用于Modbus从机) */
#define USART1_TX_DMA_CH             DMA_CH3
#define USART1_RX_DMA_CH             DMA_CH4
#define USART1_TX_DMA_IRQn           DMA_Channel3_IRQn
#define USART1_RX_DMA_IRQn           DMA_Channel4_IRQn
#define USART1_RX_DMA_BUFFER_SIZE    256
#define USART1_TX_DMA_BUFFER_SIZE    256

/* USART2 引脚定义 */
#define USART2_TX_PIN                GPIO_PIN_2
#define USART2_RX_PIN                GPIO_PIN_3
#define USART2_GPIO_PORT             GPIOA
#define USART2_GPIO_CLK              RCC_AHB_PERIPH_GPIOA
#define USART2_CLK                   RCC_APB1_PERIPH_USART2
#define USART2_GPIO_AF               GPIO_AF5_USART2

/* USART2 配置参数 */
#define USART2_BAUDRATE              115200
#define USART2_WORDLENGTH            USART_WL_8B
#define USART2_STOPBITS              USART_STPB_1
#define USART2_PARITY                USART_PE_NO
#define USART2_HARDWAREFLOWCTL       USART_HFCTRL_NONE
#define USART2_MODE                  (USART_MODE_RX | USART_MODE_TX)

/* USART2 DMA配置 */
#define USART2_TX_DMA_CH             DMA_CH1
#define USART2_RX_DMA_CH             DMA_CH2
#define USART2_TX_DMA_IRQn           DMA_Channel1_IRQn
#define USART2_RX_DMA_IRQn           DMA_Channel2_IRQn
#define USART2_RX_DMA_BUFFER_SIZE    256
#define USART2_TX_DMA_BUFFER_SIZE    256

/* 函数声明 */
void USART1_Init(void);
void USART1_SendByte(uint8_t data);
void USART1_SendString(const char* str);
void USART1_SendArray(uint8_t* data, uint16_t length);
uint8_t USART1_ReceiveByte(void);
uint16_t USART1_ReceiveString(char* buffer, uint16_t max_length);

void USART1_EnableInterrupt(void);
void USART1_DisableInterrupt(void);

/* USART1 DMA接口 (用于Modbus从机) */
void USART1_DMA_Init(void);
void USART1_SendArray_DMA(const uint8_t* data, uint16_t length);
bool USART1_IsTxBusy(void);
uint16_t USART1_DMA_GetRxData(uint8_t* buffer, uint16_t max_length);
uint16_t USART1_DMA_GetRxCount(void);
void USART1_DMA_FlushRxBuffer(void);
bool USART1_IsFrameReceived(void);
void USART1_ClearFrameReceivedFlag(void);

/* 中断处理函数 */
void USART1_IRQHandler(void);
void DMA_Channel3_IRQHandler(void);

/* USART2 DMA接口 */
void USART2_Init(void);
void USART2_SendByte(uint8_t data);
void USART2_SendArray(const uint8_t* data, uint16_t length);
void USART2_SendArray_DMA(const uint8_t* data, uint16_t length);
bool USART2_IsTxBusy(void);
uint16_t USART2_GetRxData(uint8_t* buffer, uint16_t max_length);
uint16_t USART2_GetRxCount(void);
void USART2_FlushRxBuffer(void);

/* USART2中断处理函数 */
void USART2_IRQHandler(void);
void DMA_Channel1_IRQHandler(void);
void DMA_Channel2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_USART_H__ */
