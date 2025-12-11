/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Kernel Scheduler
 */

#include <rtthread.h>
#include <rthw.h>

/* Scheduler ready list */
static rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
static rt_uint32_t rt_thread_ready_priority_group;

#if RT_THREAD_PRIORITY_MAX > 32
static rt_uint8_t rt_thread_ready_table[32];
#endif

/* Current running thread */
struct rt_thread *rt_current_thread = RT_NULL;
rt_uint8_t rt_current_priority;

/* Scheduler lock nesting */
static rt_int16_t rt_scheduler_lock_nest;

/* Interrupt context switch flag */
rt_uint32_t rt_interrupt_from_thread = 0;
rt_uint32_t rt_interrupt_to_thread = 0;
rt_uint32_t rt_thread_switch_interrupt_flag = 0;

/* Scheduler ready hook */
static void (*rt_scheduler_hook)(struct rt_thread *from, struct rt_thread *to);

/**
 * Set scheduler hook
 */
void rt_scheduler_sethook(void (*hook)(struct rt_thread *from, struct rt_thread *to))
{
    rt_scheduler_hook = hook;
}

/**
 * Get highest priority thread
 */
static struct rt_thread *_get_highest_priority_thread(rt_ubase_t *highest_prio)
{
    register struct rt_thread *highest_priority_thread;
    register rt_ubase_t highest_ready_priority;

#if RT_THREAD_PRIORITY_MAX > 32
    register rt_ubase_t number;
    number = __rt_ffs(rt_thread_ready_priority_group) - 1;
    highest_ready_priority = (number << 3) + __rt_ffs(rt_thread_ready_table[number]) - 1;
#else
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
#endif

    highest_priority_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                                           struct rt_thread,
                                           tlist);
    *highest_prio = highest_ready_priority;

    return highest_priority_thread;
}

/**
 * Initialize scheduler
 */
void rt_system_scheduler_init(void)
{
    register rt_base_t offset;

    rt_scheduler_lock_nest = 0;

    /* Initialize thread priority table */
    for (offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset++)
    {
        rt_list_init(&rt_thread_priority_table[offset]);
    }

    rt_current_priority = RT_THREAD_PRIORITY_MAX - 1;
    rt_current_thread = RT_NULL;

    /* Initialize ready priority group */
    rt_thread_ready_priority_group = 0;

#if RT_THREAD_PRIORITY_MAX > 32
    rt_memset(rt_thread_ready_table, 0, sizeof(rt_thread_ready_table));
#endif
}

/**
 * Start scheduler
 */
void rt_system_scheduler_start(void)
{
    register struct rt_thread *to_thread;
    rt_ubase_t highest_ready_priority;

    to_thread = _get_highest_priority_thread(&highest_ready_priority);

    rt_current_thread = to_thread;

    /* Switch to the highest priority thread */
    rt_hw_context_switch_to((rt_ubase_t)&to_thread->sp);

    /* Never reach here */
}

/**
 * Schedule
 */
void rt_schedule(void)
{
    rt_base_t level;
    struct rt_thread *to_thread;
    struct rt_thread *from_thread;

    level = rt_hw_interrupt_disable();

    /* Check scheduler lock */
    if (rt_scheduler_lock_nest == 0)
    {
        rt_ubase_t highest_ready_priority;

        if (rt_thread_ready_priority_group != 0)
        {
            to_thread = _get_highest_priority_thread(&highest_ready_priority);

            if (to_thread != rt_current_thread)
            {
                rt_current_priority = (rt_uint8_t)highest_ready_priority;
                from_thread = rt_current_thread;
                rt_current_thread = to_thread;

                if (rt_scheduler_hook)
                {
                    rt_scheduler_hook(from_thread, to_thread);
                }

                if (rt_thread_switch_interrupt_flag == 0)
                {
                    rt_hw_context_switch((rt_ubase_t)&from_thread->sp,
                                        (rt_ubase_t)&to_thread->sp);
                }
                else
                {
                    rt_interrupt_from_thread = (rt_ubase_t)&from_thread->sp;
                    rt_interrupt_to_thread = (rt_ubase_t)&to_thread->sp;
                }
            }
        }
    }

    rt_hw_interrupt_enable(level);
}

/**
 * Insert thread into ready list
 */
void rt_schedule_insert_thread(struct rt_thread *thread)
{
    register rt_base_t temp;

    RT_ASSERT(thread != RT_NULL);

    temp = rt_hw_interrupt_disable();

    /* Set thread status to ready */
    thread->stat = RT_THREAD_READY;

    /* Insert into ready list */
    rt_list_insert_before(&rt_thread_priority_table[thread->current_priority],
                          &thread->tlist);

    /* Set bit in ready group */
#if RT_THREAD_PRIORITY_MAX > 32
    rt_thread_ready_table[thread->number] |= thread->high_mask;
#endif
    rt_thread_ready_priority_group |= thread->number_mask;

    rt_hw_interrupt_enable(temp);
}

/**
 * Remove thread from ready list
 */
void rt_schedule_remove_thread(struct rt_thread *thread)
{
    register rt_base_t temp;

    RT_ASSERT(thread != RT_NULL);

    temp = rt_hw_interrupt_disable();

    /* Remove from ready list */
    rt_list_remove(&thread->tlist);

    /* Check if need to clear ready group */
    if (rt_list_isempty(&rt_thread_priority_table[thread->current_priority]))
    {
#if RT_THREAD_PRIORITY_MAX > 32
        rt_thread_ready_table[thread->number] &= ~thread->high_mask;
        if (rt_thread_ready_table[thread->number] == 0)
        {
            rt_thread_ready_priority_group &= ~thread->number_mask;
        }
#else
        rt_thread_ready_priority_group &= ~thread->number_mask;
#endif
    }

    rt_hw_interrupt_enable(temp);
}

/**
 * Enter critical region
 */
void rt_enter_critical(void)
{
    register rt_base_t level;

    level = rt_hw_interrupt_disable();

    rt_scheduler_lock_nest++;

    rt_hw_interrupt_enable(level);
}

/**
 * Exit critical region
 */
void rt_exit_critical(void)
{
    register rt_base_t level;

    level = rt_hw_interrupt_disable();

    rt_scheduler_lock_nest--;

    if (rt_scheduler_lock_nest <= 0)
    {
        rt_scheduler_lock_nest = 0;
        rt_hw_interrupt_enable(level);

        if (rt_current_thread)
        {
            rt_schedule();
        }
    }
    else
    {
        rt_hw_interrupt_enable(level);
    }
}

/**
 * Get critical level
 */
rt_uint16_t rt_critical_level(void)
{
    return rt_scheduler_lock_nest;
}

/* Find first set bit */
#if defined(__CC_ARM) || defined(__CLANG_ARM)
int __rt_ffs(int value)
{
    if (value == 0) return 0;
    return __builtin_ctz(value) + 1;
}
#elif defined(__GNUC__)
int __rt_ffs(int value)
{
    if (value == 0) return 0;
    return __builtin_ffs(value);
}
#else
const rt_uint8_t __lowest_bit_bitmap[] =
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
};

int __rt_ffs(int value)
{
    if (value == 0) return 0;
    if (value & 0xff)
        return __lowest_bit_bitmap[value & 0xff] + 1;
    if (value & 0xff00)
        return __lowest_bit_bitmap[(value & 0xff00) >> 8] + 9;
    if (value & 0xff0000)
        return __lowest_bit_bitmap[(value & 0xff0000) >> 16] + 17;
    return __lowest_bit_bitmap[(value & 0xff000000) >> 24] + 25;
}
#endif
