/**
 * @file usart_interface.c
 * @brief USART硬件抽象层接口实现
 * @author BrookDawn
 * @date 2025-12-20
 * @version 0.1
 * @note 提供统一的USART接口，屏蔽底层BSP差异
 */

/*=========================== [Includes] ================================*/

#include "usart_interface.h"

/*=========================== [Defines] =================================*/

#define USART_DEFAULT_BAUDRATE      115200
#define USART_DEFAULT_DATABITS      8
#define USART_DEFAULT_STOPBITS      1
#define USART_DEFAULT_PARITY        0
#define USART_DEFAULT_FLOWCTRL      0

/*=========================== [Macros] ==================================*/

/*=========================== [Typedefs] ================================*/

/*=========================== [Static Variables] ========================*/

/* USART操作函数指针 */
static const usart_ops_t *g_usart_ops = NULL;

/* 初始化标志 */
static bool g_usart_initialized = false;

/*=========================== [Static Functions Declarations] ===========*/

static usart_status_t usart_default_init(const usart_config_t *config);
static usart_status_t usart_default_deinit(void);
static usart_status_t usart_default_send(const uint8_t *data, uint16_t len, uint32_t timeout_ms);
static usart_status_t usart_default_recv(uint8_t *data, uint16_t len, uint32_t timeout_ms);
static int usart_default_putchar(int ch);
static uint16_t usart_default_get_rx_count(void);

/*=========================== [Static Functions] ========================*/

/**
 * @brief 默认初始化实现（调用BSP弱函数）
 */
static usart_status_t usart_default_init(const usart_config_t *config)
{
    return usart_bsp_init(config);
}

/**
 * @brief 默认反初始化实现（调用BSP弱函数）
 */
static usart_status_t usart_default_deinit(void)
{
    return usart_bsp_deinit();
}

/**
 * @brief 默认发送实现（调用BSP弱函数）
 */
static usart_status_t usart_default_send(const uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    return usart_bsp_send(data, len, timeout_ms);
}

/**
 * @brief 默认接收实现（调用BSP弱函数）
 */
static usart_status_t usart_default_recv(uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    return usart_bsp_recv(data, len, timeout_ms);
}

/**
 * @brief 默认putchar实现（调用BSP弱函数）
 */
static int usart_default_putchar(int ch)
{
    return usart_bsp_putchar(ch);
}

/**
 * @brief 默认获取接收数据长度实现（调用BSP弱函数）
 */
static uint16_t usart_default_get_rx_count(void)
{
    return usart_bsp_get_rx_count();
}

/**
 * @brief 默认操作函数表
 */
static const usart_ops_t g_default_ops = {
    .init           = usart_default_init,
    .deinit         = usart_default_deinit,
    .send           = usart_default_send,
    .recv           = usart_default_recv,
    .putchar        = usart_default_putchar,
    .get_rx_count   = usart_default_get_rx_count,
};

/*=========================== [External Functions] ======================*/

/**
 * @brief 注册USART操作函数
 */
usart_status_t usart_register_ops(const usart_ops_t *ops)
{
    if (ops == NULL) {
        return USART_STATUS_INVALID_PARAM;
    }

    /* 检查必须的函数指针 */
    if (ops->init == NULL || ops->send == NULL || ops->putchar == NULL) {
        return USART_STATUS_INVALID_PARAM;
    }

    g_usart_ops = ops;
    return USART_STATUS_OK;
}

/**
 * @brief 初始化USART接口
 */
usart_status_t usart_interface_init(const usart_config_t *config)
{
    usart_status_t status;
    const usart_config_t *cfg = config;
    usart_config_t default_cfg;

    /* 如果未注册ops，使用默认实现 */
    if (g_usart_ops == NULL) {
        g_usart_ops = &g_default_ops;
    }

    /* 如果config为NULL，使用默认配置 */
    if (cfg == NULL) {
        default_cfg = usart_get_default_config();
        cfg = &default_cfg;
    }

    /* 调用BSP初始化 */
    if (g_usart_ops->init != NULL) {
        status = g_usart_ops->init(cfg);
        if (status == USART_STATUS_OK) {
            g_usart_initialized = true;
        }
        return status;
    }

    return USART_STATUS_ERROR;
}

/**
 * @brief 反初始化USART接口
 */
