#!/usr/bin/env python3
"""
Modbus变量表轮询测试脚本
轮询modbus_vars.c中定义的全部寄存器
"""

import serial
import struct
import time
import sys
from dataclasses import dataclass
from typing import List, Optional

#=============================================================================
# Modbus变量表定义 (与modbus_vars.c保持一致)
#=============================================================================

@dataclass
class VarDef:
    """变量定义"""
    name: str           # 变量名
    reg_addr: int       # Modbus寄存器地址
    var_type: str       # 数据类型 U16/I16
    access: str         # 访问权限 RO/RW
    min_val: int        # 最小值
    max_val: int        # 最大值
    def_val: int        # 默认值
    eep_addr: int       # EEPROM地址 (0xFFFF=不保存)
    desc: str           # 描述

# EEPROM不保存标志
EEP_NONE = 0xFFFF

#=============================================================================
# 变量表 (与modbus_vars.c中s_varTable保持一致)
#=============================================================================
#                        名称              地址    类型   权限   最小     最大     默认    EEPROM   描述
VAR_TABLE: List[VarDef] = [
#-----------------------------------------------------------------------------
# 系统参数
#-----------------------------------------------------------------------------
    VarDef("VAR_SLAVE_ADDR",   0x0000, "U16", "RW",      1,    247,      1, 0x0010, "从机地址"),
    VarDef("VAR_BAUD_RATE",    0x0001, "U16", "RW",      0,      4,      0, 0x0012, "波特率"),
    VarDef("VAR_PARITY",       0x0002, "U16", "RW",      0,      2,      0, 0x0014, "校验方式"),
    VarDef("VAR_FW_VERSION",   0x0003, "U16", "RO",      0,  32767, 0x0100, EEP_NONE, "固件版本"),
#-----------------------------------------------------------------------------
# 控制参数
#-----------------------------------------------------------------------------
    VarDef("VAR_CTRL_MODE",    0x0010, "U16", "RW",      0,      3,      0, 0x0020, "控制模式"),
    VarDef("VAR_SET_VALUE",    0x0011, "I16", "RW", -10000,  10000,      0, 0x0022, "设定值"),
    VarDef("VAR_OUTPUT_VAL",   0x0012, "I16", "RO", -10000,  10000,      0, EEP_NONE, "输出值"),
    VarDef("VAR_MANUAL_OUT",   0x0013, "I16", "RW", -10000,  10000,      0, 0x0024, "手动输出"),
#-----------------------------------------------------------------------------
# 限位参数
#-----------------------------------------------------------------------------
    VarDef("VAR_OUT_HIGH_LIM", 0x0030, "I16", "RW", -32768,  32767,  10000, 0x0030, "输出上限"),
    VarDef("VAR_OUT_LOW_LIM",  0x0031, "I16", "RW", -32768,  32767,      0, 0x0032, "输出下限"),
    VarDef("VAR_IN_HIGH_LIM",  0x0032, "I16", "RW", -32768,  32767,  10000, 0x0034, "输入上限"),
    VarDef("VAR_IN_LOW_LIM",   0x0033, "I16", "RW", -32768,  32767,      0, 0x0036, "输入下限"),
#-----------------------------------------------------------------------------
# PID参数
#-----------------------------------------------------------------------------
    VarDef("VAR_PID_KP",       0x0040, "U16", "RW",      0,  10000,    100, 0x0040, "比例系数"),
    VarDef("VAR_PID_KI",       0x0041, "U16", "RW",      0,  10000,     10, 0x0042, "积分系数"),
    VarDef("VAR_PID_KD",       0x0042, "U16", "RW",      0,  10000,      0, 0x0044, "微分系数"),
    VarDef("VAR_PID_PERIOD",   0x0043, "U16", "RW",     10,  10000,    100, 0x0046, "控制周期ms"),
#-----------------------------------------------------------------------------
# 模拟量输入 (只读)
#-----------------------------------------------------------------------------
    VarDef("VAR_AI_CH1",       0x0050, "I16", "RO", -32768,  32767,      0, EEP_NONE, "模拟输入1"),
    VarDef("VAR_AI_CH2",       0x0051, "I16", "RO", -32768,  32767,      0, EEP_NONE, "模拟输入2"),
    VarDef("VAR_AI_CH3",       0x0052, "I16", "RO", -32768,  32767,      0, EEP_NONE, "模拟输入3"),
    VarDef("VAR_AI_CH4",       0x0053, "I16", "RO", -32768,  32767,      0, EEP_NONE, "模拟输入4"),
#-----------------------------------------------------------------------------
# 模拟量输出
#-----------------------------------------------------------------------------
    VarDef("VAR_AO_CH1",       0x0060, "I16", "RW",      0,  10000,      0, EEP_NONE, "模拟输出1"),
    VarDef("VAR_AO_CH2",       0x0061, "I16", "RW",      0,  10000,      0, EEP_NONE, "模拟输出2"),
#-----------------------------------------------------------------------------
# 报警参数
#-----------------------------------------------------------------------------
    VarDef("VAR_ALARM_HIGH",   0x0070, "I16", "RW", -32768,  32767,   9000, 0x0050, "高报警值"),
    VarDef("VAR_ALARM_LOW",    0x0071, "I16", "RW", -32768,  32767,   1000, 0x0052, "低报警值"),
    VarDef("VAR_ALARM_DEAD",   0x0072, "U16", "RW",      0,   1000,     50, 0x0054, "报警死区"),
    VarDef("VAR_ALARM_STAT",   0x0073, "U16", "RO",      0,  65535,      0, EEP_NONE, "报警状态"),
#-----------------------------------------------------------------------------
# 校准参数
#-----------------------------------------------------------------------------
    VarDef("VAR_CAL_ZERO",     0x0080, "I16", "RW", -10000,  10000,      0, 0x0060, "零点校准"),
    VarDef("VAR_CAL_SPAN",     0x0081, "I16", "RW", -10000,  10000,  10000, 0x0062, "满量程校准"),
    VarDef("VAR_CAL_OFFSET",   0x0082, "I16", "RW", -10000,  10000,      0, 0x0064, "偏移校准"),
]

