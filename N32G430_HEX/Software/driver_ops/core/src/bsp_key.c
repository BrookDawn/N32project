/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
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
*\*\file bsp_key.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/

#include "bsp_key.h"
#include "n32g430_exti.h"
#include "n32g430_gpio.h"

/**
  * @brief  Initialize key GPIO and EXTI
  */
void BSP_KEY_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;

    /* Enable GPIO clock */
    RCC_AHB_Peripheral_Clock_Enable(KEY_LEFT_GPIO_CLK);

    /* Enable AFIO clock for EXTI */
    RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO);

    /* Initialize GPIO structure */
    GPIO_Structure_Initialize(&GPIO_InitStructure);

    /* Configure PB10 as input with pull-up */
    GPIO_InitStructure.Pin           = KEY_LEFT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode     = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull     = GPIO_PULL_UP;
    GPIO_Peripheral_Initialize(KEY_LEFT_GPIO_PORT, &GPIO_InitStructure);

    /* Configure PB11 as input with pull-up */
    GPIO_InitStructure.Pin           = KEY_RIGHT_GPIO_PIN;
    GPIO_Peripheral_Initialize(KEY_RIGHT_GPIO_PORT, &GPIO_InitStructure);

    /* Connect EXTI Line to GPIO Pin */
    GPIO_EXTI_Line_Set(KEY_LEFT_PORT_SOURCE, KEY_LEFT_PIN_SOURCE);
    GPIO_EXTI_Line_Set(KEY_RIGHT_PORT_SOURCE, KEY_RIGHT_PIN_SOURCE);

    /* Configure EXTI Line 10 (PB10) - Both edges for press and release detection */
    EXTI_InitStructure.EXTI_Line    = KEY_LEFT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  /* Both edges */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Peripheral_Initializes(&EXTI_InitStructure);

    /* Configure EXTI Line 11 (PB11) - Both edges for press and release detection */
    EXTI_InitStructure.EXTI_Line    = KEY_RIGHT_EXTI_LINE;
    EXTI_Peripheral_Initializes(&EXTI_InitStructure);

    /* Enable and set EXTI10-15 Interrupt */
    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);
}

/**
  * @brief  Enable key interrupts
  */
void BSP_KEY_EnableIRQ(void)
{
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
  * @brief  Disable key interrupts
  */
void BSP_KEY_DisableIRQ(void)
{
    NVIC_DisableIRQ(EXTI15_10_IRQn);
}
