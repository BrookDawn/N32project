/**
 * @file modbus_slave.c
 * @brief Modbus RTU从机协议实现 (使用USART1 DMA中断方式)
 * @version 1.0.0
 */

#include "modbus_slave.h"
#include "bsp_usart.h"
#include "elab_log.h"
#include <string.h>

/* 从机地址 */
static uint8_t s_slaveAddress = MODBUS_SLAVE_ADDRESS_DEFAULT;

/* 数据存储区 */
static uint16_t s_holdingRegisters[MODBUS_SLAVE_HOLDING_REG_COUNT];
static uint16_t s_inputRegisters[MODBUS_SLAVE_INPUT_REG_COUNT];
static uint8_t s_coils[(MODBUS_SLAVE_COIL_COUNT + 7) / 8];
static uint8_t s_discreteInputs[(MODBUS_SLAVE_DISCRETE_COUNT + 7) / 8];

/* 接收和发送缓冲区 */
static uint8_t s_rxBuffer[MODBUS_SLAVE_RX_BUFFER_SIZE];
static uint8_t s_txBuffer[MODBUS_SLAVE_TX_BUFFER_SIZE];

/* 从机状态 */
static ModbusSlaveState_t s_state = MODBUS_SLAVE_IDLE;

/* 统计信息 */
static uint32_t s_rxFrameCount = 0;
static uint32_t s_txFrameCount = 0;
static uint32_t s_errorCount = 0;

/* 回调函数 */
static ModbusSlaveWriteCallback_t s_writeCallback = NULL;

/* CRC16计算 */
static uint16_t ModbusSlave_CalcCRC(const uint8_t* data, uint16_t length)
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

/* 发送异常响应 */
static void ModbusSlave_SendException(uint8_t functionCode, uint8_t exceptionCode)
{
    s_txBuffer[0] = s_slaveAddress;
    s_txBuffer[1] = functionCode | 0x80U;
    s_txBuffer[2] = exceptionCode;

    uint16_t crc = ModbusSlave_CalcCRC(s_txBuffer, 3U);
    s_txBuffer[3] = (uint8_t)(crc & 0xFFU);
    s_txBuffer[4] = (uint8_t)(crc >> 8);

    USART1_SendArray_DMA(s_txBuffer, 5U);
    s_txFrameCount++;
}

/* 发送响应帧 */
static void ModbusSlave_SendResponse(uint16_t length)
{
    uint16_t crc = ModbusSlave_CalcCRC(s_txBuffer, length);
    s_txBuffer[length] = (uint8_t)(crc & 0xFFU);
    s_txBuffer[length + 1] = (uint8_t)(crc >> 8);

    USART1_SendArray_DMA(s_txBuffer, length + 2U);
    s_txFrameCount++;
}

