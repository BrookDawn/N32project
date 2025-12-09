#!/usr/bin/env python3
"""
串口测试脚本 - 模拟Modbus从机响应
用于测试USART2 DMA功能
"""

import sys
import time

# 尝试不同的串口库
try:
    import serial
except ImportError:
    print("正在尝试安装 pyserial...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "pyserial"])
    import serial

def calc_crc(data):
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

def create_read_response(slave_addr, func_code, data):
    """创建读寄存器响应帧"""
    response = bytearray()
    response.append(slave_addr)
    response.append(func_code)
    response.append(len(data))
    response.extend(data)
    crc = calc_crc(response)
    response.append(crc & 0xFF)
    response.append((crc >> 8) & 0xFF)
    return bytes(response)

def create_write_single_response(slave_addr, address, value):
    """创建写单个寄存器响应帧"""
    response = bytearray()
    response.append(slave_addr)
    response.append(0x06)  # 功能码
    response.append((address >> 8) & 0xFF)
    response.append(address & 0xFF)
    response.append((value >> 8) & 0xFF)
    response.append(value & 0xFF)
    crc = calc_crc(response)
    response.append(crc & 0xFF)
    response.append((crc >> 8) & 0xFF)
    return bytes(response)

def create_write_multiple_response(slave_addr, address, count):
    """创建写多个寄存器响应帧"""
    response = bytearray()
    response.append(slave_addr)
    response.append(0x10)  # 功能码
    response.append((address >> 8) & 0xFF)
    response.append(address & 0xFF)
    response.append((count >> 8) & 0xFF)
    response.append(count & 0xFF)
    crc = calc_crc(response)
    response.append(crc & 0xFF)
    response.append((crc >> 8) & 0xFF)
    return bytes(response)

def main():
    port_name = "COM3"
    baud_rate = 115200

    print(f"打开串口 {port_name} @ {baud_rate} bps")

    try:
        ser = serial.Serial(
            port=port_name,
            baudrate=baud_rate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.5
        )
        print(f"串口已打开: {ser.name}")
    except Exception as e:
        print(f"无法打开串口: {e}")
        return

    print("等待Modbus主站请求...")
    print("按 Ctrl+C 退出\n")

    request_count = 0

    try:
        while True:
            # 等待接收数据
            if ser.in_waiting > 0:
                time.sleep(0.05)  # 等待完整帧
                data = ser.read(ser.in_waiting)

                if len(data) >= 4:
                    request_count += 1
                    hex_str = ' '.join(f'{b:02X}' for b in data)
                    print(f"[{request_count}] 收到 {len(data)} 字节: {hex_str}")

                    slave_addr = data[0]
                    func_code = data[1]

                    # 解析并响应
                    if func_code == 0x03:  # 读保持寄存器
                        start_addr = (data[2] << 8) | data[3]
                        quantity = (data[4] << 8) | data[5]
                        print(f"    -> 读请求: 起始=0x{start_addr:04X}, 数量={quantity}")

                        # 生成模拟数据
                        reg_data = bytearray()
                        for i in range(quantity):
                            val = (start_addr + i) & 0xFFFF
                            reg_data.append((val >> 8) & 0xFF)
                            reg_data.append(val & 0xFF)

                        response = create_read_response(slave_addr, func_code, reg_data)
                        ser.write(response)
                        hex_resp = ' '.join(f'{b:02X}' for b in response)
                        print(f"    <- 响应 {len(response)} 字节: {hex_resp}")

                    elif func_code == 0x06:  # 写单个寄存器
                        address = (data[2] << 8) | data[3]
                        value = (data[4] << 8) | data[5]
                        print(f"    -> 写单个: 地址=0x{address:04X}, 值=0x{value:04X}")

                        response = create_write_single_response(slave_addr, address, value)
                        ser.write(response)
                        hex_resp = ' '.join(f'{b:02X}' for b in response)
                        print(f"    <- 响应 {len(response)} 字节: {hex_resp}")

                    elif func_code == 0x10:  # 写多个寄存器
                        address = (data[2] << 8) | data[3]
                        count = (data[4] << 8) | data[5]
                        print(f"    -> 写多个: 地址=0x{address:04X}, 数量={count}")

                        response = create_write_multiple_response(slave_addr, address, count)
                        ser.write(response)
                        hex_resp = ' '.join(f'{b:02X}' for b in response)
                        print(f"    <- 响应 {len(response)} 字节: {hex_resp}")
                    else:
                        print(f"    未知功能码: 0x{func_code:02X}")

                    print()

            time.sleep(0.01)

    except KeyboardInterrupt:
        print("\n\n测试结束")
    finally:
        ser.close()
        print("串口已关闭")

if __name__ == "__main__":
    main()
