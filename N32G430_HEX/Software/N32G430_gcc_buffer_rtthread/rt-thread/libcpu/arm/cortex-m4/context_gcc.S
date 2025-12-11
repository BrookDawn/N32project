/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Cortex-M4 context switch for RT-Thread (GCC)
 */

    .syntax unified
    .cpu cortex-m4
    .fpu softvfp
    .thumb

.global rt_hw_interrupt_disable
.global rt_hw_interrupt_enable
.global rt_hw_context_switch
.global rt_hw_context_switch_to
.global rt_hw_context_switch_interrupt
.global PendSV_Handler
.global rt_hw_hard_fault_exception

.global rt_thread_switch_interrupt_flag
.global rt_interrupt_from_thread
.global rt_interrupt_to_thread

.equ SCB_VTOR,          0xE000ED08
.equ NVIC_INT_CTRL,     0xE000ED04
.equ NVIC_SYSPRI2,      0xE000ED20
.equ NVIC_PENDSV_PRI,   0xFFFF0000
.equ NVIC_PENDSVSET,    0x10000000

/*
 * rt_base_t rt_hw_interrupt_disable()
 */
.type rt_hw_interrupt_disable, %function
rt_hw_interrupt_disable:
    MRS     r0, PRIMASK
    CPSID   I
    BX      LR

/*
 * void rt_hw_interrupt_enable(rt_base_t level)
 */
.type rt_hw_interrupt_enable, %function
rt_hw_interrupt_enable:
    MSR     PRIMASK, r0
    BX      LR

/*
 * void rt_hw_context_switch(rt_uint32_t from, rt_uint32_t to)
 * r0 --> from
 * r1 --> to
 */
.type rt_hw_context_switch, %function
rt_hw_context_switch:
    /* Set rt_thread_switch_interrupt_flag to 1 */
    LDR     r2, =rt_thread_switch_interrupt_flag
    LDR     r3, [r2]
    CMP     r3, #1
    BEQ     _reswitch
    MOV     r3, #1
    STR     r3, [r2]

    LDR     r2, =rt_interrupt_from_thread
    STR     r0, [r2]

_reswitch:
    LDR     r2, =rt_interrupt_to_thread
    STR     r1, [r2]

    /* Trigger PendSV */
    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]
    BX      LR

/*
 * void rt_hw_context_switch_to(rt_uint32_t to)
 * r0 --> to
 */
.type rt_hw_context_switch_to, %function
rt_hw_context_switch_to:
    /* Set rt_interrupt_to_thread */
    LDR     r1, =rt_interrupt_to_thread
    STR     r0, [r1]

    /* Set rt_interrupt_from_thread to 0 */
    LDR     r1, =rt_interrupt_from_thread
    MOV     r0, #0
    STR     r0, [r1]

    /* Set rt_thread_switch_interrupt_flag to 1 */
    LDR     r1, =rt_thread_switch_interrupt_flag
    MOV     r0, #1
    STR     r0, [r1]

    /* Set PendSV exception priority */
    LDR     r0, =NVIC_SYSPRI2
    LDR     r1, =NVIC_PENDSV_PRI
    LDR     r2, [r0]
    ORR     r1, r1, r2
    STR     r1, [r0]

    /* Trigger PendSV */
    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]

    /* Enable interrupts */
    CPSIE   I

    /* Never return */
_wait_for_pendSV:
    B       _wait_for_pendSV

/*
 * void rt_hw_context_switch_interrupt(rt_uint32_t from, rt_uint32_t to)
 */
.type rt_hw_context_switch_interrupt, %function
rt_hw_context_switch_interrupt:
    /* Set rt_thread_switch_interrupt_flag to 1 */
    LDR     r2, =rt_thread_switch_interrupt_flag
    LDR     r3, [r2]
    CMP     r3, #1
    BEQ     _int_reswitch
    MOV     r3, #1
    STR     r3, [r2]

    LDR     r2, =rt_interrupt_from_thread
    STR     r0, [r2]

_int_reswitch:
    LDR     r2, =rt_interrupt_to_thread
    STR     r1, [r2]
    BX      LR

/*
 * PendSV Handler
 */
.type PendSV_Handler, %function
PendSV_Handler:
    /* Disable interrupts */
    MRS     r2, PRIMASK
    CPSID   I

    /* Check rt_thread_switch_interrupt_flag */
    LDR     r0, =rt_thread_switch_interrupt_flag
    LDR     r1, [r0]
    CBZ     r1, pendsv_exit

    /* Clear rt_thread_switch_interrupt_flag */
    MOV     r1, #0
    STR     r1, [r0]

    /* Check rt_interrupt_from_thread */
    LDR     r0, =rt_interrupt_from_thread
    LDR     r1, [r0]
    CBZ     r1, switch_to_thread

    /* Save from thread context */
    MRS     r1, PSP

    STMFD   r1!, {r4 - r11}

    LDR     r0, [r0]
    STR     r1, [r0]

switch_to_thread:
    /* Load to thread context */
    LDR     r1, =rt_interrupt_to_thread
    LDR     r1, [r1]
    LDR     r1, [r1]

    LDMFD   r1!, {r4 - r11}

    MSR     PSP, r1

pendsv_exit:
    /* Restore interrupts */
    MSR     PRIMASK, r2

    /* EXC_RETURN = 0xFFFFFFFD (return to thread mode, use PSP) */
    ORR     LR, LR, #0x04
    BX      LR

/*
 * Hard Fault Handler
 */
.type HardFault_Handler, %function
HardFault_Handler:
    /* Get current stack pointer */
    TST     lr, #4
    ITE     EQ
    MRSEQ   r0, MSP
    MRSNE   r0, PSP

    PUSH    {lr}
    BL      rt_hw_hard_fault_exception
    POP     {lr}

    BX      LR

.end
