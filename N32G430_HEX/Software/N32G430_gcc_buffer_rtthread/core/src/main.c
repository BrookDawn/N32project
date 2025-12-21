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

/* RT-Thread include */
#include <rtthread.h>

/*BSP include*/
#include "main.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_elab_adapter.h"
#include "elab_led.h"
#include "elab_log.h"
#include "usart_interface.h"

/* Thread control blocks and stacks */
static struct rt_thread led_thread;
static rt_uint8_t led_thread_stack[512];

static struct rt_thread elog_thread;
static rt_uint8_t elog_thread_stack[1024];

/* LED task thread */
static void led_task_entry(void *parameter)
{
    uint32_t count = 0;

    ELOG_INFO("LED Task started");

    while (1)
    {
        /* Toggle LED */
        elab_led_toggle(LED1_GPIO_PORT, LED1_GPIO_PIN);

        /* Print every 5 cycles */
        if (count % 5 == 0)
        {
            ELOG_DEBUG("LED toggled %lu times", (unsigned long)count);
        }

        count++;

        /* Delay 500ms */
        rt_thread_mdelay(500);
    }
}

/* ELOG test task thread */
static void elog_task_entry(void *parameter)
{
    uint32_t test_count = 0;

    ELOG_INFO("ELOG Test Task started");

    while (1)
    {
        /* Test different log levels */
        test_count++;

        switch (test_count % 5)
        {
            case 0:
                ELOG_DEBUG("This is DEBUG message #%lu", (unsigned long)test_count);
                break;

            case 1:
                ELOG_INFO("This is INFO message #%lu", (unsigned long)test_count);
                break;

            case 2:
                ELOG_WARN("This is WARN message #%lu", (unsigned long)test_count);
                break;

            case 3:
                ELOG_ERROR("This is ERROR message #%lu", (unsigned long)test_count);
                break;

            case 4:
                ELOG_FATAL("This is FATAL message #%lu", (unsigned long)test_count);
                break;
        }

        /* Delay 1000ms */
        rt_thread_mdelay(1000);
    }
}

/**
 * Main function
 * RT-Thread based application with LED and ELOG tasks
 */
int main(void)
{
    rt_err_t result;

    /* 初始化Elab USART适配层，注册ops */
    elab_usart_adapter_init();

    /* 初始化USART interface层 */
    usart_interface_init(NULL);

    /* 初始化LED */
    elab_led_init(LED1_GPIO_PORT, LED1_GPIO_PIN, LED1_GPIO_CLK);

    /* 打印启动横幅 */
    ELOG_INFO("========================================");
    ELOG_INFO("  N32G430 RT-Thread ELOG Test");
    ELOG_INFO("  LED Task + ELOG Task Demo");
    ELOG_INFO("========================================");

    /* Initialize LED task thread */
    result = rt_thread_init(&led_thread,
                           "led_task",
                           led_task_entry,
                           RT_NULL,
                           &led_thread_stack[0],
                           sizeof(led_thread_stack),
                           10,
                           20);

    if (result == RT_EOK)
    {
        ELOG_INFO("LED task thread initialized successfully");
        rt_thread_startup(&led_thread);
    }
    else
    {
        ELOG_ERROR("Failed to initialize LED task thread!");
    }

    /* Initialize ELOG test task thread */
    result = rt_thread_init(&elog_thread,
                           "elog_task",
                           elog_task_entry,
                           RT_NULL,
                           &elog_thread_stack[0],
                           sizeof(elog_thread_stack),
                           11,
                           20);

    if (result == RT_EOK)
    {
        ELOG_INFO("ELOG task thread initialized successfully");
        rt_thread_startup(&elog_thread);
    }
    else
    {
        ELOG_ERROR("Failed to initialize ELOG task thread!");
    }

    ELOG_INFO("System initialization completed, entering RT-Thread scheduler...");

    return 0;
}
