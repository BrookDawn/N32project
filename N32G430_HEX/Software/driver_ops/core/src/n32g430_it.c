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
*\*\file n32g430_it.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/

#include "n32g430_it.h"
#include "n32g430.h"
#include "main.h"
#include "hal_compat.h"
#include "dma.h"
#include "tim.h"
#include "oled_app.h"


/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/


/**
 *\*\name   NMI_Handler.
 *\*\fun    This function handles NMI exception.
 *\*\param  none.
 *\*\return none.
 */
void NMI_Handler(void)
{
}


/**
 *\*\name   HardFault_Handler.
 *\*\fun    This function handles Hard Fault exception.
 *\*\param  none.
 *\*\return none.
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}


/**
 *\*\name   MemManage_Handler.
 *\*\fun    This function handles Memory Manage exception.
 *\*\param  none.
 *\*\return none.
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}


/**
 *\*\name   BusFault_Handler.
 *\*\fun    This function handles Bus Fault exception.
 *\*\param  none.
 *\*\return none.
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}


/**
 *\*\name   UsageFault_Handler.
 *\*\fun    This function handles Usage Fault exception.
 *\*\param  none.
 *\*\return none.
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}


/**
 *\*\name   SVC_Handler.
 *\*\fun    This function handles SVCall exception.
 *\*\param  none.
 *\*\return none.
 */
void SVC_Handler(void)
{
}


/**
 *\*\name   DebugMon_Handler.
 *\*\fun    This function handles Debug Monitor exception.
 *\*\param  none.
 *\*\return none.
 */
void DebugMon_Handler(void)
{
}


/**
 *\*\name   SysTick_Handler.
 *\*\fun    This function handles SysTick Handler.
 *\*\param  none.
 *\*\return none.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 *\*\name   DMA_Channel1_IRQHandler.
 *\*\fun    Handle DMA1 channel 1 interrupt (I2C1 TX).
 *\*\param  none.
 *\*\return none.
 */
void DMA_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c1_tx);
}

/**
 *\*\name   TIM4_IRQHandler.
 *\*\fun    Handle TIM4 global interrupt.
 *\*\param  none.
 *\*\return none.
 */
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim4);
}



/******************************************************************************/
/*                 N32G430 Peripherals Interrupt Handlers                     */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_n32g430.s).                                                 */
/******************************************************************************/

/* Key debounce variables */
static volatile uint32_t last_key_time[2] = {0, 0};  /* 0: PB10, 1: PB11 */
static volatile uint32_t key_press_time[2] = {0, 0}; /* Key press start time */
static volatile uint8_t key_pressed[2] = {0, 0};     /* Key pressed flag */
static volatile uint8_t long_press_triggered[2] = {0, 0}; /* Long press triggered flag */
#define KEY_DEBOUNCE_TIME_MS  50   /* Debounce time in milliseconds */
#define KEY_LONG_PRESS_TIME_MS 1000 /* Long press time in milliseconds */

/**
 *\*\name   EXTI15_10_IRQHandler.
 *\*\fun    Handle EXTI Line[15:10] interrupt (Key interrupts).
 *\*\param  none.
 *\*\return none.
 */
void EXTI15_10_IRQHandler(void)
{
    uint32_t current_time = HAL_GetTick();

    /* Check EXTI Line 10 (PB10 - Left Key) */
    if (EXTI_Interrupt_Status_Get(EXTI_LINE10) != RESET)
    {
        /* Check if key is pressed (falling edge) */
        if (GPIO_Input_Pin_Data_Get(GPIOB, GPIO_PIN_10) == 0)
        {
            /* Debounce check */
            if ((current_time - last_key_time[0]) > KEY_DEBOUNCE_TIME_MS)
            {
                last_key_time[0] = current_time;
                key_press_time[0] = current_time;
                key_pressed[0] = 1;
                long_press_triggered[0] = 0;
            }
        }
        else  /* Key released */
        {
            if (key_pressed[0])
            {
                uint32_t press_duration = current_time - key_press_time[0];

                /* Only trigger short press if long press wasn't triggered */
                if (!long_press_triggered[0] && press_duration < KEY_LONG_PRESS_TIME_MS)
                {
                    OLED_App_KeyCallback(GPIO_PIN_10);
                }

                key_pressed[0] = 0;
                long_press_triggered[0] = 0;
            }
        }
        /* Clear interrupt flag */
        EXTI_Interrupt_Status_Clear(EXTI_LINE10);
    }

    /* Check EXTI Line 11 (PB11 - Right Key) */
    if (EXTI_Interrupt_Status_Get(EXTI_LINE11) != RESET)
    {
        /* Check if key is pressed (falling edge) */
        if (GPIO_Input_Pin_Data_Get(GPIOB, GPIO_PIN_11) == 0)
        {
            /* Debounce check */
            if ((current_time - last_key_time[1]) > KEY_DEBOUNCE_TIME_MS)
            {
                last_key_time[1] = current_time;
                key_press_time[1] = current_time;
                key_pressed[1] = 1;
                long_press_triggered[1] = 0;
            }
        }
        else  /* Key released */
        {
            if (key_pressed[1])
            {
                uint32_t press_duration = current_time - key_press_time[1];

                /* Only trigger short press if long press wasn't triggered */
                if (!long_press_triggered[1] && press_duration < KEY_LONG_PRESS_TIME_MS)
                {
                    OLED_App_KeyCallback(GPIO_PIN_11);
                }

                key_pressed[1] = 0;
                long_press_triggered[1] = 0;
            }
        }
        /* Clear interrupt flag */
        EXTI_Interrupt_Status_Clear(EXTI_LINE11);
    }
}

/**
 *\*\name   BSP_KEY_CheckLongPress.
 *\*\fun    Check for long press events (should be called periodically).
 *\*\param  none.
 *\*\return none.
 */
void BSP_KEY_CheckLongPress(void)
{
    uint32_t current_time = HAL_GetTick();

    /* Check PB10 long press */
    if (key_pressed[0] && !long_press_triggered[0])
    {
        if ((current_time - key_press_time[0]) >= KEY_LONG_PRESS_TIME_MS)
        {
            long_press_triggered[0] = 1;
            /* Trigger long press callback with special flag */
            OLED_App_KeyCallback(GPIO_PIN_10 | 0x8000);  /* MSB indicates long press */
        }
    }

    /* Check PB11 long press */
    if (key_pressed[1] && !long_press_triggered[1])
    {
        if ((current_time - key_press_time[1]) >= KEY_LONG_PRESS_TIME_MS)
        {
            long_press_triggered[1] = 1;
            /* Trigger long press callback with special flag */
            OLED_App_KeyCallback(GPIO_PIN_11 | 0x8000);  /* MSB indicates long press */
        }
    }
}

