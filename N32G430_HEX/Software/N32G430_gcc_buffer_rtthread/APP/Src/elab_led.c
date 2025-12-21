/**
 * @file elab_led.c
 * @brief Elab LED控制模块实现
 * @author BrookDawn
 * @date 2025-12-20
 * @version 0.1
 * @note 基于elab框架的LED控制模块
 */

/*=========================== [Includes] ================================*/
#include "elab_led.h"
#include "bsp_led.h"
#include "elab_log.h"

/*=========================== [Defines] =================================*/

/*=========================== [Typedefs] ================================*/

/*=========================== [Static Variables] ========================*/

/*=========================== [Static Functions] ========================*/

/*=========================== [External Functions] ======================*/

/**
 * @brief 初始化LED模块
 * @param led_gpio GPIO端口
 * @param led_pin GPIO引脚
 * @param ahb_periph 时钟外设
 */
void elab_led_init(GPIO_Module* led_gpio, uint16_t led_pin, uint32_t ahb_periph)
{
    ELOG_INFO("Initializing LED on GPIO Port 0x%08X, Pin %d", (uint32_t)led_gpio, led_pin);
    LED_Initialize(led_gpio, led_pin, ahb_periph);
    ELOG_INFO("LED initialized successfully");
}

/**
 * @brief 打开LED
 * @param led_gpio GPIO端口
 * @param led_pin GPIO引脚
 */
void elab_led_on(GPIO_Module* led_gpio, uint16_t led_pin)
{
    ELOG_DEBUG("Turning LED ON");
    LED_On(led_gpio, led_pin);
}

/**
 * @brief 关闭LED
 * @param led_gpio GPIO端口
 * @param led_pin GPIO引脚
 */
void elab_led_off(GPIO_Module* led_gpio, uint16_t led_pin)
{
    ELOG_DEBUG("Turning LED OFF");
    LED_Off(led_gpio, led_pin);
}

/**
 * @brief 翻转LED状态
 * @param led_gpio GPIO端口
 * @param led_pin GPIO引脚
 */
void elab_led_toggle(GPIO_Module* led_gpio, uint16_t led_pin)
{
    ELOG_DEBUG("Toggling LED");
    LED_Toggle(led_gpio, led_pin);
}

/**
 * @brief LED闪烁指定次数
 * @param led_gpio GPIO端口
 * @param led_pin GPIO引脚
 * @param times 闪烁次数
 * @param interval_ms 闪烁间隔(毫秒)
 */
void elab_led_blink(GPIO_Module* led_gpio, uint16_t led_pin, uint8_t times, uint16_t interval_ms)
{
    ELOG_INFO("LED blinking %d times with %d ms interval", times, interval_ms);

    for(uint8_t i = 0; i < times; i++)
    {
        LED_On(led_gpio, led_pin);
        rt_thread_mdelay(interval_ms);
        LED_Off(led_gpio, led_pin);
        rt_thread_mdelay(interval_ms);
    }

    ELOG_INFO("LED blink completed");
}
