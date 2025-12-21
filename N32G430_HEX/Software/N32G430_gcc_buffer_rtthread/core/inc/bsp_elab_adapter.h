/**
 * @file bsp_elab_adapter.h
 * @brief Elab框架适配层头文件
 * @author BrookDawn
 * @date 2025-12-20
 * @version 0.1
 * @note 为N32G430+RT-Thread适配elab框架
 */

#ifndef __BSP_ELAB_ADAPTER_H__
#define __BSP_ELAB_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=========================== [Includes] ================================*/
#include <stdint.h>

/*=========================== [Defines] =================================*/

/*=========================== [Typedefs] ================================*/

/*=========================== [External Functions] ======================*/
uint32_t elab_time_ms(void);
void elab_usart_adapter_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ELAB_ADAPTER_H__ */
