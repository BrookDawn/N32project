# N32project

基于 **Nations N32G430 微控制器** (ARM Cortex-M4) 的嵌入式开发项目，包含固件开发和硬件设计。

## 目录结构

```
N32project/
├── BLDC/                      # BLDC电机控制器硬件设计 (KiCad)
├── Light_bicycle/             # 自行车灯硬件设计 (KiCad)
└── N32G430_HEX/               # N32G430主项目
    ├── Hardware/              # 开发板硬件设计
    └── Software/              # 软件项目
        ├── N32G430_gcc_buffer/          # Modbus主从机项目
        ├── N32G430_gcc_buffer_rtthread/ # RT-Thread集成版本
        ├── N32G430_gcc_example/         # OLED驱动示例
        ├── N32G430_gcc_example_oled/    # OLED多屏幕适配
        └── N32G430_g++/                 # C++版本 (LED抽象层)
```

## 主要功能模块

### Modbus RTU 协议栈

完整的 Modbus RTU 主从机实现：

**从机功能** (`modbus_slave.c`):
- 功能码: 0x01/0x02 (读线圈/离散输入), 0x03/0x04 (读保持/输入寄存器), 0x05/0x06 (写单个线圈/寄存器), 0x0F/0x10 (写多个线圈/寄存器)
- 256个保持寄存器, 64个输入寄存器, 64个线圈, 64个离散输入
- USART1 DMA中断通信
- CRC16校验和异常码响应

**主机功能** (`modbus_master.c`):
- 寄存器轮询读取和写请求队列
- 支持功能码 0x03, 0x06, 0x10
- USART2 DMA通信

**全局变量表** (`modbus_vars.c`):
- 预定义变量索引 (系统参数、PID参数、模拟量等)
- EEPROM保存支持

### OLED 显示驱动

- 多屏幕支持: SSD1306 (0.96", 0.91"), SH1106 (1.3")
- 分辨率: 128x64, 128x32
- I2C DMA传输
- U8G2风格绘图API
- 分区刷新 (脏矩形跟踪)
- 动画系统和UI组件

### LED 控制系统 (C++)

- 分层架构: 应用层 -> 抽象层 -> 适配层 -> BSP -> 硬件
- 状态管理和闪烁控制
- 平台无关设计

## 技术栈

| 类别 | 技术 |
|------|------|
| MCU | Nations N32G430 (ARM Cortex-M4, 128MHz, FPU) |
| 编译器 | ARM GCC (arm-none-eabi-gcc) |
| 调试器 | J-Link + SEGGER RTT |
| PCB设计 | KiCad |
| 通信协议 | Modbus RTU, I2C, USART (DMA) |
| 操作系统 | 裸机 / RT-Thread (可选) |

## 构建方法

```bash
cd N32G430_HEX/Software/N32G430_gcc_buffer
make          # 编译
make flash    # 下载到开发板
make clean    # 清理
```

编译配置:
- CPU: Cortex-M4
- FPU: fpv4-sp-d16 (硬件浮点)
- 优化: -Os

## 测试

Python测试脚本位于 `test/` 目录:

```bash
python test/modbus_slave_test.py   # Modbus从机测试
python test/modbus_vars_test.py    # 变量表测试
python test/serial_test.py         # 串口测试
```

## 硬件项目

| 项目 | 说明 |
|------|------|
| BLDC电机控制器 | 无刷直流电机驱动板 |
| 自行车灯控制器 | 基于N32G430CBT7 |
| N32G430开发板 | 含Gerber制造文件 |

## 许可证

本项目仅供学习和参考使用。
