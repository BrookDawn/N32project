/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Idle Thread
 */

#include <rtthread.h>
#include <rthw.h>

#ifndef IDLE_THREAD_STACK_SIZE
#define IDLE_THREAD_STACK_SIZE  256
#endif

static struct rt_thread idle;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_thread_stack[IDLE_THREAD_STACK_SIZE];

/* Idle hook list */
#ifndef RT_IDLE_HOOK_LIST_SIZE
#define RT_IDLE_HOOK_LIST_SIZE  4
#endif

static void (*idle_hook_list[RT_IDLE_HOOK_LIST_SIZE])(void);

/**
 * Set idle thread hook
 */
rt_err_t rt_thread_idle_sethook(void (*hook)(void))
{
    rt_size_t i;
    rt_base_t level;

    level = rt_hw_interrupt_disable();

    for (i = 0; i < RT_IDLE_HOOK_LIST_SIZE; i++)
    {
        if (idle_hook_list[i] == RT_NULL)
        {
            idle_hook_list[i] = hook;
            rt_hw_interrupt_enable(level);
            return RT_EOK;
        }
    }

    rt_hw_interrupt_enable(level);
    return -RT_EFULL;
}

/**
 * Delete idle thread hook
 */
rt_err_t rt_thread_idle_delhook(void (*hook)(void))
{
    rt_size_t i;
    rt_base_t level;

    level = rt_hw_interrupt_disable();

    for (i = 0; i < RT_IDLE_HOOK_LIST_SIZE; i++)
    {
        if (idle_hook_list[i] == hook)
        {
            idle_hook_list[i] = RT_NULL;
            rt_hw_interrupt_enable(level);
            return RT_EOK;
        }
    }

    rt_hw_interrupt_enable(level);
    return -RT_ENOSYS;
}

/**
 * Idle thread entry
 */
static void rt_thread_idle_entry(void *parameter)
{
    (void)parameter;

    while (1)
    {
        rt_size_t i;

        /* Execute idle hook */
        for (i = 0; i < RT_IDLE_HOOK_LIST_SIZE; i++)
        {
            if (idle_hook_list[i] != RT_NULL)
            {
                idle_hook_list[i]();
            }
        }

        /* Wait for interrupt */
#ifdef RT_USING_IDLE_HOOK
        __WFI();
#endif
    }
}

/**
 * Initialize idle thread
 */
void rt_thread_idle_init(void)
{
    rt_size_t i;

    /* Initialize hook list */
    for (i = 0; i < RT_IDLE_HOOK_LIST_SIZE; i++)
    {
        idle_hook_list[i] = RT_NULL;
    }

    /* Initialize and start idle thread */
    rt_thread_init(&idle,
                   "tidle",
                   rt_thread_idle_entry,
                   RT_NULL,
                   &rt_thread_stack[0],
                   sizeof(rt_thread_stack),
                   RT_THREAD_PRIORITY_MAX - 1,
                   32);
    rt_thread_startup(&idle);
}
