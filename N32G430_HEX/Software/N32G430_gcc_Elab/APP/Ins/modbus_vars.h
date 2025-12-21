/**
 * @file modbus_vars.h
 * @brief Modbus全局变量表定义
 * @version 1.0.0
 */

#ifndef MODBUS_VARS_H
#define MODBUS_VARS_H

#include <stdint.h>
#include <stdbool.h>

/*============================================================================*/
/*                              类型定义                                       */
/*============================================================================*/

/* 变量数据类型 */
typedef enum {
    TYPE_U16 = 0,   /* 无符号16位 */
    TYPE_I16,       /* 有符号16位 */
    TYPE_U32,       /* 无符号32位 */
    TYPE_I32,       /* 有符号32位 */
} VarType_t;

/* 访问权限 */
typedef enum {
    RO = 0,         /* 只读 */
    RW,             /* 读写 */
} VarAccess_t;

/* 变量描述符 */
typedef struct {
    uint16_t    regAddr;    /* Modbus寄存器地址 */
    VarType_t   type;       /* 数据类型 */
    VarAccess_t access;     /* 访问权限 */
    int16_t     minVal;     /* 最小值 */
    int16_t     maxVal;     /* 最大值 */
    int16_t     defVal;     /* 默认值 */
    uint16_t    eepAddr;    /* EEPROM地址, 0xFFFF表示不保存 */
} VarDesc_t;

/*============================================================================*/
/*                              变量索引定义                                   */
/*============================================================================*/

typedef enum {
    /* 系统参数 */
    VAR_SLAVE_ADDR,
    VAR_BAUD_RATE,
    VAR_PARITY,
    VAR_FW_VERSION,
    /* 控制参数 */
    VAR_CTRL_MODE,
    VAR_SET_VALUE,
    VAR_OUTPUT_VAL,
    VAR_MANUAL_OUT,
    /* 限位参数 */
    VAR_OUT_HIGH_LIM,
    VAR_OUT_LOW_LIM,
    VAR_IN_HIGH_LIM,
    VAR_IN_LOW_LIM,
    /* PID参数 */
    VAR_PID_KP,
    VAR_PID_KI,
    VAR_PID_KD,
    VAR_PID_PERIOD,
    /* 模拟量输入 */
    VAR_AI_CH1,
    VAR_AI_CH2,
    VAR_AI_CH3,
    VAR_AI_CH4,
    /* 模拟量输出 */
    VAR_AO_CH1,
    VAR_AO_CH2,
    /* 报警参数 */
    VAR_ALARM_HIGH,
    VAR_ALARM_LOW,
    VAR_ALARM_DEAD,
    VAR_ALARM_STAT,
    /* 校准参数 */
    VAR_CAL_ZERO,
    VAR_CAL_SPAN,
    VAR_CAL_OFFSET,
    /* 总数 */
    VAR_COUNT
} VarIndex_t;

/*============================================================================*/
/*                              EEPROM地址定义                                 */
/*============================================================================*/

#define EEP_NONE        0xFFFFU     /* 不保存到EEPROM */
#define EEP_MAGIC       0x0000U     /* 魔数地址 */
#define EEP_MAGIC_VAL   0xA55AU     /* 魔数值 */

/*============================================================================*/
/*                              API函数声明                                   */
/*============================================================================*/

void                ModbusVars_Init(void);
const VarDesc_t*    ModbusVars_GetTable(uint16_t* pCount);
int16_t*            ModbusVars_GetValuePtr(VarIndex_t idx);
bool                ModbusVars_Read(VarIndex_t idx, int16_t* pVal);
bool                ModbusVars_Write(VarIndex_t idx, int16_t val);
bool                ModbusVars_ReadByAddr(uint16_t addr, int16_t* pVal);
bool                ModbusVars_WriteByAddr(uint16_t addr, int16_t val);
void                ModbusVars_RestoreDefaults(void);
bool                ModbusVars_SaveAll(void);
bool                ModbusVars_LoadAll(void);
void                ModbusVars_SyncToModbus(void);
void                ModbusVars_SyncFromModbus(void);

#endif /* MODBUS_VARS_H */