/* 处理读线圈 (0x01) */
static void ModbusSlave_HandleReadCoils(const uint8_t* request, uint16_t length)
{
    (void)length;

    uint16_t startAddr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t quantity = ((uint16_t)request[4] << 8) | request[5];

    if(quantity == 0 || quantity > 2000)
    {
        ModbusSlave_SendException(MODBUS_FC_READ_COILS, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(startAddr < MODBUS_SLAVE_COIL_START ||
       (startAddr + quantity) > (MODBUS_SLAVE_COIL_START + MODBUS_SLAVE_COIL_COUNT))
    {
        ModbusSlave_SendException(MODBUS_FC_READ_COILS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    uint8_t byteCount = (uint8_t)((quantity + 7U) / 8U);

    s_txBuffer[0] = s_slaveAddress;
    s_txBuffer[1] = MODBUS_FC_READ_COILS;
    s_txBuffer[2] = byteCount;

    memset(&s_txBuffer[3], 0, byteCount);

    for(uint16_t i = 0; i < quantity; i++)
    {
        uint16_t coilAddr = startAddr - MODBUS_SLAVE_COIL_START + i;
        uint8_t byteIndex = coilAddr / 8U;
        uint8_t bitIndex = coilAddr % 8U;

        if(s_coils[byteIndex] & (1U << bitIndex))
        {
            uint8_t respByteIndex = i / 8U;
            uint8_t respBitIndex = i % 8U;
            s_txBuffer[3 + respByteIndex] |= (1U << respBitIndex);
        }
    }

    ModbusSlave_SendResponse(3U + byteCount);
}

/* 处理读离散输入 (0x02) */
static void ModbusSlave_HandleReadDiscreteInputs(const uint8_t* request, uint16_t length)
{
    (void)length;

    uint16_t startAddr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t quantity = ((uint16_t)request[4] << 8) | request[5];

    if(quantity == 0 || quantity > 2000)
    {
        ModbusSlave_SendException(MODBUS_FC_READ_DISCRETE_INPUTS, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(startAddr < MODBUS_SLAVE_DISCRETE_START ||
       (startAddr + quantity) > (MODBUS_SLAVE_DISCRETE_START + MODBUS_SLAVE_DISCRETE_COUNT))
    {
        ModbusSlave_SendException(MODBUS_FC_READ_DISCRETE_INPUTS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    uint8_t byteCount = (uint8_t)((quantity + 7U) / 8U);

    s_txBuffer[0] = s_slaveAddress;
    s_txBuffer[1] = MODBUS_FC_READ_DISCRETE_INPUTS;
    s_txBuffer[2] = byteCount;

    memset(&s_txBuffer[3], 0, byteCount);

    for(uint16_t i = 0; i < quantity; i++)
    {
        uint16_t addr = startAddr - MODBUS_SLAVE_DISCRETE_START + i;
        uint8_t byteIndex = addr / 8U;
        uint8_t bitIndex = addr % 8U;

        if(s_discreteInputs[byteIndex] & (1U << bitIndex))
        {
            uint8_t respByteIndex = i / 8U;
            uint8_t respBitIndex = i % 8U;
            s_txBuffer[3 + respByteIndex] |= (1U << respBitIndex);
        }
    }

    ModbusSlave_SendResponse(3U + byteCount);
}

/* 处理读保持寄存器 (0x03) */
static void ModbusSlave_HandleReadHoldingRegisters(const uint8_t* request, uint16_t length)
{
    (void)length;

    uint16_t startAddr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t quantity = ((uint16_t)request[4] << 8) | request[5];

    if(quantity == 0 || quantity > 125)
    {
        ModbusSlave_SendException(MODBUS_FC_READ_HOLDING_REGISTERS, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(startAddr < MODBUS_SLAVE_HOLDING_REG_START ||
       (startAddr + quantity) > (MODBUS_SLAVE_HOLDING_REG_START + MODBUS_SLAVE_HOLDING_REG_COUNT))
    {
        ModbusSlave_SendException(MODBUS_FC_READ_HOLDING_REGISTERS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    uint8_t byteCount = (uint8_t)(quantity * 2U);

    s_txBuffer[0] = s_slaveAddress;
    s_txBuffer[1] = MODBUS_FC_READ_HOLDING_REGISTERS;
    s_txBuffer[2] = byteCount;

    for(uint16_t i = 0; i < quantity; i++)
    {
        uint16_t regAddr = startAddr - MODBUS_SLAVE_HOLDING_REG_START + i;
        uint16_t value = s_holdingRegisters[regAddr];
        s_txBuffer[3 + i * 2] = (uint8_t)(value >> 8);
        s_txBuffer[3 + i * 2 + 1] = (uint8_t)(value & 0xFFU);
    }

    ModbusSlave_SendResponse(3U + byteCount);
}

/* 处理读输入寄存器 (0x04) */
static void ModbusSlave_HandleReadInputRegisters(const uint8_t* request, uint16_t length)
{
    (void)length;

    uint16_t startAddr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t quantity = ((uint16_t)request[4] << 8) | request[5];

    if(quantity == 0 || quantity > 125)
    {
        ModbusSlave_SendException(MODBUS_FC_READ_INPUT_REGISTERS, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(startAddr < MODBUS_SLAVE_INPUT_REG_START ||
       (startAddr + quantity) > (MODBUS_SLAVE_INPUT_REG_START + MODBUS_SLAVE_INPUT_REG_COUNT))
    {
        ModbusSlave_SendException(MODBUS_FC_READ_INPUT_REGISTERS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    uint8_t byteCount = (uint8_t)(quantity * 2U);

    s_txBuffer[0] = s_slaveAddress;
    s_txBuffer[1] = MODBUS_FC_READ_INPUT_REGISTERS;
    s_txBuffer[2] = byteCount;

    for(uint16_t i = 0; i < quantity; i++)
    {
        uint16_t regAddr = startAddr - MODBUS_SLAVE_INPUT_REG_START + i;
        uint16_t value = s_inputRegisters[regAddr];
        s_txBuffer[3 + i * 2] = (uint8_t)(value >> 8);
        s_txBuffer[3 + i * 2 + 1] = (uint8_t)(value & 0xFFU);
    }

    ModbusSlave_SendResponse(3U + byteCount);
}

/* 处理写单个线圈 (0x05) */
static void ModbusSlave_HandleWriteSingleCoil(const uint8_t* request, uint16_t length)
{
    (void)length;

    uint16_t coilAddr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t value = ((uint16_t)request[4] << 8) | request[5];

    if(value != 0x0000U && value != 0xFF00U)
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_SINGLE_COIL, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(coilAddr < MODBUS_SLAVE_COIL_START ||
       coilAddr >= (MODBUS_SLAVE_COIL_START + MODBUS_SLAVE_COIL_COUNT))
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_SINGLE_COIL, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    uint16_t index = coilAddr - MODBUS_SLAVE_COIL_START;
    uint8_t byteIndex = index / 8U;
    uint8_t bitIndex = index % 8U;

    if(value == 0xFF00U)
    {
        s_coils[byteIndex] |= (1U << bitIndex);
    }
    else
    {
        s_coils[byteIndex] &= ~(1U << bitIndex);
    }

    memcpy(s_txBuffer, request, 6U);
    s_txBuffer[0] = s_slaveAddress;
    ModbusSlave_SendResponse(6U);
}

/* 处理写单个寄存器 (0x06) */
static void ModbusSlave_HandleWriteSingleRegister(const uint8_t* request, uint16_t length)
{
    (void)length;

    uint16_t regAddr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t value = ((uint16_t)request[4] << 8) | request[5];

    if(regAddr < MODBUS_SLAVE_HOLDING_REG_START ||
       regAddr >= (MODBUS_SLAVE_HOLDING_REG_START + MODBUS_SLAVE_HOLDING_REG_COUNT))
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_SINGLE_REGISTER, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    uint16_t index = regAddr - MODBUS_SLAVE_HOLDING_REG_START;
    s_holdingRegisters[index] = value;

    if(s_writeCallback != NULL)
    {
        s_writeCallback(regAddr, value);
    }

    memcpy(s_txBuffer, request, 6U);
    s_txBuffer[0] = s_slaveAddress;
    ModbusSlave_SendResponse(6U);
}

/* 处理写多个线圈 (0x0F) */
static void ModbusSlave_HandleWriteMultipleCoils(const uint8_t* request, uint16_t length)
{
    uint16_t startAddr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t quantity = ((uint16_t)request[4] << 8) | request[5];
    uint8_t byteCount = request[6];

    if(quantity == 0 || quantity > 1968)
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_MULTIPLE_COILS, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(byteCount != ((quantity + 7U) / 8U))
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_MULTIPLE_COILS, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(length < (7U + byteCount + 2U))
    {
        s_errorCount++;
        return;
    }

    if(startAddr < MODBUS_SLAVE_COIL_START ||
       (startAddr + quantity) > (MODBUS_SLAVE_COIL_START + MODBUS_SLAVE_COIL_COUNT))
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_MULTIPLE_COILS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for(uint16_t i = 0; i < quantity; i++)
    {
        uint16_t index = startAddr - MODBUS_SLAVE_COIL_START + i;
        uint8_t srcByteIndex = i / 8U;
        uint8_t srcBitIndex = i % 8U;
        uint8_t dstByteIndex = index / 8U;
        uint8_t dstBitIndex = index % 8U;

        if(request[7 + srcByteIndex] & (1U << srcBitIndex))
        {
            s_coils[dstByteIndex] |= (1U << dstBitIndex);
        }
        else
        {
            s_coils[dstByteIndex] &= ~(1U << dstBitIndex);
        }
    }

    s_txBuffer[0] = s_slaveAddress;
    s_txBuffer[1] = MODBUS_FC_WRITE_MULTIPLE_COILS;
    s_txBuffer[2] = (uint8_t)(startAddr >> 8);
    s_txBuffer[3] = (uint8_t)(startAddr & 0xFFU);
    s_txBuffer[4] = (uint8_t)(quantity >> 8);
    s_txBuffer[5] = (uint8_t)(quantity & 0xFFU);

    ModbusSlave_SendResponse(6U);
}

/* 处理写多个寄存器 (0x10) */
static void ModbusSlave_HandleWriteMultipleRegisters(const uint8_t* request, uint16_t length)
{
    uint16_t startAddr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t quantity = ((uint16_t)request[4] << 8) | request[5];
    uint8_t byteCount = request[6];

    if(quantity == 0 || quantity > 123)
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_MULTIPLE_REGISTERS, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(byteCount != (quantity * 2U))
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_MULTIPLE_REGISTERS, MODBUS_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if(length < (7U + byteCount + 2U))
    {
        s_errorCount++;
        return;
    }

    if(startAddr < MODBUS_SLAVE_HOLDING_REG_START ||
       (startAddr + quantity) > (MODBUS_SLAVE_HOLDING_REG_START + MODBUS_SLAVE_HOLDING_REG_COUNT))
    {
        ModbusSlave_SendException(MODBUS_FC_WRITE_MULTIPLE_REGISTERS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for(uint16_t i = 0; i < quantity; i++)
    {
        uint16_t index = startAddr - MODBUS_SLAVE_HOLDING_REG_START + i;
        uint16_t value = ((uint16_t)request[7 + i * 2] << 8) | request[7 + i * 2 + 1];
        s_holdingRegisters[index] = value;

        if(s_writeCallback != NULL)
        {
            s_writeCallback(startAddr + i, value);
        }
    }

    s_txBuffer[0] = s_slaveAddress;
    s_txBuffer[1] = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    s_txBuffer[2] = (uint8_t)(startAddr >> 8);
    s_txBuffer[3] = (uint8_t)(startAddr & 0xFFU);
    s_txBuffer[4] = (uint8_t)(quantity >> 8);
    s_txBuffer[5] = (uint8_t)(quantity & 0xFFU);

    ModbusSlave_SendResponse(6U);
}

/* 处理接收到的帧 */
static void ModbusSlave_ProcessFrame(const uint8_t* frame, uint16_t length)
{
    if(length < 4)
    {
        s_errorCount++;
        return;
    }

    uint16_t crcReceived = ((uint16_t)frame[length - 1] << 8) | frame[length - 2];
    uint16_t crcCalc = ModbusSlave_CalcCRC(frame, length - 2);

    if(crcReceived != crcCalc)
    {
        s_errorCount++;
        ELOG_ERROR("[Slave] CRC error. Recv:0x%04X Calc:0x%04X", crcReceived, crcCalc);
        return;
    }

    uint8_t address = frame[0];
    if(address != s_slaveAddress && address != 0x00)
    {
        return;
    }

    s_rxFrameCount++;

    uint8_t functionCode = frame[1];

    switch(functionCode)
    {
        case MODBUS_FC_READ_COILS:
            ModbusSlave_HandleReadCoils(frame, length);
            break;

        case MODBUS_FC_READ_DISCRETE_INPUTS:
            ModbusSlave_HandleReadDiscreteInputs(frame, length);
            break;

        case MODBUS_FC_READ_HOLDING_REGISTERS:
            ModbusSlave_HandleReadHoldingRegisters(frame, length);
            break;

        case MODBUS_FC_READ_INPUT_REGISTERS:
            ModbusSlave_HandleReadInputRegisters(frame, length);
            break;

        case MODBUS_FC_WRITE_SINGLE_COIL:
            ModbusSlave_HandleWriteSingleCoil(frame, length);
            break;

        case MODBUS_FC_WRITE_SINGLE_REGISTER:
            ModbusSlave_HandleWriteSingleRegister(frame, length);
            break;

        case MODBUS_FC_WRITE_MULTIPLE_COILS:
            ModbusSlave_HandleWriteMultipleCoils(frame, length);
            break;

        case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            ModbusSlave_HandleWriteMultipleRegisters(frame, length);
            break;

        default:
            ModbusSlave_SendException(functionCode, MODBUS_EX_ILLEGAL_FUNCTION);
            break;
    }
}

void ModbusSlave_Init(uint8_t slaveAddress)
{
    s_slaveAddress = slaveAddress;
    s_state = MODBUS_SLAVE_IDLE;

    memset(s_holdingRegisters, 0, sizeof(s_holdingRegisters));
    memset(s_inputRegisters, 0, sizeof(s_inputRegisters));
    memset(s_coils, 0, sizeof(s_coils));
    memset(s_discreteInputs, 0, sizeof(s_discreteInputs));

    s_rxFrameCount = 0;
    s_txFrameCount = 0;
    s_errorCount = 0;

    USART1_DMA_Init();
    USART1_DMA_FlushRxBuffer();

    ELOG_INFO("[Slave] Init OK. Address: 0x%02X", s_slaveAddress);
}

void ModbusSlave_Task(void)
{
    if(!USART1_IsFrameReceived())
    {
        return;
    }

    USART1_ClearFrameReceivedFlag();

    uint16_t rxLen = USART1_DMA_GetRxData(s_rxBuffer, MODBUS_SLAVE_RX_BUFFER_SIZE);

    if(rxLen >= 4)
    {
        ModbusSlave_ProcessFrame(s_rxBuffer, rxLen);
    }
}

bool ModbusSlave_GetHoldingRegister(uint16_t address, uint16_t* value)
{
    if(value == NULL)
    {
        return false;
    }

    if(address < MODBUS_SLAVE_HOLDING_REG_START ||
       address >= (MODBUS_SLAVE_HOLDING_REG_START + MODBUS_SLAVE_HOLDING_REG_COUNT))
    {
        return false;
    }

    *value = s_holdingRegisters[address - MODBUS_SLAVE_HOLDING_REG_START];
    return true;
}

bool ModbusSlave_SetHoldingRegister(uint16_t address, uint16_t value)
{
    if(address < MODBUS_SLAVE_HOLDING_REG_START ||
       address >= (MODBUS_SLAVE_HOLDING_REG_START + MODBUS_SLAVE_HOLDING_REG_COUNT))
    {
        return false;
    }

    s_holdingRegisters[address - MODBUS_SLAVE_HOLDING_REG_START] = value;
    return true;
}

bool ModbusSlave_GetInputRegister(uint16_t address, uint16_t* value)
{
    if(value == NULL)
    {
        return false;
    }

    if(address < MODBUS_SLAVE_INPUT_REG_START ||
       address >= (MODBUS_SLAVE_INPUT_REG_START + MODBUS_SLAVE_INPUT_REG_COUNT))
    {
        return false;
    }

    *value = s_inputRegisters[address - MODBUS_SLAVE_INPUT_REG_START];
    return true;
}

bool ModbusSlave_SetInputRegister(uint16_t address, uint16_t value)
{
    if(address < MODBUS_SLAVE_INPUT_REG_START ||
       address >= (MODBUS_SLAVE_INPUT_REG_START + MODBUS_SLAVE_INPUT_REG_COUNT))
    {
        return false;
    }

    s_inputRegisters[address - MODBUS_SLAVE_INPUT_REG_START] = value;
    return true;
}

bool ModbusSlave_GetCoil(uint16_t address, bool* state)
{
    if(state == NULL)
    {
        return false;
    }

    if(address < MODBUS_SLAVE_COIL_START ||
       address >= (MODBUS_SLAVE_COIL_START + MODBUS_SLAVE_COIL_COUNT))
    {
        return false;
    }

    uint16_t index = address - MODBUS_SLAVE_COIL_START;
    uint8_t byteIndex = index / 8U;
    uint8_t bitIndex = index % 8U;

    *state = (s_coils[byteIndex] & (1U << bitIndex)) != 0;
    return true;
}

bool ModbusSlave_SetCoil(uint16_t address, bool state)
{
    if(address < MODBUS_SLAVE_COIL_START ||
       address >= (MODBUS_SLAVE_COIL_START + MODBUS_SLAVE_COIL_COUNT))
    {
        return false;
    }

    uint16_t index = address - MODBUS_SLAVE_COIL_START;
    uint8_t byteIndex = index / 8U;
    uint8_t bitIndex = index % 8U;

    if(state)
    {
        s_coils[byteIndex] |= (1U << bitIndex);
    }
    else
    {
        s_coils[byteIndex] &= ~(1U << bitIndex);
    }

    return true;
}

bool ModbusSlave_GetDiscreteInput(uint16_t address, bool* state)
{
    if(state == NULL)
    {
        return false;
    }

    if(address < MODBUS_SLAVE_DISCRETE_START ||
       address >= (MODBUS_SLAVE_DISCRETE_START + MODBUS_SLAVE_DISCRETE_COUNT))
    {
        return false;
    }

    uint16_t index = address - MODBUS_SLAVE_DISCRETE_START;
    uint8_t byteIndex = index / 8U;
    uint8_t bitIndex = index % 8U;

    *state = (s_discreteInputs[byteIndex] & (1U << bitIndex)) != 0;
    return true;
}

bool ModbusSlave_SetDiscreteInput(uint16_t address, bool state)
{
    if(address < MODBUS_SLAVE_DISCRETE_START ||
       address >= (MODBUS_SLAVE_DISCRETE_START + MODBUS_SLAVE_DISCRETE_COUNT))
    {
        return false;
    }

    uint16_t index = address - MODBUS_SLAVE_DISCRETE_START;
    uint8_t byteIndex = index / 8U;
    uint8_t bitIndex = index % 8U;

    if(state)
    {
        s_discreteInputs[byteIndex] |= (1U << bitIndex);
    }
    else
    {
        s_discreteInputs[byteIndex] &= ~(1U << bitIndex);
    }

    return true;
}

uint16_t* ModbusSlave_GetHoldingRegisters(void)
{
    return s_holdingRegisters;
}

uint16_t* ModbusSlave_GetInputRegisters(void)
{
    return s_inputRegisters;
}

void ModbusSlave_RegisterWriteCallback(ModbusSlaveWriteCallback_t callback)
{
    s_writeCallback = callback;
}

void ModbusSlave_GetStats(uint32_t* rxCount, uint32_t* txCount, uint32_t* errCount)
{
    if(rxCount != NULL)
    {
        *rxCount = s_rxFrameCount;
    }
    if(txCount != NULL)
    {
        *txCount = s_txFrameCount;
    }
    if(errCount != NULL)
    {
        *errCount = s_errorCount;
    }
}

void ModbusSlave_ResetStats(void)
{
    s_rxFrameCount = 0;
    s_txFrameCount = 0;
    s_errorCount = 0;
}