usart_status_t usart_interface_deinit(void)
{
    usart_status_t status;

    if (!g_usart_initialized) {
        return USART_STATUS_ERROR;
    }

    if (g_usart_ops != NULL && g_usart_ops->deinit != NULL) {
        status = g_usart_ops->deinit();
        if (status == USART_STATUS_OK) {
            g_usart_initialized = false;
        }
        return status;
    }

    return USART_STATUS_ERROR;
}

/**
 * @brief 发送数据
 */
usart_status_t usart_interface_send(const uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    if (!g_usart_initialized) {
        return USART_STATUS_ERROR;
    }

    if (data == NULL || len == 0) {
        return USART_STATUS_INVALID_PARAM;
    }

    if (g_usart_ops != NULL && g_usart_ops->send != NULL) {
        return g_usart_ops->send(data, len, timeout_ms);
    }

    return USART_STATUS_ERROR;
}

/**
 * @brief 接收数据
 */
usart_status_t usart_interface_recv(uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    if (!g_usart_initialized) {
        return USART_STATUS_ERROR;
    }

    if (data == NULL || len == 0) {
        return USART_STATUS_INVALID_PARAM;
    }

    if (g_usart_ops != NULL && g_usart_ops->recv != NULL) {
        return g_usart_ops->recv(data, len, timeout_ms);
    }

    return USART_STATUS_ERROR;
}

/**
 * @brief 发送单字节
 */
int usart_interface_putchar(int ch)
{
    if (!g_usart_initialized) {
        return -1;
    }

    if (g_usart_ops != NULL && g_usart_ops->putchar != NULL) {
        return g_usart_ops->putchar(ch);
    }

    return -1;
}

/**
 * @brief 获取接收数据长度
 */
uint16_t usart_interface_get_rx_count(void)
{
    if (!g_usart_initialized) {
        return 0;
    }

    if (g_usart_ops != NULL && g_usart_ops->get_rx_count != NULL) {
        return g_usart_ops->get_rx_count();
    }

    return 0;
}

/**
 * @brief 获取默认配置
 */
usart_config_t usart_get_default_config(void)
{
    usart_config_t config;

    config.baud_rate    = USART_DEFAULT_BAUDRATE;
    config.data_bits    = USART_DEFAULT_DATABITS;
    config.stop_bits    = USART_DEFAULT_STOPBITS;
    config.parity       = USART_DEFAULT_PARITY;
    config.flow_control = USART_DEFAULT_FLOWCTRL;

    return config;
}

/*=========================== [Weak Functions Implementation] ===========*/

/**
 * @brief 弱函数：BSP层初始化
 * @note 应由具体的BSP层实现覆盖此函数
 */
_ELAB_WEAK usart_status_t usart_bsp_init(const usart_config_t *config)
{
    (void)config;
    /* 默认实现：返回错误，需要BSP层实现 */
    return USART_STATUS_ERROR;
}

/**
 * @brief 弱函数：BSP层反初始化
 * @note 应由具体的BSP层实现覆盖此函数
 */
_ELAB_WEAK usart_status_t usart_bsp_deinit(void)
{
    /* 默认实现：返回错误，需要BSP层实现 */
    return USART_STATUS_ERROR;
}

/**
 * @brief 弱函数：BSP层发送
 * @note 应由具体的BSP层实现覆盖此函数
 */
_ELAB_WEAK usart_status_t usart_bsp_send(const uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    (void)data;
    (void)len;
    (void)timeout_ms;
    /* 默认实现：返回错误，需要BSP层实现 */
    return USART_STATUS_ERROR;
}

/**
 * @brief 弱函数：BSP层接收
 * @note 应由具体的BSP层实现覆盖此函数
 */
_ELAB_WEAK usart_status_t usart_bsp_recv(uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    (void)data;
    (void)len;
    (void)timeout_ms;
    /* 默认实现：返回错误，需要BSP层实现 */
    return USART_STATUS_ERROR;
}

/**
 * @brief 弱函数：BSP层发送单字节
 * @note 应由具体的BSP层实现覆盖此函数
 */
_ELAB_WEAK int usart_bsp_putchar(int ch)
{
    (void)ch;
    /* 默认实现：返回错误，需要BSP层实现 */
    return -1;
}

/**
 * @brief 弱函数：BSP层获取接收数据长度
 * @note 应由具体的BSP层实现覆盖此函数
 */
_ELAB_WEAK uint16_t usart_bsp_get_rx_count(void)
{
    /* 默认实现：返回0，需要BSP层实现 */
    return 0;
}
