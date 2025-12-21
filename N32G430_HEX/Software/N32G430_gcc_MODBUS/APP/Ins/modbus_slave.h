/**
 * @file modbus_slave.h
 * @brief Modbus RTU从机协议实现
 * @version 1.0.0
 */

#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H

#include <stdbool.h>
#include <stdint.h>

/* 从机配置 */
#define MODBUS_SLAVE_ADDRESS_DEFAULT     0x01U
#define MODBUS_SLAVE_HOLDING_REG_START   0x0000U
#define MODBUS_SLAVE_HOLDING_REG_COUNT   256U
#define MODBUS_SLAVE_INPUT_REG_START     0x0000U
#define MODBUS_SLAVE_INPUT_REG_COUNT     64U
#define MODBUS_SLAVE_COIL_START          0x0000U
#define MODBUS_SLAVE_COIL_COUNT          64U
#define MODBUS_SLAVE_DISCRETE_START      0x0000U
#define MODBUS_SLAVE_DISCRETE_COUNT      64U

/* 帧缓冲区大小 */
#define MODBUS_SLAVE_RX_BUFFER_SIZE      256U
#define MODBUS_SLAVE_TX_BUFFER_SIZE      256U

/* 帧间超时 (ms) - 用于检测帧结束 */
#define MODBUS_SLAVE_FRAME_TIMEOUT_MS    5U

/* Modbus功能码 */
#define MODBUS_FC_READ_COILS                 0x01U
#define MODBUS_FC_READ_DISCRETE_INPUTS       0x02U
#define MODBUS_FC_READ_HOLDING_REGISTERS     0x03U
#define MODBUS_FC_READ_INPUT_REGISTERS       0x04U
#define MODBUS_FC_WRITE_SINGLE_COIL          0x05U
#define MODBUS_FC_WRITE_SINGLE_REGISTER      0x06U
#define MODBUS_FC_WRITE_MULTIPLE_COILS       0x0FU
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS   0x10U

/* Modbus异常码 */
#define MODBUS_EX_ILLEGAL_FUNCTION           0x01U
#define MODBUS_EX_ILLEGAL_DATA_ADDRESS       0x02U
#define MODBUS_EX_ILLEGAL_DATA_VALUE         0x03U
#define MODBUS_EX_SLAVE_DEVICE_FAILURE       0x04U

/* 从机状态 */
typedef enum {
    MODBUS_SLAVE_IDLE = 0,
    MODBUS_SLAVE_RECEIVING,
    MODBUS_SLAVE_PROCESSING,
    MODBUS_SLAVE_RESPONDING
} ModbusSlaveState_t;

/* 寄存器写回调函数类型 */
typedef bool (*ModbusSlaveWriteCallback_t)(uint16_t address, uint16_t value);

/* 寄存器读回调函数类型 */
typedef bool (*ModbusSlaveReadCallback_t)(uint16_t address, uint16_t* value);

/**
 * @brief 初始化Modbus从机
 * @param slaveAddress 从机地址
 */
void ModbusSlave_Init(uint8_t slaveAddress);

/**
 * @brief Modbus从机任务 (需在主循环中调用)
 */
void ModbusSlave_Task(void);

/**
 * @brief 获取保持寄存器值
 * @param address 寄存器地址
 * @param value 输出值指针
 * @return true成功, false地址无效
 */
bool ModbusSlave_GetHoldingRegister(uint16_t address, uint16_t* value);

/**
 * @brief 设置保持寄存器值
 * @param address 寄存器地址
 * @param value 要设置的值
 * @return true成功, false地址无效
 */
bool ModbusSlave_SetHoldingRegister(uint16_t address, uint16_t value);

/**
 * @brief 获取输入寄存器值
 * @param address 寄存器地址
 * @param value 输出值指针
 * @return true成功, false地址无效
 */
bool ModbusSlave_GetInputRegister(uint16_t address, uint16_t* value);

/**
 * @brief 设置输入寄存器值
 * @param address 寄存器地址
 * @param value 要设置的值
 * @return true成功, false地址无效
 */
bool ModbusSlave_SetInputRegister(uint16_t address, uint16_t value);

/**
 * @brief 获取线圈状态
 * @param address 线圈地址
 * @param state 输出状态指针
 * @return true成功, false地址无效
 */
bool ModbusSlave_GetCoil(uint16_t address, bool* state);

/**
 * @brief 设置线圈状态
 * @param address 线圈地址
 * @param state 要设置的状态
 * @return true成功, false地址无效
 */
bool ModbusSlave_SetCoil(uint16_t address, bool state);

/**
 * @brief 获取离散输入状态
 * @param address 离散输入地址
 * @param state 输出状态指针
 * @return true成功, false地址无效
 */
bool ModbusSlave_GetDiscreteInput(uint16_t address, bool* state);

/**
 * @brief 设置离散输入状态
 * @param address 离散输入地址
 * @param state 要设置的状态
 * @return true成功, false地址无效
 */
bool ModbusSlave_SetDiscreteInput(uint16_t address, bool state);

/**
 * @brief 获取保持寄存器数组指针 (用于批量访问)
 * @return 保持寄存器数组指针
 */
uint16_t* ModbusSlave_GetHoldingRegisters(void);

/**
 * @brief 获取输入寄存器数组指针 (用于批量访问)
 * @return 输入寄存器数组指针
 */
uint16_t* ModbusSlave_GetInputRegisters(void);

/**
 * @brief 注册保持寄存器写回调
 * @param callback 回调函数
 */
void ModbusSlave_RegisterWriteCallback(ModbusSlaveWriteCallback_t callback);

/**
 * @brief 获取通信统计信息
 * @param rxCount 接收帧计数输出
 * @param txCount 发送帧计数输出
 * @param errCount 错误计数输出
 */
void ModbusSlave_GetStats(uint32_t* rxCount, uint32_t* txCount, uint32_t* errCount);

/**
 * @brief 重置通信统计
 */
void ModbusSlave_ResetStats(void);

#endif /* MODBUS_SLAVE_H */
