/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Thread Management
 */

#include <rtthread.h>
#include <rthw.h>

extern struct rt_thread *rt_current_thread;
extern rt_list_t rt_thread_priority_table[];
extern rt_uint32_t rt_thread_ready_priority_group;

/* Forward declarations */
void rt_thread_timeout(void *parameter);
rt_err_t rt_thread_sleep(rt_tick_t tick);
rt_tick_t rt_tick_from_millisecond(rt_int32_t ms);

/* Thread exit function */
static void _thread_exit(void)
{
    struct rt_thread *thread;
    register rt_base_t level;

    thread = rt_current_thread;

    level = rt_hw_interrupt_disable();

    /* Remove from ready list */
    rt_schedule_remove_thread(thread);

    /* Change thread state */
    thread->stat = RT_THREAD_CLOSE;

    /* Invoke cleanup if set */
    if (thread->cleanup != RT_NULL)
    {
        thread->cleanup(thread);
    }

    rt_hw_interrupt_enable(level);

    /* Schedule */
    rt_schedule();
}

/**
 * Initialize a thread
 */
rt_err_t rt_thread_init(struct rt_thread *thread,
                        const char       *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size,
                        rt_uint8_t        priority,
                        rt_uint32_t       tick)
{
    /* Initialize thread object */
    rt_object_init((rt_object_t)thread, RT_Object_Class_Thread, name);

    /* Init thread list */
    rt_list_init(&thread->tlist);

    thread->entry = (void *)entry;
    thread->parameter = parameter;

    /* Stack initialization */
    thread->stack_addr = stack_start;
    thread->stack_size = stack_size;

    /* Init stack */
    rt_memset(thread->stack_addr, '#', thread->stack_size);
    thread->sp = (void *)rt_hw_stack_init(thread->entry,
                                          thread->parameter,
                                          (rt_uint8_t *)((char *)thread->stack_addr + thread->stack_size - sizeof(rt_ubase_t)),
                                          (void *)_thread_exit);

    /* Priority initialization */
    thread->current_priority = priority;
    thread->init_priority    = priority;

#if RT_THREAD_PRIORITY_MAX > 32
    thread->number       = priority >> 3;
    thread->number_mask  = 1L << thread->number;
    thread->high_mask    = 1L << (priority & 0x07);
#else
    thread->number_mask  = 1L << priority;
#endif

    /* Tick initialization */
    thread->init_tick      = tick;
    thread->remaining_tick = tick;

    /* Error and stat initialization */
    thread->error = RT_EOK;
    thread->stat  = RT_THREAD_INIT;

    /* Initialize thread timer */
    rt_timer_init(&thread->thread_timer,
                  name,
                  rt_thread_timeout,
                  thread,
                  0,
                  RT_TIMER_FLAG_ONE_SHOT);

    /* Cleanup */
    thread->cleanup = RT_NULL;
    thread->user_data = 0;

    return RT_EOK;
}

/**
 * Start a thread
 */
rt_err_t rt_thread_startup(rt_thread_t thread)
{
    /* Set thread state to suspend first */
    thread->stat = RT_THREAD_SUSPEND;

    /* Resume thread */
    rt_thread_resume(thread);

    if (rt_current_thread != RT_NULL)
    {
        /* Schedule */
        rt_schedule();
    }

    return RT_EOK;
}

/**
 * Get current thread
 */
rt_thread_t rt_thread_self(void)
{
    return rt_current_thread;
}

/**
 * Thread yield
 */
rt_err_t rt_thread_yield(void)
{
    rt_schedule();
    return RT_EOK;
}

/**
 * Thread delay
 */
rt_err_t rt_thread_delay(rt_tick_t tick)
{
    return rt_thread_sleep(tick);
}

/**
 * Thread delay (milliseconds)
 */
rt_err_t rt_thread_mdelay(rt_int32_t ms)
{
    rt_tick_t tick;

    tick = rt_tick_from_millisecond(ms);

    return rt_thread_sleep(tick);
}

/**
 * Thread sleep
 */
rt_err_t rt_thread_sleep(rt_tick_t tick)
{
    register rt_base_t temp;
    struct rt_thread *thread;

    temp = rt_hw_interrupt_disable();

    thread = rt_current_thread;

    /* Suspend current thread */
    rt_thread_suspend(thread);

    /* Reset thread timer */
    rt_timer_control(&thread->thread_timer, RT_TIMER_CTRL_SET_TIME, &tick);
    rt_timer_start(&thread->thread_timer);

    rt_hw_interrupt_enable(temp);

    rt_schedule();

    /* Check error */
    if (thread->error == -RT_ETIMEOUT)
    {
        return -RT_ETIMEOUT;
    }

    return RT_EOK;
}

/**
 * Suspend thread
 */
rt_err_t rt_thread_suspend(rt_thread_t thread)
{
    register rt_base_t temp;

    if (thread == RT_NULL)
    {
        return -RT_ERROR;
    }

    temp = rt_hw_interrupt_disable();

    if (thread->stat != RT_THREAD_READY)
    {
        rt_hw_interrupt_enable(temp);
        return -RT_ERROR;
    }

    /* Change thread state */
    thread->stat = RT_THREAD_SUSPEND;

    /* Stop timer */
    rt_timer_stop(&thread->thread_timer);

    /* Remove from ready list */
    rt_schedule_remove_thread(thread);

    rt_hw_interrupt_enable(temp);

    return RT_EOK;
}

/**
 * Resume thread
 */
rt_err_t rt_thread_resume(rt_thread_t thread)
{
    register rt_base_t temp;

    if (thread == RT_NULL)
    {
        return -RT_ERROR;
    }

    temp = rt_hw_interrupt_disable();

    if (thread->stat != RT_THREAD_SUSPEND)
    {
        rt_hw_interrupt_enable(temp);
        return -RT_ERROR;
    }

    /* Remove thread timer */
    rt_timer_stop(&thread->thread_timer);

    /* Insert to ready list */
    rt_schedule_insert_thread(thread);

    rt_hw_interrupt_enable(temp);

    return RT_EOK;
}

/**
 * Thread timeout handler
 */
void rt_thread_timeout(void *parameter)
{
    struct rt_thread *thread;

    thread = (struct rt_thread *)parameter;

    thread->error = -RT_ETIMEOUT;

    /* Remove from suspend list */
    rt_list_remove(&thread->tlist);

    /* Insert to ready list */
    rt_schedule_insert_thread(thread);

    /* Schedule */
    rt_schedule();
}

/**
 * Convert milliseconds to ticks
 */
rt_tick_t rt_tick_from_millisecond(rt_int32_t ms)
{
    rt_tick_t tick;

    if (ms < 0)
    {
        tick = (rt_tick_t)RT_WAITING_FOREVER;
    }
    else
    {
        tick = (RT_TICK_PER_SECOND * ms + 999) / 1000;
        if (tick == 0) tick = 1;
    }

    return tick;
}
