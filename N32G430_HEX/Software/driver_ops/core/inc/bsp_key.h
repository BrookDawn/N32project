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
*\*\file bsp_key.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/

#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "n32g430.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define KEY_LEFT_GPIO_PORT      GPIOB
#define KEY_LEFT_GPIO_PIN       GPIO_PIN_10
#define KEY_LEFT_GPIO_CLK       RCC_AHB_PERIPH_GPIOB
#define KEY_LEFT_EXTI_LINE      EXTI_LINE10
#define KEY_LEFT_PORT_SOURCE    GPIOB_PORT_SOURCE
#define KEY_LEFT_PIN_SOURCE     GPIO_PIN_SOURCE10

#define KEY_RIGHT_GPIO_PORT     GPIOB
#define KEY_RIGHT_GPIO_PIN      GPIO_PIN_11
#define KEY_RIGHT_GPIO_CLK      RCC_AHB_PERIPH_GPIOB
#define KEY_RIGHT_EXTI_LINE     EXTI_LINE11
#define KEY_RIGHT_PORT_SOURCE   GPIOB_PORT_SOURCE
#define KEY_RIGHT_PIN_SOURCE    GPIO_PIN_SOURCE11

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  Initialize key GPIO and EXTI
  */
void BSP_KEY_Init(void);

/**
  * @brief  Enable key interrupts
  */
void BSP_KEY_EnableIRQ(void);

/**
  * @brief  Disable key interrupts
  */
void BSP_KEY_DisableIRQ(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_KEY_H__ */
