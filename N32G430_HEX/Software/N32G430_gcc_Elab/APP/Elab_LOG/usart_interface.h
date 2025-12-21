/**
 * @file usart_interface.h
 * @brief USART硬件抽象层接口，适配不同BSP和编译器平台
 * @author BrookDawn
 * @date 2025-12-20
 * @version 0.1
 * @note 提供统一的USART接口，屏蔽底层BSP差异
 */

#ifndef __USART_INTERFACE_H__
#define __USART_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=========================== [Includes] ================================*/

#include "elab_std.h"
#include "elab_def.h"

/*=========================== [Defines] =================================*/

/**
 * @brief USART传输状态枚举
 */
typedef enum {
    USART_STATUS_OK = 0,        /**< 操作成功 */
    USART_STATUS_ERROR,         /**< 操作失败 */
    USART_STATUS_BUSY,          /**< 设备忙 */
    USART_STATUS_TIMEOUT,       /**< 超时 */
    USART_STATUS_INVALID_PARAM  /**< 参数无效 */
} usart_status_t;

/**
 * @brief USART配置参数结构体
 */
typedef struct {
    uint32_t baud_rate;         /**< 波特率 */
    uint8_t  data_bits;         /**< 数据位(8/9) */
    uint8_t  stop_bits;         /**< 停止位(1/2) */
    uint8_t  parity;            /**< 校验位(0:无 1:奇 2:偶) */
    uint8_t  flow_control;      /**< 流控(0:无 1:硬件) */
} usart_config_t;

/**
 * @brief USART操作函数指针结构体（BSP适配层）
 */
typedef struct {
    /**
     * @brief 初始化USART硬件
     * @param config 配置参数指针
     * @return usart_status_t 状态码
     */
    usart_status_t (*init)(const usart_config_t *config);

    /**
     * @brief 反初始化USART硬件
     * @return usart_status_t 状态码
     */
    usart_status_t (*deinit)(void);

    /**
     * @brief 发送数据（阻塞模式）
     * @param data 数据缓冲区指针
     * @param len 数据长度
     * @param timeout_ms 超时时间(ms)，0表示无限等待
     * @return usart_status_t 状态码
     */
    usart_status_t (*send)(const uint8_t *data, uint16_t len, uint32_t timeout_ms);

    /**
     * @brief 接收数据（阻塞模式）
     * @param data 接收缓冲区指针
     * @param len 期望接收长度
     * @param timeout_ms 超时时间(ms)，0表示无限等待
     * @return usart_status_t 状态码
     */
    usart_status_t (*recv)(uint8_t *data, uint16_t len, uint32_t timeout_ms);

    /**
     * @brief 发送单字节（用于printf重定向）
     * @param ch 待发送字符
     * @return int 发送的字符，失败返回-1
     */
    int (*putchar)(int ch);

    /**
     * @brief 获取接收数据长度
     * @return uint16_t 可读字节数
     */
    uint16_t (*get_rx_count)(void);

} usart_ops_t;

/*=========================== [Macros] ==================================*/

/*=========================== [Typedefs] ================================*/

/*=========================== [External Functions] ======================*/

/**
 * @brief 注册USART操作函数（由BSP层调用）
 * @param ops USART操作函数指针结构体
 * @return usart_status_t 状态码
 */
usart_status_t usart_register_ops(const usart_ops_t *ops);

/**
 * @brief 初始化USART接口
 * @param config 配置参数，NULL使用默认配置
 * @return usart_status_t 状态码
 */
usart_status_t usart_interface_init(const usart_config_t *config);

/**
 * @brief 反初始化USART接口
 * @return usart_status_t 状态码
 */
usart_status_t usart_interface_deinit(void);

/**
 * @brief 发送数据
 * @param data 数据缓冲区指针
 * @param len 数据长度
 * @param timeout_ms 超时时间(ms)，0表示无限等待
 * @return usart_status_t 状态码
 */
usart_status_t usart_interface_send(const uint8_t *data, uint16_t len, uint32_t timeout_ms);

/**
 * @brief 接收数据
 * @param data 接收缓冲区指针
 * @param len 期望接收长度
 * @param timeout_ms 超时时间(ms)，0表示无限等待
 * @return usart_status_t 状态码
 */
usart_status_t usart_interface_recv(uint8_t *data, uint16_t len, uint32_t timeout_ms);

/**
 * @brief 发送单字节（用于printf重定向）
 * @param ch 待发送字符
 * @return int 发送的字符，失败返回-1
 */
int usart_interface_putchar(int ch);

/**
 * @brief 获取接收数据长度
 * @return uint16_t 可读字节数
 */
uint16_t usart_interface_get_rx_count(void);

/**
 * @brief 获取默认配置
 * @return usart_config_t 默认配置结构体
 */
usart_config_t usart_get_default_config(void);

/*=========================== [Weak Functions for BSP Override] =========*/

/**
 * @brief 弱函数：BSP层初始化（可被BSP层覆盖）
 * @param config 配置参数
 * @return usart_status_t 状态码
 */
_ELAB_WEAK usart_status_t usart_bsp_init(const usart_config_t *config);

/**
 * @brief 弱函数：BSP层反初始化（可被BSP层覆盖）
 * @return usart_status_t 状态码
 */
_ELAB_WEAK usart_status_t usart_bsp_deinit(void);

/**
 * @brief 弱函数：BSP层发送（可被BSP层覆盖）
 * @param data 数据缓冲区
 * @param len 数据长度
 * @param timeout_ms 超时时间
 * @return usart_status_t 状态码
 */
_ELAB_WEAK usart_status_t usart_bsp_send(const uint8_t *data, uint16_t len, uint32_t timeout_ms);

/**
 * @brief 弱函数：BSP层接收（可被BSP层覆盖）
 * @param data 接收缓冲区
 * @param len 期望接收长度
 * @param timeout_ms 超时时间
 * @return usart_status_t 状态码
 */
_ELAB_WEAK usart_status_t usart_bsp_recv(uint8_t *data, uint16_t len, uint32_t timeout_ms);

/**
 * @brief 弱函数：BSP层发送单字节（可被BSP层覆盖）
 * @param ch 待发送字符
 * @return int 发送的字符
 */
_ELAB_WEAK int usart_bsp_putchar(int ch);

/**
 * @brief 弱函数：BSP层获取接收数据长度（可被BSP层覆盖）
 * @return uint16_t 可读字节数
 */
_ELAB_WEAK uint16_t usart_bsp_get_rx_count(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_INTERFACE_H__ */
