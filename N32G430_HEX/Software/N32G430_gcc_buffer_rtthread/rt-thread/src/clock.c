/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Clock and Timer
 */

#include <rtthread.h>
#include <rthw.h>

/* System tick */
static volatile rt_tick_t rt_tick = 0;

/* Timer list */
static rt_list_t rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL];

/**
 * Initialize system timer
 */
void rt_system_timer_init(void)
{
    int i;
    for (i = 0; i < RT_TIMER_SKIP_LIST_LEVEL; i++)
    {
        rt_list_init(&rt_timer_list[i]);
    }
}

/**
 * Get current tick
 */
rt_tick_t rt_tick_get(void)
{
    return rt_tick;
}

/**
 * Increase tick
 */
void rt_tick_increase(void)
{
    struct rt_thread *thread;

    rt_tick++;

    thread = rt_thread_self();
    if (thread != RT_NULL)
    {
        thread->remaining_tick--;
        if (thread->remaining_tick == 0)
        {
            thread->remaining_tick = thread->init_tick;
            thread->stat |= RT_THREAD_STAT_YIELD;
            rt_schedule();
        }
    }

    rt_timer_check();
}

/**
 * Initialize timer
 */
void rt_timer_init(rt_timer_t  timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void       *parameter,
                   rt_tick_t   time,
                   rt_uint8_t  flag)
{
    rt_object_init(&timer->parent, RT_Object_Class_Timer, name);

    /* Initialize timer list */
    rt_list_init(&timer->row[0]);

    timer->timeout_func = timeout;
    timer->parameter = parameter;
    timer->init_tick = time;
    timer->timeout_tick = 0;

    timer->parent.flag = flag & ~RT_TIMER_FLAG_ACTIVATED;
}

/**
 * Start timer
 */
rt_err_t rt_timer_start(rt_timer_t timer)
{
    rt_list_t *timer_list;
    rt_list_t *n;
    struct rt_timer *t;
    rt_base_t level;

    level = rt_hw_interrupt_disable();

    /* Remove timer from list first */
    rt_list_remove(&timer->row[0]);

    /* Get timeout tick */
    timer->timeout_tick = rt_tick_get() + timer->init_tick;

    timer_list = &rt_timer_list[0];

    /* Find insert position */
    rt_list_for_each(n, timer_list)
    {
        t = rt_list_entry(n, struct rt_timer, row[0]);
        if ((t->timeout_tick - timer->timeout_tick) > 0)
        {
            break;
        }
    }

    /* Insert timer */
    rt_list_insert_before(n, &timer->row[0]);

    timer->parent.flag |= RT_TIMER_FLAG_ACTIVATED;

    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

/**
 * Stop timer
 */
rt_err_t rt_timer_stop(rt_timer_t timer)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();

    /* Check if timer is activated */
    if (!(timer->parent.flag & RT_TIMER_FLAG_ACTIVATED))
    {
        rt_hw_interrupt_enable(level);
        return -RT_ERROR;
    }

    /* Remove timer from list */
    rt_list_remove(&timer->row[0]);

    timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

/**
 * Control timer
 */
rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();

    switch (cmd)
    {
    case RT_TIMER_CTRL_GET_TIME:
        *(rt_tick_t *)arg = timer->init_tick;
        break;

    case RT_TIMER_CTRL_SET_TIME:
        timer->init_tick = *(rt_tick_t *)arg;
        break;

    case RT_TIMER_CTRL_SET_ONESHOT:
        timer->parent.flag &= ~RT_TIMER_FLAG_PERIODIC;
        break;

    case RT_TIMER_CTRL_SET_PERIODIC:
        timer->parent.flag |= RT_TIMER_FLAG_PERIODIC;
        break;

    case RT_TIMER_CTRL_GET_STATE:
        if (timer->parent.flag & RT_TIMER_FLAG_ACTIVATED)
        {
            *(rt_uint32_t *)arg = RT_TIMER_FLAG_ACTIVATED;
        }
        else
        {
            *(rt_uint32_t *)arg = RT_TIMER_FLAG_DEACTIVATED;
        }
        break;

    default:
        break;
    }

    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

/**
 * Check timer timeout
 */
void rt_timer_check(void)
{
    struct rt_timer *t;
    rt_tick_t current_tick;
    rt_base_t level;

    current_tick = rt_tick_get();

    level = rt_hw_interrupt_disable();

    while (!rt_list_isempty(&rt_timer_list[0]))
    {
        t = rt_list_entry(rt_timer_list[0].next, struct rt_timer, row[0]);

        if ((current_tick - t->timeout_tick) < RT_TICK_MAX / 2)
        {
            /* Remove timer from list */
            rt_list_remove(&t->row[0]);

            t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

            /* Call timeout function */
            if (t->timeout_func != RT_NULL)
            {
                t->timeout_func(t->parameter);
            }

            /* Restart periodic timer */
            if (t->parent.flag & RT_TIMER_FLAG_PERIODIC)
            {
                t->timeout_tick = current_tick + t->init_tick;
                rt_timer_start(t);
            }
        }
        else
        {
            break;
        }
    }

    rt_hw_interrupt_enable(level);
}
