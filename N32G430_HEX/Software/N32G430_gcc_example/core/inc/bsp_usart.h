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

/* 函数声明 */
void USART1_Init(void);
void USART1_SendByte(uint8_t data);
void USART1_SendString(const char* str);
void USART1_SendArray(uint8_t* data, uint16_t length);
uint8_t USART1_ReceiveByte(void);
uint16_t USART1_ReceiveString(char* buffer, uint16_t max_length);

void USART1_EnableInterrupt(void);
void USART1_DisableInterrupt(void);

/* 中断处理函数 */
void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_USART_H__ */
