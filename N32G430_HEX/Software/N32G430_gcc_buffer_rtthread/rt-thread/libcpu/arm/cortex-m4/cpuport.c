/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Cortex-M4 CPU port for RT-Thread (GCC)
 */

#include <rtthread.h>
#include <rthw.h>

/* Exception return value */
#define EXC_RETURN_PROCESS_STACK    0xFFFFFFFD

/* Align macro */
#ifndef RT_ALIGN_DOWN
#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))
#endif

/* Register structure for context save */
struct exception_stack_frame
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r12;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t psr;
};

struct stack_frame
{
#if defined(__VFP_FP__) && !defined(__SOFTFP__)
    rt_uint32_t flag;
#endif

    /* r4 ~ r11 register */
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t r11;

    struct exception_stack_frame exception_stack_frame;
};

/**
 * Initialize thread stack
 */
rt_uint8_t *rt_hw_stack_init(void       *tentry,
                             void       *parameter,
                             rt_uint8_t *stack_addr,
                             void       *texit)
{
    struct stack_frame *stack_frame;
    rt_uint8_t *stk;
    unsigned long i;

    stk  = stack_addr + sizeof(rt_ubase_t);
    stk  = (rt_uint8_t *)RT_ALIGN_DOWN((rt_ubase_t)stk, 8);
    stk -= sizeof(struct stack_frame);

    stack_frame = (struct stack_frame *)stk;

    /* Init all register to 0xDEADBEEF */
    for (i = 0; i < sizeof(struct stack_frame) / sizeof(rt_uint32_t); i++)
    {
        ((rt_uint32_t *)stack_frame)[i] = 0xDEADBEEF;
    }

    stack_frame->exception_stack_frame.r0  = (unsigned long)parameter;
    stack_frame->exception_stack_frame.r1  = 0;
    stack_frame->exception_stack_frame.r2  = 0;
    stack_frame->exception_stack_frame.r3  = 0;
    stack_frame->exception_stack_frame.r12 = 0;
    stack_frame->exception_stack_frame.lr  = (unsigned long)texit;
    stack_frame->exception_stack_frame.pc  = (unsigned long)tentry;
    stack_frame->exception_stack_frame.psr = 0x01000000UL;

#if defined(__VFP_FP__) && !defined(__SOFTFP__)
    stack_frame->flag = 0;
#endif

    return stk;
}

/* Align macro */
#ifndef RT_ALIGN_DOWN
#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))
#endif
