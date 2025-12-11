/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Inter-Process Communication
 */

#include <rtthread.h>
#include <rthw.h>

#ifdef RT_USING_SEMAPHORE

/**
 * Initialize a semaphore
 */
rt_err_t rt_sem_init(rt_sem_t    sem,
                     const char *name,
                     rt_uint32_t value,
                     rt_uint8_t  flag)
{
    RT_ASSERT(sem != RT_NULL);

    /* Initialize object */
    rt_object_init(&sem->parent, RT_Object_Class_Semaphore, name);

    /* Initialize suspend list */
    rt_list_init(&sem->suspend_thread);

    sem->value = value;
    sem->parent.flag = flag;

    return RT_EOK;
}

/**
 * Detach a semaphore
 */
rt_err_t rt_sem_detach(rt_sem_t sem)
{
    RT_ASSERT(sem != RT_NULL);

    /* Wakeup all suspend threads */
    while (!rt_list_isempty(&sem->suspend_thread))
    {
        struct rt_thread *thread;
        register rt_base_t level;

        level = rt_hw_interrupt_disable();

        thread = rt_list_entry(sem->suspend_thread.next,
                               struct rt_thread,
                               tlist);
        thread->error = -RT_ERROR;

        rt_thread_resume(thread);

        rt_hw_interrupt_enable(level);
    }

    rt_object_detach(&sem->parent);

    return RT_EOK;
}

/**
 * Take a semaphore
 */
rt_err_t rt_sem_take(rt_sem_t sem, rt_int32_t timeout)
{
    register rt_base_t level;
    struct rt_thread *thread;

    RT_ASSERT(sem != RT_NULL);

    level = rt_hw_interrupt_disable();

    if (sem->value > 0)
    {
        sem->value--;
        rt_hw_interrupt_enable(level);
    }
    else
    {
        if (timeout == 0)
        {
            rt_hw_interrupt_enable(level);
            return -RT_ETIMEOUT;
        }

        thread = rt_thread_self();

        /* Reset error */
        thread->error = RT_EOK;

        /* Suspend thread */
        rt_thread_suspend(thread);

        /* Insert to suspend list */
        if (sem->parent.flag == RT_IPC_FLAG_FIFO)
        {
            rt_list_insert_before(&sem->suspend_thread, &thread->tlist);
        }
        else
        {
            rt_list_insert_after(&sem->suspend_thread, &thread->tlist);
        }

        /* Start timer */
        if (timeout > 0)
        {
            rt_timer_control(&thread->thread_timer,
                           RT_TIMER_CTRL_SET_TIME,
                           &timeout);
            rt_timer_start(&thread->thread_timer);
        }

        rt_hw_interrupt_enable(level);

        /* Do schedule */
        rt_schedule();

        if (thread->error != RT_EOK)
        {
            return thread->error;
        }
    }

    return RT_EOK;
}

/**
 * Try to take a semaphore
 */
rt_err_t rt_sem_trytake(rt_sem_t sem)
{
    return rt_sem_take(sem, 0);
}

/**
 * Release a semaphore
 */
rt_err_t rt_sem_release(rt_sem_t sem)
{
    register rt_base_t level;
    register rt_bool_t need_schedule;

    RT_ASSERT(sem != RT_NULL);

    need_schedule = RT_FALSE;

    level = rt_hw_interrupt_disable();

    if (!rt_list_isempty(&sem->suspend_thread))
    {
        struct rt_thread *thread;

        thread = rt_list_entry(sem->suspend_thread.next,
                               struct rt_thread,
                               tlist);

        /* Remove from suspend list */
        rt_list_remove(&thread->tlist);

        /* Stop timer */
        rt_timer_stop(&thread->thread_timer);

        /* Resume thread */
        rt_thread_resume(thread);

        need_schedule = RT_TRUE;
    }
    else
    {
        if (sem->value < RT_UINT16_MAX)
        {
            sem->value++;
        }
    }

    rt_hw_interrupt_enable(level);

    if (need_schedule == RT_TRUE)
    {
        rt_schedule();
    }

    return RT_EOK;
}

#endif /* RT_USING_SEMAPHORE */

#ifdef RT_USING_MUTEX

/**
 * Initialize a mutex
 */
