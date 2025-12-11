/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RT-Thread Hardware Layer
 */

#ifndef __RT_HW_H__
#define __RT_HW_H__

#include <rtdef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Hardware interface */
rt_base_t rt_hw_interrupt_disable(void);
void rt_hw_interrupt_enable(rt_base_t level);

/* Context switch */
void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to);
void rt_hw_context_switch_to(rt_ubase_t to);
void rt_hw_context_switch_interrupt(rt_ubase_t from, rt_ubase_t to);

/* Stack initialization */
rt_uint8_t *rt_hw_stack_init(void       *entry,
                             void       *parameter,
                             rt_uint8_t *stack_addr,
                             void       *exit);

/* Scheduler functions */
void rt_schedule_insert_thread(struct rt_thread *thread);
void rt_schedule_remove_thread(struct rt_thread *thread);

/* FFS */
int __rt_ffs(int value);

/* Timer control */
rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg);

/* vsnprintf */
int rt_vsnprintf(char *buf, rt_size_t size, const char *fmt, va_list args);
int rt_snprintf(char *buf, rt_size_t size, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __RT_HW_H__ */
