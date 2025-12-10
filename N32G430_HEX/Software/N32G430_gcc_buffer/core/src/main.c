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
#include "modbus_slave.h"

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
    SEGGER_RTT_printf(0, "N32G430 Modbus Slave Started\r\n");
    SEGGER_RTT_printf(0, "USART1 RTU slave, Address: 0x01\r\n");

    /* 初始化Modbus从机（内部会配置USART1 DMA） */
    ModbusSlave_Init(MODBUS_SLAVE_ADDRESS_DEFAULT);

    /* 预设一些寄存器值用于测试 */
    ModbusSlave_SetHoldingRegister(0x0000, 0x1234);
    ModbusSlave_SetHoldingRegister(0x0001, 0x5678);
    ModbusSlave_SetHoldingRegister(0x0002, 0xABCD);
    ModbusSlave_SetInputRegister(0x0000, 0x1111);
    ModbusSlave_SetInputRegister(0x0001, 0x2222);
    ModbusSlave_SetCoil(0x0000, true);
    ModbusSlave_SetCoil(0x0001, false);
    ModbusSlave_SetCoil(0x0002, true);

    while(1)
    {
        ModbusSlave_Task();
    }
}


