#include "modbus_master.h"

#include <string.h>

#include "SEGGER_RTT.h"
#include "bsp_delay.h"
#include "bsp_usart.h"

#define MODBUS_FUNC_READ_HOLDING_REGISTERS   0x03U
#define MODBUS_FUNC_WRITE_SINGLE_REGISTER    0x06U
#define MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS 0x10U
#define MODBUS_EXCEPTION_MASK                0x80U

#define MODBUS_MAX_REGS_PER_REQUEST          60U
#define MODBUS_RESPONSE_TIMEOUT_MS           50U
#define MODBUS_INTER_REQUEST_DELAY_MS        10U

#define MODBUS_FRAME_HEADER_SIZE             3U
#define MODBUS_FRAME_BUFFER_SIZE             (MODBUS_FRAME_HEADER_SIZE + (MODBUS_MAX_REGS_PER_REQUEST * 2U))

static uint8_t s_slaveAddress = MODBUS_SLAVE_ADDRESS_DEFAULT;
static uint16_t s_registerCache[MODBUS_REGISTER_COUNT];
static uint16_t s_nextRegister = MODBUS_REGISTER_START;

/* 写请求队列 */
static ModbusWriteRequest_t s_writeQueue[MODBUS_WRITE_QUEUE_SIZE];
static uint8_t s_writeQueueHead = 0;
static uint8_t s_writeQueueTail = 0;
static uint8_t s_writeQueueCount = 0;

static uint16_t ModbusMaster_CalcCRC(const uint8_t* data, uint16_t length);
static bool ModbusMaster_ReadBytes(uint8_t* buffer, uint16_t length);
static bool ModbusMaster_ReadHoldingRegisters(uint16_t startReg, uint16_t quantity, uint16_t* dest);
static bool ModbusMaster_ExecuteWriteSingle(uint16_t address, uint16_t value);
static bool ModbusMaster_ExecuteWriteMultiple(uint16_t address, uint16_t count, const uint16_t* values);
static bool ModbusMaster_ProcessWriteQueue(void);

void ModbusMaster_Init(uint8_t slaveAddress)
{
    s_slaveAddress = slaveAddress;
    s_nextRegister = MODBUS_REGISTER_START;
    memset(s_registerCache, 0, sizeof(s_registerCache));

    /* 初始化写队列 */
    s_writeQueueHead = 0;
    s_writeQueueTail = 0;
    s_writeQueueCount = 0;
    memset(s_writeQueue, 0, sizeof(s_writeQueue));

    USART2_Init();
    USART2_FlushRxBuffer();

    SEGGER_RTT_printf(0, "[Modbus] Master ready. Slave: 0x%02X, Range: 0x%04X-0x%04X\r\n",
                      s_slaveAddress, MODBUS_REGISTER_START, MODBUS_REGISTER_END);
}

