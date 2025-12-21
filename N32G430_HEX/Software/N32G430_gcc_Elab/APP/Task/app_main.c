/**
 * @file app_main.c
 * @brief Standard definitions and includes for the APP Task module.
 * @author BrookDawn
 * @date 2025-12-19
 * @version 0.1
 */

/*=========================== [Includes] ================================*/
#include "app_main.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "elab_log.h"
#include "usart_interface.h"

/*=========================== [Defines] =================================*/

/*=========================== [Macros] ==================================*/

/*=========================== [Typedefs] ================================*/

/*=========================== [Static Functions] ========================*/





static void LED_Task(void *argument);
static void LOG_Task(void *argument);

/*=========================== [Private Variables] ======================*/
static osThreadId_t ledTaskHandle;
static const osThreadAttr_t ledTask_attributes = {
  .name = "ledTask",
  .stack_size = 512,
  .priority = (osPriority_t) osPriorityNormal,
};

static osThreadId_t logTaskHandle;
static const osThreadAttr_t logTask_attributes = {
  .name = "logTask",
  .stack_size = 1024,
  .priority = (osPriority_t) osPriorityNormal,
};

/*=========================== [External Functions] ======================*/

void app_main(void)
{
    // Initialize USART interface
    usart_interface_init(NULL);  // Use default config

    // Create LED Task
    ledTaskHandle = osThreadNew(LED_Task, NULL, &ledTask_attributes);

    // Create LOG Task
    logTaskHandle = osThreadNew(LOG_Task, NULL, &logTask_attributes);
}

/*=========================== [Task Implementations] ====================*/

/**
 * @brief LED Task: Toggle PC13 every 500ms
 */
static void LED_Task(void *argument)
{
    /* 初始化LED1 (PA1) */
    LED_Initialize(LED1_GPIO_PORT, LED1_GPIO_PIN, LED1_GPIO_CLK);
    ELOG_INFO("LED Task Started");
    for(;;)
    {
        LED_Toggle(LED1_GPIO_PORT, LED1_GPIO_PIN);
        ELOG_DEBUG("LED Toggled");
        osDelay(500);
    }
}

/**
 * @brief LOG Task: Print message every 1000ms
 */
static void LOG_Task(void *argument)
{
    uint32_t count = 0;

    /* 启动时打印一次欢迎信息 */
    ELOG_INFO("N32G430 FreeRTOS Log System Started");

    for(;;)
    {
        /* 日志异步刷新 */
        log_async_flush();

        // 每1000ms打印一次
        if (count % 1000 == 0) {
            ELOG_INFO("System Heartbeat - Time: %lu ms", elab_time_ms());
        }

        count++;
        osDelay(1); // 提高刷新频率，1ms刷新一次缓冲区
    }
}
