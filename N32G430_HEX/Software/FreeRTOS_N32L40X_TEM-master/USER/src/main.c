/**
 * @file    main.c
 * @brief   FreeRTOS在N32L403KBQ芯片上的主程序文件
 * @author  William
 * @date    2024
 * @version 1.0
 * 
 * @description
 * 本文件实现了基于FreeRTOS的嵌入式应用程序，包括：
 * - 系统初始化
 * - LED任务创建和管理
 * - FreeRTOS钩子函数实现
 * - 硬件抽象层配置
 * 
 * 目标芯片：N32L403KBQ (ARM Cortex-M4F, 128KB Flash, 24KB SRAM)
 * 开发环境：Keil MDK-ARM
 * RTOS版本：FreeRTOS
 */

/**
 * @file main.c
 * @author Nations
 * @version V1.2.2
 *
 * @copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
 * 
 * @note 此文件已针对FreeRTOS进行简化配置：
 *       - 使用PB0引脚控制LED，推挽输出模式
 *       - 基于FreeRTOS任务实现LED周期性闪烁
 *       - 适配N32L403KBQ7芯片，系统时钟64MHz
 */

#include "main.h"
#include "n32l40x.h"

/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

/* LED控制引脚定义 */
#define LED_PORT    GPIOB
#define LED_PIN     GPIO_PIN_0

/* 任务优先级定义 */
#define LED_TASK_PRIORITY   (tskIDLE_PRIORITY + 1)

/* 任务堆栈大小 */
#define LED_TASK_STACK_SIZE 64

/**
 * @brief  配置LED GPIO引脚
 * @param  无
 * @retval 无
 */
static void LED_GPIO_Config(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* 使能GPIOB时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    /* 配置PB0为推挽输出模式 */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
    GPIO_InitStructure.GPIO_Pull = GPIO_No_Pull;
    GPIO_InitPeripheral(LED_PORT, &GPIO_InitStructure);

    /* 初始状态LED熄灭（输出高电平） */
    GPIO_SetBits(LED_PORT, LED_PIN);
}

/**
 * @brief  LED点亮（输出低电平）
 * @param  无
 * @retval 无
 */
static void LED_On(void)
{
    GPIO_ResetBits(LED_PORT, LED_PIN);
}

/**
 * @brief  LED熄灭（输出高电平）
 * @param  无
 * @retval 无
 */
static void LED_Off(void)
{
    GPIO_SetBits(LED_PORT, LED_PIN);
}

/**
 * @brief  LED状态切换
 * @param  无
 * @retval 无
 */
static void LED_Toggle(void)
{
    if (GPIO_ReadOutputDataBit(LED_PORT, LED_PIN))
    {
        LED_On();    // 当前高电平（LED熄灭），切换为低电平（LED点亮）
    }
    else
    {
        LED_Off();   // 当前低电平（LED点亮），切换为高电平（LED熄灭）
    }
}

/**
 * @brief  LED闪烁任务
 * @param  pvParameters: 任务参数（未使用）
 * @retval 无
 */
static void LED_Task(void *pvParameters)
{
    /* 任务主循环 */
    while (1)
    {
        /* LED状态切换 */
        LED_Toggle();
        
        /* 延时500ms */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief  系统时钟配置
 * @param  无
 * @retval 无
 */
static void SystemClock_Config(void)
{
    /* SystemInit()函数已在启动文件startup_n32l40x.s中调用 */
    /* 系统时钟已配置为64MHz */
}

/**
 * @brief  主函数
 * @param  无
 * @retval int
 */
int main(void)
{
    /* 系统时钟配置 */
    SystemClock_Config();
    
    /* LED GPIO配置 */
    LED_GPIO_Config();
    
    /* 创建LED闪烁任务 */
    xTaskCreate(LED_Task,                    /* 任务函数 */
                "LED_Task",                  /* 任务名称 */
                LED_TASK_STACK_SIZE,         /* 任务堆栈大小 */
                NULL,                        /* 任务参数 */
                LED_TASK_PRIORITY,           /* 任务优先级 */
                NULL);                       /* 任务句柄 */
    
    /* 启动FreeRTOS调度器 */
    vTaskStartScheduler();
    
    /* 正常情况下不会执行到这里 */
    while (1)
    {
        /* 如果调度器启动失败，程序会执行到这里 */
    }
}

/**
 * @brief  断言失败处理函数
 * @param  expr: 断言表达式
 * @param  file: 文件名
 * @param  line: 行号
 * @retval 无
 */
#ifdef USE_FULL_ASSERT
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    /* 用户可以在此添加自己的断言失败处理代码 */
    while (1)
    {
        /* 无限循环 */
    }
}
#endif /* USE_FULL_ASSERT */

/**
 * @brief  FreeRTOS内存分配失败钩子函数
 * @param  无
 * @retval 无
 */
void vApplicationMallocFailedHook(void)
{
    /* 内存分配失败时调用此函数 */
    taskDISABLE_INTERRUPTS();
    while (1)
    {
        /* 无限循环 */
    }
}

/**
 * @brief  FreeRTOS堆栈溢出钩子函数
 * @param  xTask: 发生堆栈溢出的任务句柄
 * @param  pcTaskName: 发生堆栈溢出的任务名称
 * @retval 无
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* 堆栈溢出时调用此函数 */
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    while (1)
    {
        /* 无限循环 */
    }
}

/**
 * @brief  FreeRTOS空闲任务钩子函数
 * @param  无
 * @retval 无
 */
void vApplicationIdleHook(void)
{
    /* 空闲任务钩子函数，可在此处添加低功耗代码 */
}

/**
 * @brief  FreeRTOS滴答中断钩子函数
 * @param  无
 * @retval 无
 */
void vApplicationTickHook(void)
{
    /* 滴答中断钩子函数，每个系统节拍都会调用 */
    /* 注意：此函数在中断上下文中执行，应保持简短 */
}

/**
 * @}
 */
