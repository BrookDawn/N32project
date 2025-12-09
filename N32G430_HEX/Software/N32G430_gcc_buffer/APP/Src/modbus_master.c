#include "modbus_master.h"

#include <string.h>

#include "SEGGER_RTT.h"
#include "bsp_delay.h"
#include "bsp_usart.h"

#define MODBUS_FUNC_READ_HOLDING_REGISTERS   0x03U
#define MODBUS_EXCEPTION_MASK                0x80U

#define MODBUS_MAX_REGS_PER_REQUEST          60U
#define MODBUS_RESPONSE_TIMEOUT_MS           50U
#define MODBUS_INTER_REQUEST_DELAY_MS        10U

#define MODBUS_FRAME_HEADER_SIZE             3U
#define MODBUS_FRAME_BUFFER_SIZE             (MODBUS_FRAME_HEADER_SIZE + (MODBUS_MAX_REGS_PER_REQUEST * 2U))

static uint8_t s_slaveAddress = MODBUS_SLAVE_ADDRESS_DEFAULT;
static uint16_t s_registerCache[MODBUS_REGISTER_COUNT];
static uint16_t s_nextRegister = MODBUS_REGISTER_START;

static uint16_t ModbusMaster_CalcCRC(const uint8_t* data, uint16_t length);
static bool ModbusMaster_ReadBytes(uint8_t* buffer, uint16_t length);
static bool ModbusMaster_ReadHoldingRegisters(uint16_t startReg, uint16_t quantity, uint16_t* dest);

void ModbusMaster_Init(uint8_t slaveAddress)
{
    s_slaveAddress = slaveAddress;
    s_nextRegister = MODBUS_REGISTER_START;
    memset(s_registerCache, 0, sizeof(s_registerCache));

    USART2_Init();
    USART2_FlushRxBuffer();

    SEGGER_RTT_printf(0, "[Modbus] Master ready. Slave: 0x%02X, Range: 0x%04X-0x%04X\r\n",
                      s_slaveAddress, MODBUS_REGISTER_START, MODBUS_REGISTER_END);
}

void ModbusMaster_Task(void)
{
    uint16_t start = s_nextRegister;
    uint16_t remaining = (MODBUS_REGISTER_END - start) + 1U;
    uint16_t quantity = (remaining > MODBUS_MAX_REGS_PER_REQUEST) ? MODBUS_MAX_REGS_PER_REQUEST : remaining;

    if(ModbusMaster_ReadHoldingRegisters(start, quantity, &s_registerCache[start - MODBUS_REGISTER_START]))
    {
        uint16_t endAddr = start + quantity - 1U;
        SEGGER_RTT_printf(0, "[Modbus] Read OK: 0x%04X-0x%04X (%u regs)\r\n", start, endAddr, quantity);

        s_nextRegister = (endAddr >= MODBUS_REGISTER_END) ? MODBUS_REGISTER_START : (uint16_t)(endAddr + 1U);
    }
    else
    {
        SEGGER_RTT_printf(0, "[Modbus] Read failed at 0x%04X (%u regs)\r\n", start, quantity);
    }

    SysTick_Delay_Ms(MODBUS_INTER_REQUEST_DELAY_MS);
}

const uint16_t* ModbusMaster_GetRegisterCache(void)
{
    return s_registerCache;
}

bool ModbusMaster_GetRegister(uint16_t address, uint16_t* value)
{
    if(value == NULL || address < MODBUS_REGISTER_START || address > MODBUS_REGISTER_END)
    {
        return false;
    }

    *value = s_registerCache[address - MODBUS_REGISTER_START];
    return true;
}