#=============================================================================
# Modbus通信函数
#=============================================================================

def calc_crc16(data: bytes) -> int:
    """计算Modbus CRC16"""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc

def read_holding_register(ser: serial.Serial, slave_addr: int, reg_addr: int) -> Optional[int]:
    """读取单个保持寄存器"""
    frame = bytes([slave_addr, 0x03,
                   (reg_addr >> 8) & 0xFF, reg_addr & 0xFF,
                   0x00, 0x01])
    crc = calc_crc16(frame)
    frame += struct.pack('<H', crc)

    ser.reset_input_buffer()
    ser.write(frame)
    time.sleep(0.1)

    # 读取响应
    response = b''
    end_time = time.time() + 0.5
    while time.time() < end_time:
        if ser.in_waiting > 0:
            response += ser.read(ser.in_waiting)
            time.sleep(0.02)
        else:
            if len(response) >= 7:
                break
            time.sleep(0.01)

    if len(response) >= 7:
        # 校验CRC
        crc_recv = struct.unpack('<H', response[-2:])[0]
        crc_calc = calc_crc16(response[:-2])
        if crc_recv == crc_calc and response[1] == 0x03:
            value = (response[3] << 8) | response[4]
            return value
    return None

def write_holding_register(ser: serial.Serial, slave_addr: int, reg_addr: int, value: int) -> bool:
    """写入单个保持寄存器"""
    frame = bytes([slave_addr, 0x06,
                   (reg_addr >> 8) & 0xFF, reg_addr & 0xFF,
                   (value >> 8) & 0xFF, value & 0xFF])
    crc = calc_crc16(frame)
    frame += struct.pack('<H', crc)

    ser.reset_input_buffer()
    ser.write(frame)
    time.sleep(0.1)

    # 读取响应
    response = b''
    end_time = time.time() + 0.5
    while time.time() < end_time:
        if ser.in_waiting > 0:
            response += ser.read(ser.in_waiting)
            time.sleep(0.02)
        else:
            if len(response) >= 8:
                break
            time.sleep(0.01)

    if len(response) >= 8:
        crc_recv = struct.unpack('<H', response[-2:])[0]
        crc_calc = calc_crc16(response[:-2])
        if crc_recv == crc_calc and response[1] == 0x06:
            return True
    return False

