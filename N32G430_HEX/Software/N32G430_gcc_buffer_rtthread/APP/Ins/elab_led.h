/**
 * @file elab_led.h
 * @brief Elab LED控制模块头文件
 * @author BrookDawn
 * @date 2025-12-20
 * @version 0.1
 * @note 基于elab框架的LED控制模块
 */

#ifndef __ELAB_LED_H__
#define __ELAB_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=========================== [Includes] ================================*/
#include "n32g430.h"
#include <rtthread.h>

/*=========================== [Defines] =================================*/

/*=========================== [Typedefs] ================================*/

/*=========================== [External Functions] ======================*/
void elab_led_init(GPIO_Module* led_gpio, uint16_t led_pin, uint32_t ahb_periph);
void elab_led_on(GPIO_Module* led_gpio, uint16_t led_pin);
void elab_led_off(GPIO_Module* led_gpio, uint16_t led_pin);
void elab_led_toggle(GPIO_Module* led_gpio, uint16_t led_pin);
void elab_led_blink(GPIO_Module* led_gpio, uint16_t led_pin, uint8_t times, uint16_t interval_ms);

#ifdef __cplusplus
}
#endif

#endif /* __ELAB_LED_H__ */
