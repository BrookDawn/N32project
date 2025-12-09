#ifndef MODBUS_MASTER_H
#define MODBUS_MASTER_H

#include <stdbool.h>
#include <stdint.h>

/* 轮询寄存器范围 */
#define MODBUS_REGISTER_START            0x0000U
#define MODBUS_REGISTER_END              0x00BFU
#define MODBUS_REGISTER_COUNT            (MODBUS_REGISTER_END - MODBUS_REGISTER_START + 1U)

/* 默认从机地址 */
#define MODBUS_SLAVE_ADDRESS_DEFAULT     0x01U

/* 写操作队列大小 */
#define MODBUS_WRITE_QUEUE_SIZE          8U
#define MODBUS_MAX_WRITE_REGS            16U

/* 写请求类型 */
typedef enum {
    MODBUS_WRITE_SINGLE = 0,
    MODBUS_WRITE_MULTIPLE
} ModbusWriteType_t;

/* 写请求结构 */
typedef struct {
    ModbusWriteType_t type;
    uint16_t address;
    uint16_t count;
    uint16_t values[MODBUS_MAX_WRITE_REGS];
} ModbusWriteRequest_t;

void ModbusMaster_Init(uint8_t slaveAddress);
void ModbusMaster_Task(void);
const uint16_t* ModbusMaster_GetRegisterCache(void);
bool ModbusMaster_GetRegister(uint16_t address, uint16_t* value);

/* 写操作API */
bool ModbusMaster_WriteSingleRegister(uint16_t address, uint16_t value);
bool ModbusMaster_WriteMultipleRegisters(uint16_t address, uint16_t count, const uint16_t* values);
uint8_t ModbusMaster_GetWriteQueueCount(void);

#endif /* MODBUS_MASTER_H */
