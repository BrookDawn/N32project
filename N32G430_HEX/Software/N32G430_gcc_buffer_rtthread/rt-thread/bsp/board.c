/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Board Support Package for N32G430
 */

#include <rtthread.h>
#include <rthw.h>
#include "n32g430.h"
#include "bsp_usart.h"

/* System clock frequency */
#define SYSTEM_CLOCK    48000000    /* 48MHz */

/* Forward declarations */
void rt_interrupt_enter(void);
void rt_interrupt_leave(void);
rt_thread_t rt_thread_create(const char *name,
                             void (*entry)(void *parameter),
                             void       *parameter,
                             rt_uint32_t stack_size,
                             rt_uint8_t  priority,
                             rt_uint32_t tick);

/* SysTick configuration */
static uint32_t _SysTick_Config(uint32_t ticks)
{
    if (ticks > SysTick_LOAD_RELOAD_Msk)
    {
        return 1;
    }

    SysTick->LOAD = ticks - 1;
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
    return 0;
}

/**
 * Hardware initialization for the RT-Thread
 */
void rt_hw_board_init(void)
{
    /* Configure SysTick */
    _SysTick_Config(SYSTEM_CLOCK / RT_TICK_PER_SECOND);

    /* Initialize USART1 for console output */
    USART1_Init();

#ifdef RT_USING_CONSOLE
    /* Set console device */
    /* rt_console_set_device(RT_CONSOLE_DEVICE_NAME); */
#endif
}

/**
 * Console output hook for rt_kprintf
 */
void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);

    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            USART1_SendByte(a);
        }
        USART1_SendByte(*(str + i));
    }
}

/**
 * SysTick interrupt handler
 * Called from startup file
 */
void SysTick_Handler(void)
{
    /* Enter interrupt */
    rt_interrupt_enter();

    /* Tick increase */
    rt_tick_increase();

    /* Leave interrupt */
    rt_interrupt_leave();
}

/* Interrupt nesting counter */
volatile rt_uint8_t rt_interrupt_nest = 0;

/**
 * Enter interrupt
 */
void rt_interrupt_enter(void)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();

    rt_interrupt_nest++;

    rt_hw_interrupt_enable(level);
}

/**
 * Leave interrupt
 */
void rt_interrupt_leave(void)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();

    rt_interrupt_nest--;

    rt_hw_interrupt_enable(level);
}

/**
 * Get interrupt nest
 */
rt_uint8_t rt_interrupt_get_nest(void)
{
    return rt_interrupt_nest;
}

/**
 * Hard fault exception handler
 */
void rt_hw_hard_fault_exception(void *sp)
{
    rt_kprintf("Hard Fault!\n");
    rt_kprintf("SCB->CFSR = 0x%08x\n", SCB->CFSR);
    rt_kprintf("SCB->HFSR = 0x%08x\n", SCB->HFSR);

    while (1);
}

/**
 * Entry point for application
 * This is called from RT-Thread after kernel is initialized
 */
void rt_application_init(void)
{
    /* The main thread is created in main.c */
    extern int main(void);
    rt_thread_t tid;

    tid = rt_thread_create("main",
                          (void (*)(void *))main,
                          RT_NULL,
                          RT_MAIN_THREAD_STACK_SIZE,
                          RT_THREAD_PRIORITY_MAX / 3,
                          20);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
}

/**
 * Create thread dynamically (for convenience)
 */
rt_thread_t rt_thread_create(const char *name,
                             void (*entry)(void *parameter),
                             void       *parameter,
                             rt_uint32_t stack_size,
                             rt_uint8_t  priority,
                             rt_uint32_t tick)
{
    static struct rt_thread main_thread;
    static rt_uint8_t main_stack[512];

    rt_thread_init(&main_thread,
                   name,
                   entry,
                   parameter,
                   main_stack,
                   sizeof(main_stack),
                   priority,
                   tick);

    return &main_thread;
}
