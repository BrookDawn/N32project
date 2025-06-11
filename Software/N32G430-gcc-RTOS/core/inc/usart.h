/**
 * @file usart1.h
 * @brief USART1 Serial Communication Interface
 * @version v1.0.0
 * @author N32G430 Project
 */

#ifndef __USART_H__
#define __USART_H__

#include "n32g430.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* USART1 Configuration */
#define USART1_BAUDRATE         115200
#define USART1_TX_BUFFER_SIZE   256
#define USART1_RX_BUFFER_SIZE   256

/* USART1 GPIO Configuration */
#define USART1_TX_GPIO_PORT     GPIOA
#define USART1_TX_GPIO_PIN      GPIO_PIN_9
#define USART1_TX_GPIO_AF       GPIO_AF1_USART1

#define USART1_RX_GPIO_PORT     GPIOA
#define USART1_RX_GPIO_PIN      GPIO_PIN_10
#define USART1_RX_GPIO_AF       GPIO_AF1_USART1

/* Function Prototypes */
void USART1_Init(uint32_t baudrate);
void USART1_DeInit(void);

/* Basic Send/Receive Functions */
void USART1_Send_Char(char ch);
void USART1_Send_String(const char* str);
void USART1_Send_Data(const uint8_t* data, uint16_t length);
char USART1_Receive_Char(void);
uint16_t USART1_Receive_String(char* buffer, uint16_t max_length, uint32_t timeout_ms);

/* Advanced Functions */
void USART1_Printf(const char* format, ...);
void USART1_Send_Hex(uint32_t value);
void USART1_Send_Dec(uint32_t value);
void USART1_Send_Float(float value, uint8_t decimals);

/* Status Functions */
uint8_t USART1_Is_TX_Ready(void);
uint8_t USART1_Is_RX_Ready(void);
uint8_t USART1_Is_Initialized(void);

/* Interrupt Functions */
void USART1_IT_Enable(void);
void USART1_IT_Disable(void);
uint16_t USART1_Get_RX_Buffer_Count(void);
uint16_t USART1_Read_RX_Buffer(char* buffer, uint16_t max_length);

/* Test Functions */
void USART1_Test_Communication(void);
void USART1_Test_Echo(void);
void USART1_Test_Send_Info(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART1_H__ */ 