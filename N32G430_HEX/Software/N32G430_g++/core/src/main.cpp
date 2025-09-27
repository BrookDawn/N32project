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
*\*\file main.cpp
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/

/*BSP include*/
#include "main.h"
#include "bsp_delay.h"
#include "bsp_usart.h"

/*APP include*/
#include "Buffer.h"
#include "Led.h"
#include "LedAdapter_N32G430.h"

/*RTT include*/
#include "SEGGER_RTT.h"

/**
 *\*\name   main.
 *\*\fun    main function.
 *\*\param  none.
 *\*\return none.
**/
int main(void)
{
	uint32_t led_counter = 0;
	uint32_t system_tick = 0;

	/*RTT初始化*/
	SEGGER_RTT_Init();
	SEGGER_RTT_printf(0, "N32G430 C++ LED System Test Started!\r\n");
	SEGGER_RTT_printf(0, "Baudrate: 115200\r\n");
	SEGGER_RTT_printf(0, "LED Class System Initialized...\r\n");

	/* 初始化USART1 */
	USART1_Init();
	
	/* 初始化LED系统 */
	if (LedAdapter_N32G430_InitAllLeds() == LED_OK) 
	{
		SEGGER_RTT_printf(0, "LED System initialized successfully!\r\n");
		
		// 配置LED闪烁
		Led_SetBlinkPeriod(LED_ID_STATUS, 	LED_BLINK_SLOW);   		// 状态LED慢闪
		Led_SetBlinkPeriod(LED_ID_ERROR, 	LED_BLINK_FAST);    	// 错误LED快闪
		Led_SetBlinkPeriod(LED_ID_COMM, 	LED_BLINK_NORMAL);   	// 通信LED正常闪烁
		
		// 开启状态LED
		Led_On(LED_ID_STATUS);
	} else {
		SEGGER_RTT_printf(0, "LED System initialization failed!\r\n");
	}

	while(1)
	{
		system_tick++;
		
		// 更新LED闪烁状态（每10ms调用一次）
		if ((system_tick % 10) == 0)
		{
			uint32_t current_time = system_tick;
			Led_BlinkUpdate(LED_ID_STATUS, 	current_time);
			Led_BlinkUpdate(LED_ID_ERROR, 	current_time);
			Led_BlinkUpdate(LED_ID_COMM, 	current_time);
		}

		/* 每2秒发送一次状态信息并演示LED控制 */
		if(led_counter >= 2000)  // 2秒计数
		{
			SEGGER_RTT_printf(0,"System running... LED demo...\r\n");
			
			// LED控制演示
			static uint8_t demo_step = 0;
			switch(demo_step) {
				case 0:
					Led_On(LED_ID_1);
					Led_Off(LED_ID_2);
					Led_Off(LED_ID_3);
					SEGGER_RTT_printf(0,"LED1 ON\r\n");
					break;
				case 1:
					Led_Off(LED_ID_1);
					Led_On(LED_ID_2);
					Led_Off(LED_ID_3);
					SEGGER_RTT_printf(0,"LED2 ON\r\n");
					break;
				case 2:
					Led_Off(LED_ID_1);
					Led_Off(LED_ID_2);
					Led_On(LED_ID_3);
					SEGGER_RTT_printf(0,"LED3 ON\r\n");
					break;
				case 3:
					Led_Toggle(LED_ID_1);
					Led_Toggle(LED_ID_2);
					Led_Toggle(LED_ID_3);
					SEGGER_RTT_printf(0,"All LEDs TOGGLE\r\n");
					break;
			}
			
			demo_step = (demo_step + 1) % 4;
			led_counter = 0;
		}
		
		// 简单延时（实际项目中应该使用定时器）
		for(volatile uint32_t i = 0; i < 10000; i++);
		led_counter++;
	}
}



