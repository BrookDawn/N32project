/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Configuration for N32G430
 */

#ifndef __RTCONFIG_H__
#define __RTCONFIG_H__

/* RT-Thread Kernel */
#define RT_NAME_MAX                     8
#define RT_ALIGN_SIZE                   4
#define RT_THREAD_PRIORITY_MAX          32
#define RT_TICK_PER_SECOND              1000

/* Inter-Thread communication */
#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX

/* Memory Management */
/* #define RT_USING_MEMPOOL */
/* #define RT_USING_HEAP */

/* Kernel Console */
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE              128

/* Components Initialization */
/* #define RT_USING_COMPONENTS_INIT */
/* #define RT_USING_USER_MAIN */

/* Timer skip list level */
#define RT_TIMER_SKIP_LIST_LEVEL        1

/* Idle thread stack size */
#define IDLE_THREAD_STACK_SIZE          256

/* System main thread stack size */
#define RT_MAIN_THREAD_STACK_SIZE       512

/* System clock configuration */
#define RT_USING_SYSTICK

/* Hardware timer configuration */
/* Use SysTick as system timer */

/* Debug Configuration */
/* #define RT_DEBUG */
/* #define RT_DEBUG_INIT   0 */
/* #define RT_DEBUG_THREAD 0 */
/* #define RT_DEBUG_SCHEDULER 0 */
/* #define RT_DEBUG_IPC    0 */
/* #define RT_DEBUG_TIMER  0 */

/* Idle hook */
/* #define RT_USING_IDLE_HOOK */

/* CPU Configuration */
#define RT_USING_CPU_FFS
#define RT_CPUS_NR                      1

#endif /* __RTCONFIG_H__ */
