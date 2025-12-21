/**
 * @file bsp_elab_adapter.c
 * @brief Elab框架适配层实现
 * @author BrookDawn
 * @date 2025-12-20
 * @version 0.1
 * @note 为N32G430+RT-Thread适配elab框架
 */

/*=========================== [Includes] ================================*/
#include "bsp_elab_adapter.h"
#include "usart_interface.h"
#include "bsp_usart.h"
#include <rtthread.h>

/*=========================== [Defines] =================================*/

/*=========================== [Typedefs] ================================*/

/*=========================== [Static Variables] ========================*/

/*=========================== [Static Functions Declarations] ===========*/
static usart_status_t elab_usart_init(const usart_config_t *config);
static usart_status_t elab_usart_deinit(void);
static usart_status_t elab_usart_send(const uint8_t *data, uint16_t len, uint32_t timeout_ms);
static usart_status_t elab_usart_recv(uint8_t *data, uint16_t len, uint32_t timeout_ms);
static int elab_usart_putchar(int ch);
static uint16_t elab_usart_get_rx_count(void);

/*=========================== [Static Variables] ========================*/

/* USART操作函数表 */
static const usart_ops_t g_elab_usart_ops = {
    .init = elab_usart_init,
    .deinit = elab_usart_deinit,
    .send = elab_usart_send,
    .recv = elab_usart_recv,
    .putchar = elab_usart_putchar,
    .get_rx_count = elab_usart_get_rx_count,
};

/*=========================== [Static Functions] ========================*/

/**
 * @brief Elab USART初始化适配 - 对接N32G430的USART1
 */
static usart_status_t elab_usart_init(const usart_config_t *config)
{
    (void)config;  /* 使用固定配置，忽略传入参数 */
    USART1_Init();
    return USART_STATUS_OK;
}

/**
 * @brief Elab USART反初始化适配
 */
static usart_status_t elab_usart_deinit(void)
{
    /* N32G430不需要特殊的反初始化 */
    return USART_STATUS_OK;
}

/**
 * @brief Elab USART发送适配
 */
static usart_status_t elab_usart_send(const uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    (void)timeout_ms;  /* 当前实现为阻塞发送，忽略超时 */
    USART1_SendArray((uint8_t *)data, len);
    return USART_STATUS_OK;
}

/**
 * @brief Elab USART接收适配
 */
static usart_status_t elab_usart_recv(uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    (void)timeout_ms;  /* 简化实现，忽略超时参数 */
    uint16_t received = USART1_ReceiveString((char *)data, len);
    return (received > 0) ? USART_STATUS_OK : USART_STATUS_ERROR;
}

/**
 * @brief Elab USART单字节发送适配
 */
static int elab_usart_putchar(int ch)
{
    USART1_SendByte((uint8_t)ch);
    return ch;
}

/**
 * @brief Elab USART获取接收计数适配
 */
static uint16_t elab_usart_get_rx_count(void)
{
    /* N32G430的bsp_usart中没有直接提供接口，返回0 */
    return 0;
}

/*=========================== [External Functions] ======================*/

/**
 * @brief 获取系统时间（毫秒）- 重写elab_common.c中的弱函数
 * @return 系统运行时间，单位：毫秒
 */
uint32_t elab_time_ms(void)
{
    return (uint32_t)rt_tick_get();  /* RT-Thread的tick转换为毫秒 */
}

/**
 * @brief 初始化Elab USART适配层
 * @note 注册USART操作函数到usart_interface层
 */
void elab_usart_adapter_init(void)
{
    /* 注册USART操作函数 */
    usart_register_ops(&g_elab_usart_ops);
}