rt_err_t rt_mutex_init(rt_mutex_t mutex, const char *name, rt_uint8_t flag)
{
    RT_ASSERT(mutex != RT_NULL);

    /* Initialize object */
    rt_object_init(&mutex->parent, RT_Object_Class_Mutex, name);

    /* Initialize suspend list */
    rt_list_init(&mutex->suspend_thread);

    mutex->value = 1;
    mutex->owner = RT_NULL;
    mutex->original_priority = 0xFF;
    mutex->hold = 0;
    mutex->parent.flag = flag;

    return RT_EOK;
}

/**
 * Detach a mutex
 */
rt_err_t rt_mutex_detach(rt_mutex_t mutex)
{
    RT_ASSERT(mutex != RT_NULL);

    /* Wakeup all suspend threads */
    while (!rt_list_isempty(&mutex->suspend_thread))
    {
        struct rt_thread *thread;
        register rt_base_t level;

        level = rt_hw_interrupt_disable();

        thread = rt_list_entry(mutex->suspend_thread.next,
                               struct rt_thread,
                               tlist);
        thread->error = -RT_ERROR;

        rt_thread_resume(thread);

        rt_hw_interrupt_enable(level);
    }

    rt_object_detach(&mutex->parent);

    return RT_EOK;
}

/**
 * Take a mutex
 */
rt_err_t rt_mutex_take(rt_mutex_t mutex, rt_int32_t timeout)
{
    register rt_base_t level;
    struct rt_thread *thread;

    RT_ASSERT(mutex != RT_NULL);

    thread = rt_thread_self();

    level = rt_hw_interrupt_disable();

    /* Current thread already own the mutex */
    if (mutex->owner == thread)
    {
        mutex->hold++;
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }

    /* The mutex is available */
    if (mutex->value > 0)
    {
        mutex->value = 0;
        mutex->owner = thread;
        mutex->original_priority = thread->current_priority;
        mutex->hold = 1;

        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }

    /* No wait */
    if (timeout == 0)
    {
        rt_hw_interrupt_enable(level);
        return -RT_ETIMEOUT;
    }

    /* Suspend current thread */
    thread->error = RT_EOK;
    rt_thread_suspend(thread);

    /* Insert to suspend list */
    rt_list_insert_before(&mutex->suspend_thread, &thread->tlist);

    /* Start timer */
    if (timeout > 0)
    {
        rt_timer_control(&thread->thread_timer,
                       RT_TIMER_CTRL_SET_TIME,
                       &timeout);
        rt_timer_start(&thread->thread_timer);
    }

    rt_hw_interrupt_enable(level);

    /* Do schedule */
    rt_schedule();

    return thread->error;
}

/**
 * Release a mutex
 */
rt_err_t rt_mutex_release(rt_mutex_t mutex)
{
    register rt_base_t level;
    struct rt_thread *thread;
    rt_bool_t need_schedule;

    RT_ASSERT(mutex != RT_NULL);

    need_schedule = RT_FALSE;

    thread = rt_thread_self();

    level = rt_hw_interrupt_disable();

    /* Not the owner */
    if (mutex->owner != thread)
    {
        rt_hw_interrupt_enable(level);
        return -RT_ERROR;
    }

    /* Decrease hold */
    mutex->hold--;

    if (mutex->hold == 0)
    {
        /* Wakeup suspend thread */
        if (!rt_list_isempty(&mutex->suspend_thread))
        {
            struct rt_thread *next_thread;

            next_thread = rt_list_entry(mutex->suspend_thread.next,
                                        struct rt_thread,
                                        tlist);

            /* Remove from suspend list */
            rt_list_remove(&next_thread->tlist);

            /* Stop timer */
            rt_timer_stop(&next_thread->thread_timer);

            /* Set new owner */
            mutex->owner = next_thread;
            mutex->original_priority = next_thread->current_priority;
            mutex->hold = 1;

            /* Resume thread */
            rt_thread_resume(next_thread);

            need_schedule = RT_TRUE;
        }
        else
        {
            mutex->value = 1;
            mutex->owner = RT_NULL;
            mutex->original_priority = 0xFF;
        }
    }

    rt_hw_interrupt_enable(level);

    if (need_schedule == RT_TRUE)
    {
        rt_schedule();
    }

    return RT_EOK;
}

#endif /* RT_USING_MUTEX */
