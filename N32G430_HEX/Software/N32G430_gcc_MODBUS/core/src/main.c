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
#include "bsp_usart.h"
#include "bsp_delay.h"

/* std include */
#include <stdio.h>
#include <string.h>

/*MODBUS include*/
// #include "modbus_slave.h"  // MODBUS功能已注释

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
    /* RTT初始化和banner */
    SEGGER_RTT_Init();

    /* ==================== MODBUS相关代码已注释 ==================== */
    // /* 初始化Modbus从机（内部会配置USART1 DMA） */
    // ModbusSlave_Init(MODBUS_SLAVE_ADDRESS_DEFAULT);

    // /* 预设一些寄存器值用于测试 */
    // ModbusSlave_SetHoldingRegister(0x0000, 0x1234);
    // ModbusSlave_SetHoldingRegister(0x0001, 0x5678);
    // ModbusSlave_SetHoldingRegister(0x0002, 0xABCD);
    // ModbusSlave_SetInputRegister(0x0000, 0x1111);
    // ModbusSlave_SetInputRegister(0x0001, 0x2222);
    // ModbusSlave_SetCoil(0x0000, true);
    // ModbusSlave_SetCoil(0x0001, false);
    // ModbusSlave_SetCoil(0x0002, true);
    /* ============================================================= */

    /* ==================== USART1 测试程序 ==================== */
    uint32_t counter = 0;
    char tx_buffer[64];

    /* 初始化USART1 */
    USART1_Init();

    /* 通过USART1发送欢迎信息 */
    USART1_SendString("\r\n================================\r\n");
    USART1_SendString("  N32G430 USART1 Test Program\r\n");
    USART1_SendString("  Baudrate: 115200\r\n");
    USART1_SendString("================================\r\n\r\n");

    while(1)
    {
        /* 每秒发送一次计数信息 */
        sprintf(tx_buffer, "Counter: %lu\r\n", counter++);
        USART1_SendString(tx_buffer);

        /* RTT也打印一份 */
        SEGGER_RTT_printf(0, "USART1 TX: %s", tx_buffer);

        /* 延时1秒 */
        SysTick_Delay_Ms(1000);

        /* 检查是否有接收数据 */
        if (USART_Flag_Status_Get(USART1, USART_FLAG_RXDNE) == SET)
        {
            uint8_t rx_data = USART1_ReceiveByte();

            /* 回显接收到的数据 */
            USART1_SendString("Received: ");
            USART1_SendByte(rx_data);
            USART1_SendString("\r\n");

            /* RTT打印接收信息 */
            SEGGER_RTT_printf(0, "USART1 RX: 0x%02X ('%c')\r\n", rx_data,
                             (rx_data >= 32 && rx_data < 127) ? rx_data : '.');
        }
    }
    /* ======================================================== */
}


