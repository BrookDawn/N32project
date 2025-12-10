/**
 * @file modbus_vars.c
 * @brief Modbus全局变量表实现
 * @version 1.0.0
 *
 * 变量表采用清晰的行列格式，便于查看和维护
 */

#include "modbus_vars.h"
#include "modbus_slave.h"
#include <string.h>

/*============================================================================*/
/*                              变量值存储区                                   */
/*============================================================================*/

static int16_t s_varValues[VAR_COUNT];

/*============================================================================*/
/*                              变量描述表                                     */
/*============================================================================*/
/*
 * 表格说明:
 * - regAddr : Modbus保持寄存器地址
 * - type    : 数据类型 (TYPE_U16/TYPE_I16)
 * - access  : 访问权限 (RO=只读, RW=读写)
 * - minVal  : 最小值
 * - maxVal  : 最大值
 * - defVal  : 默认值
 * - eepAddr : EEPROM存储地址 (EEP_NONE=不保存)
 */

/*                              regAddr  type      access  minVal  maxVal  defVal  eepAddr                */
/*                              -------  --------  ------  ------  ------  ------  -------                */
static const VarDesc_t s_varTable[VAR_COUNT] = {
/*============================================================================*/
/* 系统参数                                                                    */
/*============================================================================*/
/* VAR_SLAVE_ADDR  从机地址   */ { 0x0000, TYPE_U16, RW,        1,    247,      1,   0x0010 },
/* VAR_BAUD_RATE   波特率     */ { 0x0001, TYPE_U16, RW,        0,      4,      0,   0x0012 },
/* VAR_PARITY      校验方式   */ { 0x0002, TYPE_U16, RW,        0,      2,      0,   0x0014 },
/* VAR_FW_VERSION  固件版本   */ { 0x0003, TYPE_U16, RO,        0,  32767,  0x100, EEP_NONE },

/*============================================================================*/
/* 控制参数                                                                    */
/*============================================================================*/
/* VAR_CTRL_MODE   控制模式   */ { 0x0010, TYPE_U16, RW,        0,      3,      0,   0x0020 },
/* VAR_SET_VALUE   设定值     */ { 0x0011, TYPE_I16, RW,   -10000,  10000,      0,   0x0022 },
/* VAR_OUTPUT_VAL  输出值     */ { 0x0012, TYPE_I16, RO,   -10000,  10000,      0, EEP_NONE },
/* VAR_MANUAL_OUT  手动输出   */ { 0x0013, TYPE_I16, RW,   -10000,  10000,      0,   0x0024 },

/*============================================================================*/
/* 限位参数                                                                    */
/*============================================================================*/
/* VAR_OUT_HIGH_LIM 输出上限  */ { 0x0030, TYPE_I16, RW,   -32768,  32767,  10000,   0x0030 },
/* VAR_OUT_LOW_LIM  输出下限  */ { 0x0031, TYPE_I16, RW,   -32768,  32767,      0,   0x0032 },
/* VAR_IN_HIGH_LIM  输入上限  */ { 0x0032, TYPE_I16, RW,   -32768,  32767,  10000,   0x0034 },
/* VAR_IN_LOW_LIM   输入下限  */ { 0x0033, TYPE_I16, RW,   -32768,  32767,      0,   0x0036 },

/*============================================================================*/
/* PID参数                                                                     */
/*============================================================================*/
/* VAR_PID_KP      比例系数   */ { 0x0040, TYPE_U16, RW,        0,  10000,    100,   0x0040 },
/* VAR_PID_KI      积分系数   */ { 0x0041, TYPE_U16, RW,        0,  10000,     10,   0x0042 },
/* VAR_PID_KD      微分系数   */ { 0x0042, TYPE_U16, RW,        0,  10000,      0,   0x0044 },
/* VAR_PID_PERIOD  控制周期ms */ { 0x0043, TYPE_U16, RW,       10,  10000,    100,   0x0046 },

/*============================================================================*/
/* 模拟量输入 (只读)                                                           */
/*============================================================================*/
/* VAR_AI_CH1      模拟输入1  */ { 0x0050, TYPE_I16, RO,   -32768,  32767,      0, EEP_NONE },
/* VAR_AI_CH2      模拟输入2  */ { 0x0051, TYPE_I16, RO,   -32768,  32767,      0, EEP_NONE },
/* VAR_AI_CH3      模拟输入3  */ { 0x0052, TYPE_I16, RO,   -32768,  32767,      0, EEP_NONE },
/* VAR_AI_CH4      模拟输入4  */ { 0x0053, TYPE_I16, RO,   -32768,  32767,      0, EEP_NONE },

/*============================================================================*/
/* 模拟量输出                                                                  */
/*============================================================================*/
/* VAR_AO_CH1      模拟输出1  */ { 0x0060, TYPE_I16, RW,        0,  10000,      0, EEP_NONE },
/* VAR_AO_CH2      模拟输出2  */ { 0x0061, TYPE_I16, RW,        0,  10000,      0, EEP_NONE },

/*============================================================================*/
/* 报警参数                                                                    */
/*============================================================================*/
/* VAR_ALARM_HIGH  高报警值   */ { 0x0070, TYPE_I16, RW,   -32768,  32767,   9000,   0x0050 },
/* VAR_ALARM_LOW   低报警值   */ { 0x0071, TYPE_I16, RW,   -32768,  32767,   1000,   0x0052 },
/* VAR_ALARM_DEAD  报警死区   */ { 0x0072, TYPE_U16, RW,        0,   1000,     50,   0x0054 },
/* VAR_ALARM_STAT  报警状态   */ { 0x0073, TYPE_U16, RO,        0,  65535,      0, EEP_NONE },

/*============================================================================*/
/* 校准参数                                                                    */
/*============================================================================*/
/* VAR_CAL_ZERO    零点校准   */ { 0x0080, TYPE_I16, RW,   -10000,  10000,      0,   0x0060 },
/* VAR_CAL_SPAN    满量程校准 */ { 0x0081, TYPE_I16, RW,   -10000,  10000,  10000,   0x0062 },
/* VAR_CAL_OFFSET  偏移校准   */ { 0x0082, TYPE_I16, RW,   -10000,  10000,      0,   0x0064 },
};

