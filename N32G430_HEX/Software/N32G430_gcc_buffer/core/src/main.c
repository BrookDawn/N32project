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

/*BSP include*/
#include "main.h"
#include "bsp_led.h"
#include "bsp_delay.h"
#include "bsp_usart.h"

/*APP include*/
#include "Buffer.h"

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

	/*RTT初始化*/
	SEGGER_RTT_Init();
	SEGGER_RTT_printf(0, "N32G430 USART1 Test Started!\r\n");
	SEGGER_RTT_printf(0, "Baudrate: 115200\r\n");
	SEGGER_RTT_printf(0, "Send any character to echo back...\r\n");

	/* 初始化USART1 */
	USART1_Init();


	while(1)
	{
		

		/* 每2秒发送一次状态信息 */
		led_counter++;
		if(led_counter >= 2)
		{
			USART1_SendString("System running... LED blinking...\r\n");
			SEGGER_RTT_printf(0,"System running... LED blinking...\r\n");
			led_counter = 0;
		}
	}
}