static bool ModbusMaster_ReadHoldingRegisters(uint16_t startReg, uint16_t quantity, uint16_t* dest)
{
    if(dest == NULL || quantity == 0U || quantity > MODBUS_MAX_REGS_PER_REQUEST)
    {
        return false;
    }

    uint8_t txFrame[8];
    txFrame[0] = s_slaveAddress;
    txFrame[1] = MODBUS_FUNC_READ_HOLDING_REGISTERS;
    txFrame[2] = (uint8_t)(startReg >> 8);
    txFrame[3] = (uint8_t)(startReg & 0xFFU);
    txFrame[4] = (uint8_t)(quantity >> 8);
    txFrame[5] = (uint8_t)(quantity & 0xFFU);

    uint16_t crc = ModbusMaster_CalcCRC(txFrame, 6U);
    txFrame[6] = (uint8_t)(crc & 0xFFU);
    txFrame[7] = (uint8_t)(crc >> 8);

    USART2_FlushRxBuffer();
    USART2_SendArray(txFrame, sizeof(txFrame));

    uint8_t rxFrame[MODBUS_FRAME_BUFFER_SIZE] = {0};
    if(!ModbusMaster_ReadBytes(rxFrame, MODBUS_FRAME_HEADER_SIZE))
    {
        return false;
    }

    if(rxFrame[0] != s_slaveAddress)
    {
        return false;
    }

    if(rxFrame[1] == (MODBUS_FUNC_READ_HOLDING_REGISTERS | MODBUS_EXCEPTION_MASK))
    {
        uint8_t exception[1];
        if(!ModbusMaster_ReadBytes(exception, 1U))
        {
            return false;
        }

        uint8_t crcBytes[2];
        if(!ModbusMaster_ReadBytes(crcBytes, 2U))
        {
            return false;
        }

        uint8_t exceptionFrame[MODBUS_FRAME_HEADER_SIZE];
        exceptionFrame[0] = rxFrame[0];
        exceptionFrame[1] = rxFrame[1];
        exceptionFrame[2] = exception[0];

        uint16_t crcCalc = ModbusMaster_CalcCRC(exceptionFrame, MODBUS_FRAME_HEADER_SIZE);
        uint16_t crcResp = (uint16_t)crcBytes[0] | ((uint16_t)crcBytes[1] << 8);

        if(crcCalc == crcResp)
        {
            SEGGER_RTT_printf(0, "[Modbus] Exception code: 0x%02X\r\n", exception[0]);
        }

        return false;
    }

    if(rxFrame[1] != MODBUS_FUNC_READ_HOLDING_REGISTERS)
    {
        return false;
    }

    uint8_t byteCount = rxFrame[2];
    if(byteCount != (quantity * 2U))
    {
        return false;
    }

    if(!ModbusMaster_ReadBytes(&rxFrame[MODBUS_FRAME_HEADER_SIZE], byteCount))
    {
        return false;
    }

    uint8_t crcBytes[2];
    if(!ModbusMaster_ReadBytes(crcBytes, 2U))
    {
        return false;
    }

    uint16_t crcCalc = ModbusMaster_CalcCRC(rxFrame, MODBUS_FRAME_HEADER_SIZE + byteCount);
    uint16_t crcResp = (uint16_t)crcBytes[0] | ((uint16_t)crcBytes[1] << 8);

    if(crcCalc != crcResp)
    {
        SEGGER_RTT_printf(0, "[Modbus] CRC error. Calc:0x%04X Resp:0x%04X\r\n", crcCalc, crcResp);
        return false;
    }

    for(uint16_t i = 0; i < quantity; i++)
    {
        uint16_t index = MODBUS_FRAME_HEADER_SIZE + (i * 2U);
        dest[i] = ((uint16_t)rxFrame[index] << 8) | rxFrame[index + 1U];
    }

    return true;
}

static bool ModbusMaster_ReadBytes(uint8_t* buffer, uint16_t length)
{
    if(buffer == NULL)
    {
        return false;
    }

    for(uint16_t i = 0; i < length; i++)
    {
        if(!USART2_ReadByteTimeout(&buffer[i], MODBUS_RESPONSE_TIMEOUT_MS))
        {
            return false;
        }
    }

    return true;
}

static uint16_t ModbusMaster_CalcCRC(const uint8_t* data, uint16_t length)
{
    uint16_t crc = 0xFFFFU;

    for(uint16_t pos = 0; pos < length; pos++)
    {
        crc ^= data[pos];
        for(uint8_t i = 0; i < 8U; i++)
        {
            if((crc & 0x0001U) != 0U)
            {
                crc = (crc >> 1U) ^ 0xA001U;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc;
}