def format_value(value: int, var_type: str) -> str:
    """格式化显示值"""
    if var_type == "I16":
        # 有符号16位
        if value > 32767:
            value = value - 65536
        return f"{value:6d}"
    else:
        return f"{value:6d}"

#=============================================================================
# 测试功能
#=============================================================================

def poll_all_registers(ser: serial.Serial, slave_addr: int):
    """轮询所有寄存器"""
    print("\n" + "=" * 100)
    print(" Modbus变量表轮询")
    print("=" * 100)
    print(f"{'序号':^4} {'变量名':<18} {'地址':^8} {'类型':^4} {'权限':^4} "
          f"{'当前值':^8} {'默认值':^8} {'范围':^16} {'描述':<12}")
    print("-" * 100)

    success = 0
    failed = 0

    for i, var in enumerate(VAR_TABLE):
        # 读取当前值
        value = read_holding_register(ser, slave_addr, var.reg_addr)

        if value is not None:
            val_str = format_value(value, var.var_type)
            def_str = format_value(var.def_val & 0xFFFF, var.var_type)
            range_str = f"[{var.min_val}, {var.max_val}]"
            status = "OK"
            success += 1
        else:
            val_str = "ERROR"
            def_str = format_value(var.def_val & 0xFFFF, var.var_type)
            range_str = f"[{var.min_val}, {var.max_val}]"
            status = "FAIL"
            failed += 1

        print(f"{i:4d} {var.name:<18} 0x{var.reg_addr:04X}   {var.var_type:^4} {var.access:^4} "
              f"{val_str:>8} {def_str:>8} {range_str:>16} {var.desc:<12}")

    print("-" * 100)
    print(f"总计: {len(VAR_TABLE)} 个寄存器, 成功: {success}, 失败: {failed}")
    print("=" * 100)

    return failed == 0

def continuous_poll(ser: serial.Serial, slave_addr: int, interval: float = 1.0):
    """连续轮询模式"""
    print("\n[连续轮询模式] 按 Ctrl+C 退出\n")

    try:
        cycle = 0
        while True:
            cycle += 1
            print(f"\n--- 轮询周期 #{cycle} ---")

            # 打印表头
            print(f"{'变量名':<18} {'地址':^8} {'当前值':^10} {'描述':<12}")
            print("-" * 60)

            for var in VAR_TABLE:
                value = read_holding_register(ser, slave_addr, var.reg_addr)
                if value is not None:
                    val_str = format_value(value, var.var_type)
                else:
                    val_str = "ERROR"

                print(f"{var.name:<18} 0x{var.reg_addr:04X}   {val_str:>10} {var.desc:<12}")

            time.sleep(interval)

    except KeyboardInterrupt:
        print("\n\n[停止轮询]")

