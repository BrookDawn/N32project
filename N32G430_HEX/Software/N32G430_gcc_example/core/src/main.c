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
*\*\file main.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/

#include "main.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "hal_compat.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "oled_app.h"
#include "oled_direct_test.h"  

/**
 *\*\name   main.
 *\*\fun    main function.
 *\*\param  none.
 *\*\return none.
**/
int main(void)
{
    HAL_InitTick();

    /* Initialize LEDs for simple status indication */
    LED_Initialize(LED1_GPIO_PORT, LED1_GPIO_PIN, LED1_GPIO_CLK);
    LED_Initialize(LED2_GPIO_PORT, LED2_GPIO_PIN | LED3_GPIO_PIN, LED2_GPIO_CLK);
    LED_Off(LED2_GPIO_PORT, LED1_GPIO_PIN | LED2_GPIO_PIN | LED3_GPIO_PIN);

    /* Initialize peripherals */
    MX_DMA_Init();
    MX_I2C1_Init();
    MX_TIM4_Init();

    /* Initialize keys (PB10, PB11) */
    BSP_KEY_Init();

    /* 先用直接测试验证OLED硬件（3秒） */
    LED_On(LED2_GPIO_PORT, LED1_GPIO_PIN);  /* LED1亮=开始直接测试 */
    OLED_DirectTest(&hi2c1, 0x78);
    HAL_Delay(3000);
    LED_Off(LED2_GPIO_PORT, LED1_GPIO_PIN);

    /* 然后用U8G2测试 */
    LED_On(LED2_GPIO_PORT, LED2_GPIO_PIN);  /* LED2亮=开始U8G2测试 */
    OLED_App_Init();
    LED_Off(LED2_GPIO_PORT, LED2_GPIO_PIN);

    while(1)
    {
        /* Update OLED application (check long press, etc.) */
        OLED_App_Update();

        /* Small delay to reduce CPU usage */
        HAL_Delay(10);
    }
}


