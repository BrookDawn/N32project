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
#include "hal_compat.h"
#include "dma.h"
#include "tim.h"
#include "display_config.h"

/* Include display-specific headers based on configuration */
#ifdef DISPLAY_USE_LCD
#include "lcd_menu_demo.h"  /* Menu demo mode */
// #include "button_test_demo.h"  /* Button test mode (for debugging) */
#else
#include "oled_test_demo.h"
#endif

#include "spi.h"

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

    MX_TIM4_Init();
#ifdef DISPLAY_USE_LCD
    /* Menu demo initialization */
    LCD_MenuDemo_Init();

    while(1)
    {
        LCD_MenuDemo_Main();
    }
#else
    /* OLED test demo initialization */
    OLED_TestDemo_Init();

    while(1)
    {
        OLED_TestDemo_Main();
    }
#endif
}
