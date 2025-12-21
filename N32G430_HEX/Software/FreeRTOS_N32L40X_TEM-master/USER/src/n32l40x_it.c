/*****************************************************************************
 * Copyright (c) 2022, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32l40x_it.c
 * @author Nations
 * @version V1.2.2
 *
 * @copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
 * 
 * @note 此文件已针对FreeRTOS进行优化配置：
 *       - SVC_Handler和SysTick_Handler已被注释，由FreeRTOS内核管理
 *       - 适配N32L403KBQ7芯片，系统时钟64MHz，堆大小24KB
 *       - 保留必要的异常处理程序以确保系统稳定性
 */
#include "n32l40x_it.h"
#include "n32l40x.h"
#include "main.h"

/** @addtogroup N32L40X_StdPeriph_Template
 * @{
 */

extern __IO uint32_t CurrDataCounterEnd;

/******************************************************************************/
/*            Cortex-M4 处理器异常处理程序                                      */
/******************************************************************************/

/**
 * @brief  此函数处理NMI异常。
 */
void NMI_Handler(void)
{
}

/**
 * @brief  此函数处理硬件错误异常。
 */
void HardFault_Handler(void)
{
    /* 当硬件错误异常发生时进入无限循环 */
    while (1)
    {
    }
}

/**
 * @brief  此函数处理内存管理异常。
 */
void MemManage_Handler(void)
{
    /* 当内存管理异常发生时进入无限循环 */
    while (1)
    {
    }
}

/**
 * @brief  此函数处理总线错误异常。
 */
void BusFault_Handler(void)
{
    /* 当总线错误异常发生时进入无限循环 */
    while (1)
    {
    }
}

/**
 * @brief  此函数处理使用错误异常。
 */
void UsageFault_Handler(void)
{
    /* 当使用错误异常发生时进入无限循环 */
    while (1)
    {
    }
}

/**
 * @brief  此函数处理SVCall异常。
 * @note   在FreeRTOS中，此函数由FreeRTOS内核管理，不应在此处实现。
 */
/*
void SVC_Handler(void)
{
}
*/

/**
 * @brief  此函数处理调试监视器异常。
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  此函数处理SysTick中断。
 * @note   在FreeRTOS中，SysTick由FreeRTOS内核管理，不应在此处实现。
 *         FreeRTOS使用SysTick作为系统时钟节拍，由xPortSysTickHandler处理。
 */
/*
void SysTick_Handler(void)
{
}
*/

/**
 * @brief  此函数处理在main.h中定义的DMA中断请求。
 */
void DMA_IRQ_HANDLER(void)
{
}

/******************************************************************************/
/*                 N32L40X 外设中断处理程序                                    */
/*  在此处添加所使用外设的中断处理程序(PPP)，可用的外设中断处理程序名称请参考     */
/*  启动文件 (startup_n32l40x.s)。                                           */
/******************************************************************************/

/**
 * @brief  此函数处理PPP中断请求。
 */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @}
 */
