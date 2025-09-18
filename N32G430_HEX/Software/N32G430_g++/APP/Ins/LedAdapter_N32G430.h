/**
 * @file LedAdapter_N32G430.h
 * @brief N32G430平台LED硬件适配层头文件
 * @author AI Assistant
 * @version v1.0.0
 * @date 2025-09-18
 */

#ifndef __LED_ADAPTER_N32G430_H__
#define __LED_ADAPTER_N32G430_H__

#include "Led.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化N32G430 LED适配器
 * @return 错误代码
 */
LedError_t LedAdapter_N32G430_Init(void);

/**
 * @brief 快速初始化所有可用的LED
 * @return 错误代码
 */
LedError_t LedAdapter_N32G430_InitAllLeds(void);

#ifdef __cplusplus
}
#endif

#endif /* __LED_ADAPTER_N32G430_H__ */
