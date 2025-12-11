/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread System Entry
 */

#include <rtthread.h>
#include <rthw.h>

extern void rt_hw_board_init(void);
extern void rt_application_init(void);

/**
 * RT-Thread startup entry
 * Called from Reset_Handler after system initialization
 */
int rtthread_startup(void)
{
    /* Disable interrupt first */
    rt_hw_interrupt_disable();

    /* Board level initialization */
    rt_hw_board_init();

    /* Timer system initialization */
    rt_system_timer_init();

    /* Scheduler initialization */
    rt_system_scheduler_init();

    /* Initialize idle thread */
    rt_thread_idle_init();

    /* Initialize application */
    rt_application_init();

    /* Start scheduler */
    rt_system_scheduler_start();

    /* Never reach here */
    return 0;
}
