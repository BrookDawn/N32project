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
#include "bsp_delay.h"
#include "bsp_usart.h"

/**
 *\*\name   main.
 *\*\fun    main function.
 *\*\param  none.
 *\*\return none.
**/
int main(void)
{
	char rx_buffer[128];
	uint16_t rx_length;
	uint32_t led_counter = 0;

	/* 初始化USART1 */
	USART1_Init();

	/* 发送启动信息 */
	USART1_SendString("N32G430 USART1 Test Started!\r\n");
	USART1_SendString("Baudrate: 115200\r\n");
	USART1_SendString("Send any character to echo back...\r\n");

	/* Initialize Led1~Led3 as output push-pull mode */
	LED_Initialize(LED1_GPIO_PORT, LED1_GPIO_PIN, LED1_GPIO_CLK);
	LED_Initialize(LED2_GPIO_PORT, LED2_GPIO_PIN | LED3_GPIO_PIN, LED2_GPIO_CLK);

	/* Turn off Led1~Led3 */
	LED_Off(LED2_GPIO_PORT, LED1_GPIO_PIN | LED2_GPIO_PIN | LED3_GPIO_PIN);

	/* Turn on Led2~Led3 */
	LED_On(LED2_GPIO_PORT, LED2_GPIO_PIN | LED3_GPIO_PIN);

	/* Delay 1s */
	SysTick_Delay_Ms(1000);

	while(1)
	{
		/* 检查是否有串口数据 */
		rx_length = USART1_ReceiveString(rx_buffer, sizeof(rx_buffer));
		if(rx_length > 0)
		{
			/* 回显接收到的数据 */
			USART1_SendString("Received: ");
			USART1_SendArray((uint8_t*)rx_buffer, rx_length);
			USART1_SendString("\r\n");

			/* 发送LED状态信息 */
			USART1_SendString("LED Status: ");
			if(GPIO_Output_Pin_Data_Get(LED1_GPIO_PORT, LED1_GPIO_PIN))
			{
				USART1_SendString("LED1=ON ");
			}
			else
			{
				USART1_SendString("LED1=OFF ");
			}

			if(GPIO_Output_Pin_Data_Get(LED2_GPIO_PORT, LED2_GPIO_PIN))
			{
				USART1_SendString("LED2=ON ");
			}
			else
			{
				USART1_SendString("LED2=OFF ");
			}

			if(GPIO_Output_Pin_Data_Get(LED3_GPIO_PORT, LED3_GPIO_PIN))
			{
				USART1_SendString("LED3=ON\r\n");
			}
			else
			{
				USART1_SendString("LED3=OFF\r\n");
			}
		}

		/* Turn on Led1 */
		LED1_ON;

		/* Toggle LED2 */
		LED_Toggle(LED2_GPIO_PORT, LED2_GPIO_PIN);

		/* Delay 1s */
		SysTick_Delay_Ms(1000);

		/* Toggle LED3 */
		LED_Toggle(LED3_GPIO_PORT, LED3_GPIO_PIN);

		/* Delay 1s */
		SysTick_Delay_Ms(1000);

		/* Turn off LED1 */
		LED1_OFF;

		/* Delay 1s */
		SysTick_Delay_Ms(1000);

		/* 每10秒发送一次状态信息 */
		led_counter++;
		if(led_counter >= 10)
		{
			USART1_SendString("System running... LED blinking...\r\n");
			led_counter = 0;
		}
	}
}