def test_write_registers(ser: serial.Serial, slave_addr: int):
    """测试写入可写寄存器"""
    print("\n" + "=" * 80)
    print(" 测试写入寄存器")
    print("=" * 80)

    rw_vars = [v for v in VAR_TABLE if v.access == "RW"]

    print(f"{'变量名':<18} {'地址':^8} {'写入值':^8} {'读回值':^8} {'结果':^8}")
    print("-" * 80)

    success = 0
    failed = 0

    for var in rw_vars:
        # 选择一个测试值 (在范围内)
        test_val = (var.min_val + var.max_val) // 2
        if test_val < 0:
            test_val = test_val & 0xFFFF  # 转为无符号

        # 写入
        write_ok = write_holding_register(ser, slave_addr, var.reg_addr, test_val)

        if write_ok:
            # 读回验证
            read_val = read_holding_register(ser, slave_addr, var.reg_addr)
            if read_val == test_val:
                result = "PASS"
                success += 1
            else:
                result = "MISMATCH"
                failed += 1
            read_str = f"0x{read_val:04X}" if read_val is not None else "ERROR"
        else:
            result = "FAIL"
            read_str = "N/A"
            failed += 1

        print(f"{var.name:<18} 0x{var.reg_addr:04X}   0x{test_val:04X}   {read_str:>8}   {result:^8}")

    print("-" * 80)
    print(f"可写寄存器: {len(rw_vars)} 个, 成功: {success}, 失败: {failed}")
    print("=" * 80)

    # 恢复默认值
    print("\n[恢复默认值...]")
    for var in rw_vars:
        def_val = var.def_val & 0xFFFF
        write_holding_register(ser, slave_addr, var.reg_addr, def_val)
    print("[完成]")

    return failed == 0

def print_usage():
    """打印使用说明"""
    print("""
Modbus变量表测试脚本
====================

用法: python modbus_vars_test.py [选项]

选项:
  -p, --port PORT       串口号 (默认: COM3)
  -b, --baud BAUDRATE   波特率 (默认: 115200)
  -a, --addr ADDRESS    从机地址 (默认: 1)
  -m, --mode MODE       测试模式:
                          poll     - 单次轮询所有寄存器 (默认)
                          cont     - 连续轮询
                          write    - 测试写入
                          all      - 执行所有测试
  -i, --interval SEC    连续轮询间隔秒数 (默认: 1.0)
  -h, --help            显示帮助信息

示例:
  python modbus_vars_test.py -p COM3 -b 115200 -m poll
  python modbus_vars_test.py -p COM5 -m cont -i 2
  python modbus_vars_test.py -m all
""")

def main():
    import argparse

    parser = argparse.ArgumentParser(description='Modbus变量表测试脚本', add_help=False)
    parser.add_argument('-p', '--port', default='COM3', help='串口号')
    parser.add_argument('-b', '--baud', type=int, default=115200, help='波特率')
    parser.add_argument('-a', '--addr', type=int, default=1, help='从机地址')
    parser.add_argument('-m', '--mode', default='poll', choices=['poll', 'cont', 'write', 'all'], help='测试模式')
    parser.add_argument('-i', '--interval', type=float, default=1.0, help='轮询间隔')
    parser.add_argument('-h', '--help', action='store_true', help='显示帮助')

    args = parser.parse_args()

    if args.help:
        print_usage()
        return 0

    print("=" * 60)
    print(" Modbus变量表测试")
    print("=" * 60)
    print(f" 串口: {args.port}")
    print(f" 波特率: {args.baud}")
    print(f" 从机地址: {args.addr}")
    print(f" 测试模式: {args.mode}")
    print(f" 变量数量: {len(VAR_TABLE)}")
    print("=" * 60)

    try:
        ser = serial.Serial(
            port=args.port,
            baudrate=args.baud,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )
        print(f"[OK] 串口 {args.port} 打开成功")
        time.sleep(0.5)
        ser.reset_input_buffer()

        result = True

        if args.mode == 'poll':
            result = poll_all_registers(ser, args.addr)
        elif args.mode == 'cont':
            continuous_poll(ser, args.addr, args.interval)
        elif args.mode == 'write':
            result = test_write_registers(ser, args.addr)
        elif args.mode == 'all':
            result1 = poll_all_registers(ser, args.addr)
            result2 = test_write_registers(ser, args.addr)
            result = result1 and result2

        ser.close()
        print(f"\n[{'PASS' if result else 'FAIL'}] 测试{'成功' if result else '失败'}")
        return 0 if result else 1

    except serial.SerialException as e:
        print(f"[ERROR] 串口错误: {e}")
        return 1
    except Exception as e:
        print(f"[ERROR] {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