void ModbusMaster_Task(void)
{
    /* 优先处理写请求队列 */
    if(s_writeQueueCount > 0)
    {
        ModbusMaster_ProcessWriteQueue();
        SysTick_Delay_Ms(MODBUS_INTER_REQUEST_DELAY_MS);
        return;
    }

    /* 定时轮询读取寄存器 */
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

    uint32_t timeout = MODBUS_RESPONSE_TIMEOUT_MS;
    uint16_t received = 0;

    while(received < length && timeout > 0)
    {
        uint16_t count = USART2_GetRxData(&buffer[received], length - received);
        if(count > 0)
        {
            received += count;
        }
        else
        {
            SysTick_Delay_Ms(1);
            timeout--;
        }
    }

    return (received == length);
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

/* 写单个寄存器实现 (功能码0x06) */
static bool ModbusMaster_ExecuteWriteSingle(uint16_t address, uint16_t value)
{
    uint8_t txFrame[8];
    txFrame[0] = s_slaveAddress;
    txFrame[1] = MODBUS_FUNC_WRITE_SINGLE_REGISTER;
    txFrame[2] = (uint8_t)(address >> 8);
    txFrame[3] = (uint8_t)(address & 0xFFU);
    txFrame[4] = (uint8_t)(value >> 8);
    txFrame[5] = (uint8_t)(value & 0xFFU);

    uint16_t crc = ModbusMaster_CalcCRC(txFrame, 6U);
    txFrame[6] = (uint8_t)(crc & 0xFFU);
    txFrame[7] = (uint8_t)(crc >> 8);

    USART2_FlushRxBuffer();
    USART2_SendArray(txFrame, sizeof(txFrame));

    /* 读取响应 - 写单个寄存器响应与请求帧相同 */
    uint8_t rxFrame[8];
    if(!ModbusMaster_ReadBytes(rxFrame, 8U))
    {
        SEGGER_RTT_printf(0, "[Modbus] Write single timeout\r\n");
        return false;
    }

    /* 验证从机地址 */
    if(rxFrame[0] != s_slaveAddress)
    {
        return false;
    }

    /* 检查异常响应 */
    if(rxFrame[1] == (MODBUS_FUNC_WRITE_SINGLE_REGISTER | MODBUS_EXCEPTION_MASK))
    {
        SEGGER_RTT_printf(0, "[Modbus] Write single exception: 0x%02X\r\n", rxFrame[2]);
        return false;
    }

    /* 验证功能码 */
    if(rxFrame[1] != MODBUS_FUNC_WRITE_SINGLE_REGISTER)
    {
        return false;
    }

    /* 验证CRC */
    uint16_t crcCalc = ModbusMaster_CalcCRC(rxFrame, 6U);
    uint16_t crcResp = (uint16_t)rxFrame[6] | ((uint16_t)rxFrame[7] << 8);

    if(crcCalc != crcResp)
    {
        SEGGER_RTT_printf(0, "[Modbus] Write single CRC error\r\n");
        return false;
    }

    /* 验证响应的地址和值 */
    uint16_t respAddr = ((uint16_t)rxFrame[2] << 8) | rxFrame[3];
    uint16_t respValue = ((uint16_t)rxFrame[4] << 8) | rxFrame[5];

    if(respAddr != address || respValue != value)
    {
        SEGGER_RTT_printf(0, "[Modbus] Write single echo mismatch\r\n");
        return false;
    }

    SEGGER_RTT_printf(0, "[Modbus] Write OK: 0x%04X = 0x%04X\r\n", address, value);
    return true;
}

/* 写多个寄存器实现 (功能码0x10) */
static bool ModbusMaster_ExecuteWriteMultiple(uint16_t address, uint16_t count, const uint16_t* values)
{
    if(values == NULL || count == 0U || count > MODBUS_MAX_WRITE_REGS)
    {
        return false;
    }

    uint8_t txFrame[7 + MODBUS_MAX_WRITE_REGS * 2];
    txFrame[0] = s_slaveAddress;
    txFrame[1] = MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS;
    txFrame[2] = (uint8_t)(address >> 8);
    txFrame[3] = (uint8_t)(address & 0xFFU);
    txFrame[4] = (uint8_t)(count >> 8);
    txFrame[5] = (uint8_t)(count & 0xFFU);
    txFrame[6] = (uint8_t)(count * 2U);

    /* 填充数据 */
    for(uint16_t i = 0; i < count; i++)
    {
        txFrame[7 + i * 2] = (uint8_t)(values[i] >> 8);
        txFrame[7 + i * 2 + 1] = (uint8_t)(values[i] & 0xFFU);
    }

    uint16_t frameLen = 7U + count * 2U;
    uint16_t crc = ModbusMaster_CalcCRC(txFrame, frameLen);
    txFrame[frameLen] = (uint8_t)(crc & 0xFFU);
    txFrame[frameLen + 1] = (uint8_t)(crc >> 8);

    USART2_FlushRxBuffer();
    USART2_SendArray(txFrame, frameLen + 2U);

    /* 读取响应 - 写多个寄存器响应为8字节 */
    uint8_t rxFrame[8];
    if(!ModbusMaster_ReadBytes(rxFrame, 8U))
    {
        SEGGER_RTT_printf(0, "[Modbus] Write multiple timeout\r\n");
        return false;
    }

    /* 验证从机地址 */
    if(rxFrame[0] != s_slaveAddress)
    {
        return false;
    }

    /* 检查异常响应 */
    if(rxFrame[1] == (MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS | MODBUS_EXCEPTION_MASK))
    {
        SEGGER_RTT_printf(0, "[Modbus] Write multiple exception: 0x%02X\r\n", rxFrame[2]);
        return false;
    }

    /* 验证功能码 */
    if(rxFrame[1] != MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS)
    {
        return false;
    }

    /* 验证CRC */
    uint16_t crcCalc = ModbusMaster_CalcCRC(rxFrame, 6U);
    uint16_t crcResp = (uint16_t)rxFrame[6] | ((uint16_t)rxFrame[7] << 8);

    if(crcCalc != crcResp)
    {
        SEGGER_RTT_printf(0, "[Modbus] Write multiple CRC error\r\n");
        return false;
    }

    /* 验证响应的地址和数量 */
    uint16_t respAddr = ((uint16_t)rxFrame[2] << 8) | rxFrame[3];
    uint16_t respCount = ((uint16_t)rxFrame[4] << 8) | rxFrame[5];

    if(respAddr != address || respCount != count)
    {
        SEGGER_RTT_printf(0, "[Modbus] Write multiple echo mismatch\r\n");
        return false;
    }

    SEGGER_RTT_printf(0, "[Modbus] Write OK: 0x%04X (%u regs)\r\n", address, count);
    return true;
}

/* 处理写请求队列 */
static bool ModbusMaster_ProcessWriteQueue(void)
{
    if(s_writeQueueCount == 0)
    {
        return false;
    }

    ModbusWriteRequest_t* req = &s_writeQueue[s_writeQueueHead];
    bool result = false;

    if(req->type == MODBUS_WRITE_SINGLE)
    {
        result = ModbusMaster_ExecuteWriteSingle(req->address, req->values[0]);
    }
    else if(req->type == MODBUS_WRITE_MULTIPLE)
    {
        result = ModbusMaster_ExecuteWriteMultiple(req->address, req->count, req->values);
    }

    /* 从队列移除 */
    s_writeQueueHead = (s_writeQueueHead + 1U) % MODBUS_WRITE_QUEUE_SIZE;
    s_writeQueueCount--;

    return result;
}

/* 公开API: 写单个寄存器 (添加到队列) */
bool ModbusMaster_WriteSingleRegister(uint16_t address, uint16_t value)
{
    if(s_writeQueueCount >= MODBUS_WRITE_QUEUE_SIZE)
    {
        SEGGER_RTT_printf(0, "[Modbus] Write queue full\r\n");
        return false;
    }

    ModbusWriteRequest_t* req = &s_writeQueue[s_writeQueueTail];
    req->type = MODBUS_WRITE_SINGLE;
    req->address = address;
    req->count = 1U;
    req->values[0] = value;

    s_writeQueueTail = (s_writeQueueTail + 1U) % MODBUS_WRITE_QUEUE_SIZE;
    s_writeQueueCount++;

    SEGGER_RTT_printf(0, "[Modbus] Queued write single: 0x%04X = 0x%04X\r\n", address, value);
    return true;
}

/* 公开API: 写多个寄存器 (添加到队列) */
bool ModbusMaster_WriteMultipleRegisters(uint16_t address, uint16_t count, const uint16_t* values)
{
    if(values == NULL || count == 0U || count > MODBUS_MAX_WRITE_REGS)
    {
        return false;
    }

    if(s_writeQueueCount >= MODBUS_WRITE_QUEUE_SIZE)
    {
        SEGGER_RTT_printf(0, "[Modbus] Write queue full\r\n");
        return false;
    }

    ModbusWriteRequest_t* req = &s_writeQueue[s_writeQueueTail];
    req->type = MODBUS_WRITE_MULTIPLE;
    req->address = address;
    req->count = count;
    memcpy(req->values, values, count * sizeof(uint16_t));

    s_writeQueueTail = (s_writeQueueTail + 1U) % MODBUS_WRITE_QUEUE_SIZE;
    s_writeQueueCount++;

    SEGGER_RTT_printf(0, "[Modbus] Queued write multiple: 0x%04X (%u regs)\r\n", address, count);
    return true;
}

/* 公开API: 获取写队列当前数量 */
uint8_t ModbusMaster_GetWriteQueueCount(void)
{
    return s_writeQueueCount;
}
