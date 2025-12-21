/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
*\*\file main.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/

/*BSP include*/
#include "main.h"
#include "bsp_usart.h"
#include "bsp_delay.h"

/* std include */
#include <stdio.h>
#include <string.h>

/* Elab include */
#include "elab_log.h"
#include "usart_interface.h"

/* RTOS include */
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "app_main.h"

/*RTT include*/
#include "SEGGER_RTT.h"

static __IO uint32_t uwTick = 0;

/**
 * @brief 实现Elab框架要求的时间戳获取函数
 */
uint32_t elab_time_ms(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        return xTaskGetTickCount();
    }
    return uwTick;
}

/**
 * @brief SysTick自增 (由中断调用)
 */
void Tick_Increment(void)
{
    uwTick++;
}

/**
 *\*\name   main.
 *\*\fun    main function.
 *\*\param  none.
 *\*\return none.
**/
int main(void)
{
    /* RTT初始化和banner */
    SEGGER_RTT_Init();

    /* 初始化OS内核 */
    osKernelInitialize();

    /* 初始化FreeRTOS默认任务和信号量等 (STM32CubeMX生成的逻辑) */
    // MX_FREERTOS_Init();

    /* 初始化应用程序任务 */
    app_main();

    /* 启动调度器 */
    osKernelStart();

    /* 正常情况下不会执行到这里 */
    while(1)
    {
    }
}


