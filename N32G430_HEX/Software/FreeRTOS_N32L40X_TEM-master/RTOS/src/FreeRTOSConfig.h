/*
 * FreeRTOS V202411.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "n32l40x.h"
#include "core_cm4.h"

/*-----------------------------------------------------------
 * 应用程序特定定义
 *
 * 这些定义应根据您的特定硬件和应用程序要求进行调整。
 *
 * 这些参数在FreeRTOS.org网站上提供的FreeRTOS API文档的"配置"部分中有描述。
 *
 * 参见 http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1		// 使能抢占式调度
#define configUSE_IDLE_HOOK				1		// 使能空闲任务钩子函数
#define configUSE_TICK_HOOK				1		// 使能滴答中断钩子函数
#define configCPU_CLOCK_HZ			( 64000000 )	// 系统时钟频率（64MHz，匹配N32L403KBQ7最大频率）
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )	// 系统节拍频率（1000Hz，即1ms一个节拍）
#define configMAX_PRIORITIES			( 5 )	// 最大优先级数量
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 128 )	// 最小堆栈大小（字）

#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 8 * 1024 ) )	// 总堆大小（8KB，为FreeRTOS任务和队列分配）
#define configMAX_TASK_NAME_LEN			( 10 )		// 任务名称最大长度
#define configUSE_TRACE_FACILITY		1		// 使能跟踪功能
#define configUSE_16_BIT_TICKS			0		// 使用32位时间片计数
#define configIDLE_SHOULD_YIELD			1		// 空闲任务应该让出CPU
#define configUSE_MUTEXES				1		// 使能互斥量
#define configQUEUE_REGISTRY_SIZE		8		// 队列注册表大小
#define configCHECK_FOR_STACK_OVERFLOW	2		// 堆栈溢出检查（方法2）
#define configUSE_RECURSIVE_MUTEXES		1		// 使能递归互斥量
#define configUSE_MALLOC_FAILED_HOOK	1		// 使能内存分配失败钩子函数
#define configUSE_APPLICATION_TASK_TAG	0		// 禁用应用程序任务标签
#define configUSE_COUNTING_SEMAPHORES	1		// 使能计数信号量
#define configGENERATE_RUN_TIME_STATS	0		// 禁用运行时统计

/* 协程定义 */
#define configUSE_CO_ROUTINES 		0		// 禁用协程
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )	// 协程最大优先级数量

/* 软件定时器定义。此示例使用I2C写入LED。由于这需要有限的时间，
   并且因为定时器回调写入LED，定时器任务的优先级保持在最低限度，
   以确保它不会干扰检查自己执行时间的测试任务。 */
#define configUSE_TIMERS				1		// 使能软件定时器
#define configTIMER_TASK_PRIORITY		( 0 )		// 定时器任务优先级
#define configTIMER_QUEUE_LENGTH		5		// 定时器队列长度
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )	// 定时器任务堆栈深度

/* 将以下定义设置为1以包含API函数，设置为0以排除API函数。 */
#define INCLUDE_vTaskPrioritySet		1		// 包含任务优先级设置函数
#define INCLUDE_uxTaskPriorityGet		1		// 包含任务优先级获取函数
#define INCLUDE_vTaskDelete				1		// 包含任务删除函数
#define INCLUDE_vTaskCleanUpResources	1		// 包含任务清理资源函数
#define INCLUDE_vTaskSuspend			1		// 包含任务挂起函数
#define INCLUDE_vTaskDelayUntil			1		// 包含任务延时到指定时间函数
#define INCLUDE_vTaskDelay				1		// 包含任务延时函数

/* Cortex-M特定定义 */
#ifdef __NVIC_PRIO_BITS
	/* 使用CMSIS时将指定__NVIC_PRIO_BITS */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		5        /* 32个优先级 */
#endif

/* 可用于调用"设置优先级"函数的最低中断优先级 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0x1f

/* 任何调用中断安全FreeRTOS API函数的中断服务例程可以使用的最高中断优先级。
   不要从具有比此优先级更高的中断中调用中断安全的FreeRTOS API函数！
   （更高的优先级是更低的数值） */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* 内核端口层本身使用的中断优先级。这些对所有Cortex-M端口都是通用的，
   不依赖于任何特定的库函数。 */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY不能设置为零 !!!!
   参见 http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
	
/* 正常的assert()语义，不依赖于assert.h头文件的提供 */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
	
/* 将FreeRTOS端口中断处理程序映射到其CMSIS标准名称的定义 */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

