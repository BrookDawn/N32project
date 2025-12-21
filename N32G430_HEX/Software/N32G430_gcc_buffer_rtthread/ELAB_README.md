# Elab框架适配说明

## 项目概述
本项目成功将Elab日志框架适配到N32G430微控制器 + RT-Thread Nano环境中。

## 编译结果
- **程序大小**: text=50192 bytes, data=1768 bytes, bss=7400 bytes
- **总计**: 59360 bytes (58.9 KB)
- **编译状态**: ✅ 成功

## 项目结构

```
APP/
├── Elab_LOG/              # Elab框架核心
│   ├── elab_log.h/c       # 日志模块
│   ├── elab_common.h/c    # 公共函数
│   ├── elab_def.h         # 编译器定义
│   └── elab_std.h         # 标准头文件
├── Src/
│   └── elab_led.c         # LED适配层实现
└── Ins/
    └── elab_led.h         # LED适配层头文件

core/
├── src/
│   ├── bsp_elab_adapter.c # Elab适配层(时间函数)
│   ├── bsp_led.c          # LED底层驱动(已修复灌电流逻辑)
│   └── main.c             # 主程序(包含测试代码)
└── inc/
    ├── bsp_elab_adapter.h
    └── bsp_led.h
```

## 已完成功能

### 1. LED驱动修复
- ✅ 修复了灌电流LED控制逻辑
- ✅ LED1 (PB2): 低电平点亮，高电平熄灭

### 2. Elab框架适配
- ✅ 实现 `elab_time_ms()` 函数，基于RT-Thread tick
- ✅ 支持彩色日志输出(ANSI转义码)
- ✅ 5个日志等级: DEBUG, INFO, WARN, ERROR, FATAL

### 3. LED控制模块
基于Elab框架封装的LED控制API：
- `elab_led_init()` - 初始化LED
- `elab_led_on()` - 打开LED
- `elab_led_off()` - 关闭LED
- `elab_led_toggle()` - 翻转LED状态
- `elab_led_blink()` - LED闪烁N次

### 4. 测试程序
创建了独立的测试线程 `elab_test_thread`，测试内容：
1. LED初始化
2. LED开/关控制
3. LED快速闪烁(5次)
4. LED慢速闪烁(3次，300ms间隔)
5. 显示系统运行时间

## 使用示例

### 基本日志输出
```c
#include "elab_log.h"

ELOG_DEBUG("调试信息: value = %d", value);
ELOG_INFO("普通信息: 系统已启动");
ELOG_WARN("警告信息: 温度过高");
ELOG_ERROR("错误信息: 通信失败");
ELOG_FATAL("严重错误: 系统崩溃");
```

### LED控制
```c
#include "elab_led.h"

// 初始化
elab_led_init(LED1_GPIO_PORT, LED1_GPIO_PIN, LED1_GPIO_CLK);

// 点亮LED
elab_led_on(LED1_GPIO_PORT, LED1_GPIO_PIN);

// 熄灭LED
elab_led_off(LED1_GPIO_PORT, LED1_GPIO_PIN);

// 闪烁5次，每次间隔200ms
elab_led_blink(LED1_GPIO_PORT, LED1_GPIO_PIN, 5, 200);
```

### 获取系统时间
```c
#include "bsp_elab_adapter.h"

uint32_t uptime = elab_time_ms();
ELOG_INFO("系统已运行 %lu 毫秒", uptime);
```

## 日志输出格式
```
[级别/文件名 L:行号, T时间戳] 日志内容
```

示例：
```
[I/elab_led.c L:93, T1234] Initializing LED on GPIO Port 0x40010C00, Pin 2
[D/elab_led.c L:47, T1235] Turning LED ON
```

## 测试程序运行流程
1. 系统初始化，输出系统信息
2. 创建3个线程：
   - `led` 线程 (优先级20) - 备用
   - `elab_test` 线程 (优先级15) - 执行测试
   - `uart` 线程 (优先级10) - UART通信
3. elab_test线程循环执行:
   - LED ON (1秒)
   - LED OFF (1秒)
   - 快速闪烁 (5次 × 200ms)
   - 慢速闪烁 (3次 × 300ms)
   - 等待3秒后重复

## 编译方法
```bash
cd GCC
make clean
make
```

## 输出文件
- `build/output.elf` - ELF可执行文件
- `build/output.hex` - HEX固件文件
- `build/output.bin` - BIN固件文件
- `build/output.map` - 内存映射文件

## 技术特性
- ✅ RT-Thread Nano RTOS
- ✅ Elab日志框架
- ✅ 彩色终端输出
- ✅ 灌电流LED驱动
- ✅ 模块化设计
- ✅ 多线程测试

## 作者信息
- **Framework**: Elab (模仿狗哥，恩师：极天隙流光)
- **Adapter**: BrookDawn
- **Date**: 2025-12-20
- **Version**: 0.1

## 备注
- LED硬件配置: PB2, 3.3V上拉, 1kΩ限流电阻, 灌电流点亮
- RT-Thread Nano版本，轻量级RTOS
- 支持SEGGER RTT调试输出
- UART波特率: 115200
