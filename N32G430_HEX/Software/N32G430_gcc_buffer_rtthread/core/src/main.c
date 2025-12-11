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

/* RT-Thread include */
#include <rtthread.h>

/*BSP include*/
#include "main.h"
#include "bsp_led.h"
#include "bsp_usart.h"

/*RTT include*/
#include "SEGGER_RTT.h"

/* Thread definitions */
#define LED_THREAD_STACK_SIZE   256
#define LED_THREAD_PRIORITY     20
#define LED_THREAD_TIMESLICE    5

#define UART_THREAD_STACK_SIZE  512
#define UART_THREAD_PRIORITY    10
#define UART_THREAD_TIMESLICE   5

/* Thread control blocks */
static struct rt_thread led_thread;
static struct rt_thread uart_thread;

/* Thread stacks */
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_thread_stack[LED_THREAD_STACK_SIZE];
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t uart_thread_stack[UART_THREAD_STACK_SIZE];

/* Semaphore for UART data */
static struct rt_semaphore uart_rx_sem;

/**
 * LED thread entry
 * Blinks LEDs periodically
 */
static void led_thread_entry(void *parameter)
{
    (void)parameter;

    /* Initialize LEDs */
    LED_Initialize(LED1_GPIO_PORT, LED1_GPIO_PIN, LED1_GPIO_CLK);
    LED_Initialize(LED2_GPIO_PORT, LED2_GPIO_PIN | LED3_GPIO_PIN, LED2_GPIO_CLK);

    /* Turn off all LEDs */
    LED_Off(LED2_GPIO_PORT, LED1_GPIO_PIN | LED2_GPIO_PIN | LED3_GPIO_PIN);

    rt_kprintf("LED thread started!\n");

    while (1)
    {
        /* Toggle LED1 */
        LED_Toggle(LED1_GPIO_PORT, LED1_GPIO_PIN);
        rt_thread_mdelay(500);

        /* Toggle LED2 */
        LED_Toggle(LED2_GPIO_PORT, LED2_GPIO_PIN);
        rt_thread_mdelay(500);

        /* Toggle LED3 */
        LED_Toggle(LED3_GPIO_PORT, LED3_GPIO_PIN);
        rt_thread_mdelay(500);
    }
}

/**
 * UART thread entry
 * Handles UART receive and echo
 */
static void uart_thread_entry(void *parameter)
{
    char rx_buffer[128];
    uint16_t rx_length;

    (void)parameter;

    rt_kprintf("N32G430 RT-Thread Nano Started!\n");
    rt_kprintf("UART1 Test - Baudrate: 115200\n");
    rt_kprintf("Send any character to echo back...\n");

    /* Also send to UART directly */
    USART1_SendString("N32G430 RT-Thread Nano Started!\r\n");
    USART1_SendString("UART1 Test - Baudrate: 115200\r\n");
    USART1_SendString("Send any character to echo back...\r\n");

    while (1)
    {
        /* Check UART receive buffer */
        rx_length = USART1_ReceiveString(rx_buffer, sizeof(rx_buffer));

        if (rx_length > 0)
        {
            /* Echo received data */
            USART1_SendString("Received: ");
            USART1_SendArray((uint8_t*)rx_buffer, rx_length);
            USART1_SendString("\r\n");

            /* Also output to RTT console */
            rt_kprintf("Received %d bytes: %s\n", rx_length, rx_buffer);

            /* Print LED status */
            USART1_SendString("LED Status: ");
            if (GPIO_Output_Pin_Data_Get(LED1_GPIO_PORT, LED1_GPIO_PIN))
            {
                USART1_SendString("LED1=ON ");
            }
            else
            {
                USART1_SendString("LED1=OFF ");
            }

            if (GPIO_Output_Pin_Data_Get(LED2_GPIO_PORT, LED2_GPIO_PIN))
            {
                USART1_SendString("LED2=ON ");
            }
            else
            {
                USART1_SendString("LED2=OFF ");
            }

            if (GPIO_Output_Pin_Data_Get(LED3_GPIO_PORT, LED3_GPIO_PIN))
            {
                USART1_SendString("LED3=ON\r\n");
            }
            else
            {
                USART1_SendString("LED3=OFF\r\n");
            }
        }

        /* Delay to prevent busy loop */
        rt_thread_mdelay(50);
    }
}

/**
 * Main function
 * Called from RT-Thread main thread
 */
int main(void)
{
    /* Initialize RTT */
    SEGGER_RTT_Init();
    SEGGER_RTT_printf(0, "N32G430 RT-Thread Nano Started!\r\n");

    /* Initialize UART semaphore */
    rt_sem_init(&uart_rx_sem, "uart_rx", 0, RT_IPC_FLAG_FIFO);

    /* Create LED thread */
    rt_thread_init(&led_thread,
                   "led",
                   led_thread_entry,
                   RT_NULL,
                   &led_thread_stack[0],
                   LED_THREAD_STACK_SIZE,
                   LED_THREAD_PRIORITY,
                   LED_THREAD_TIMESLICE);
    rt_thread_startup(&led_thread);

    /* Create UART thread */
    rt_thread_init(&uart_thread,
                   "uart",
                   uart_thread_entry,
                   RT_NULL,
                   &uart_thread_stack[0],
                   UART_THREAD_STACK_SIZE,
                   UART_THREAD_PRIORITY,
                   UART_THREAD_TIMESLICE);
    rt_thread_startup(&uart_thread);

    rt_kprintf("All threads started!\n");

    /* Main thread can exit or do other tasks */
    while (1)
    {
        /* Keep alive message every 30 seconds */
        rt_thread_mdelay(30000);
        rt_kprintf("System running... tick=%d\n", rt_tick_get());
        USART1_SendString("System running...\r\n");
    }

    return 0;
}
