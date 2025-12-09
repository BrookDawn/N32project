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

void ModbusMaster_Init(uint8_t slaveAddress);
void ModbusMaster_Task(void);
const uint16_t* ModbusMaster_GetRegisterCache(void);
bool ModbusMaster_GetRegister(uint16_t address, uint16_t* value);

#endif /* MODBUS_MASTER_H */