/*============================================================================*/
/*                              API函数实现                                   */
/*============================================================================*/

void ModbusVars_Init(void)
{
    ModbusVars_RestoreDefaults();
}

const VarDesc_t* ModbusVars_GetTable(uint16_t* pCount)
{
    if (pCount != NULL)
    {
        *pCount = VAR_COUNT;
    }
    return s_varTable;
}

int16_t* ModbusVars_GetValuePtr(VarIndex_t idx)
{
    if (idx >= VAR_COUNT)
    {
        return NULL;
    }
    return &s_varValues[idx];
}

bool ModbusVars_Read(VarIndex_t idx, int16_t* pVal)
{
    if (idx >= VAR_COUNT || pVal == NULL)
    {
        return false;
    }
    *pVal = s_varValues[idx];
    return true;
}

bool ModbusVars_Write(VarIndex_t idx, int16_t val)
{
    if (idx >= VAR_COUNT)
    {
        return false;
    }

    const VarDesc_t* pDesc = &s_varTable[idx];

    /* 检查写权限 */
    if (pDesc->access == RO)
    {
        return false;
    }

    /* 范围检查 */
    if (val < pDesc->minVal || val > pDesc->maxVal)
    {
        return false;
    }

    s_varValues[idx] = val;
    return true;
}

bool ModbusVars_ReadByAddr(uint16_t addr, int16_t* pVal)
{
    if (pVal == NULL)
    {
        return false;
    }

    for (uint16_t i = 0; i < VAR_COUNT; i++)
    {
        if (s_varTable[i].regAddr == addr)
        {
            *pVal = s_varValues[i];
            return true;
        }
    }
    return false;
}

bool ModbusVars_WriteByAddr(uint16_t addr, int16_t val)
{
    for (uint16_t i = 0; i < VAR_COUNT; i++)
    {
        if (s_varTable[i].regAddr == addr)
        {
            return ModbusVars_Write((VarIndex_t)i, val);
        }
    }
    return false;
}

void ModbusVars_RestoreDefaults(void)
{
    for (uint16_t i = 0; i < VAR_COUNT; i++)
    {
        s_varValues[i] = s_varTable[i].defVal;
    }
}

bool ModbusVars_SaveAll(void)
{
    /* TODO: 根据实际EEPROM驱动实现 */
    /*
    for (uint16_t i = 0; i < VAR_COUNT; i++)
    {
        if (s_varTable[i].eepAddr != EEP_NONE)
        {
            EEPROM_WriteU16(s_varTable[i].eepAddr, (uint16_t)s_varValues[i]);
        }
    }
    EEPROM_WriteU16(EEP_MAGIC, EEP_MAGIC_VAL);
    */
    return true;
}

bool ModbusVars_LoadAll(void)
{
    /* TODO: 根据实际EEPROM驱动实现 */
    /*
    uint16_t magic = EEPROM_ReadU16(EEP_MAGIC);
    if (magic != EEP_MAGIC_VAL)
    {
        ModbusVars_RestoreDefaults();
        return false;
    }

    for (uint16_t i = 0; i < VAR_COUNT; i++)
    {
        if (s_varTable[i].eepAddr != EEP_NONE)
        {
            s_varValues[i] = (int16_t)EEPROM_ReadU16(s_varTable[i].eepAddr);
        }
    }
    */
    return true;
}

void ModbusVars_SyncToModbus(void)
{
    for (uint16_t i = 0; i < VAR_COUNT; i++)
    {
        ModbusSlave_SetHoldingRegister(s_varTable[i].regAddr, (uint16_t)s_varValues[i]);
    }
}

void ModbusVars_SyncFromModbus(void)
{
    for (uint16_t i = 0; i < VAR_COUNT; i++)
    {
        uint16_t val;
        if (ModbusSlave_GetHoldingRegister(s_varTable[i].regAddr, &val))
        {
            if (s_varTable[i].access == RW)
            {
                int16_t sval = (int16_t)val;
                if (sval >= s_varTable[i].minVal && sval <= s_varTable[i].maxVal)
                {
                    s_varValues[i] = sval;
                }
            }
        }
    }
}
