# FreeRTOS N32L403KBQ 模板工程

## 项目介绍

本项目是基于国民技术 N32L403KBQ 微控制器的 FreeRTOS 实时操作系统模板工程。该项目提供了一个完整的开发框架，包含 FreeRTOS 内核、硬件抽象层、外设驱动库以及示例应用程序，帮助开发者快速上手基于 N32L403KBQ 的嵌入式应用开发。

### 主要特性

- **目标芯片**: N32L403KBQ (ARM Cortex-M4F 内核)
- **RTOS版本**: FreeRTOS 202411.00
- **开发环境**: Keil MDK-ARM
- **系统时钟**: 64MHz (最大频率)
- **内存配置**: 128KB Flash, 24KB SRAM
- **示例功能**: LED 闪烁任务演示

## 芯片规格

### N32L403KBQ 技术参数

| 参数 | 规格 |
|------|------|
| **内核** | ARM Cortex-M4F (32位) |
| **主频** | 最高 64MHz |
| **Flash** | 128KB |
| **SRAM** | 24KB |
| **封装** | LQFP32 |
| **工作电压** | 1.8V - 3.6V |
| **工作温度** | -40°C ~ +85°C |

### 外设资源

- **GPIO**: 多达 25 个 I/O 引脚
- **定时器**: 8 个定时器 (TIM1-TIM9)
- **通信接口**: 
  - 3 个 USART
  - 2 个 I2C
  - 2 个 SPI
  - 1 个 CAN
  - 1 个 USB 2.0 FS
- **模拟外设**:
  - 12位 ADC (16通道)
  - 12位 DAC (2通道)
  - 2 个比较器
  - 2 个运算放大器
- **其他外设**:
  - RTC (实时时钟)
  - WWDG/IWDG (看门狗)
  - DMA (8通道)
  - LCD 控制器
  - 低功耗定时器 (LPTIM)
  - 低功耗串口 (LPUART)

## 软件架构

```
FreeRTOS_N32L40X_TEM/
├── firmware/                    # 固件库
│   ├── CMSIS/                  # ARM CMSIS 标准接口
│   │   ├── core/              # Cortex-M4 内核文件
│   │   └── device/            # N32L40X 设备相关文件
│   ├── n32l40x_std_periph_driver/  # 标准外设驱动库
│   │   ├── inc/               # 头文件
│   │   └── src/               # 源文件
│   └── n32l40x_usbfs_driver/   # USB 全速设备驱动
├── RTOS/                       # FreeRTOS 实时操作系统
│   ├── inc/                   # FreeRTOS 头文件
│   ├── src/                   # FreeRTOS 源文件
│   │   └── FreeRTOSConfig.h   # FreeRTOS 配置文件
│   └── port/                  # ARM Cortex-M4 移植层
├── USER/                       # 用户应用程序
│   ├── inc/                   # 用户头文件
│   │   ├── main.h
│   │   └── n32l40x_it.h
│   └── src/                   # 用户源文件
│       ├── main.c             # 主程序
│       └── n32l40x_it.c       # 中断服务程序
├── Objects/                    # 编译输出文件
├── Listings/                   # 编译列表文件
└── *.uvprojx                  # Keil 工程文件
```

### 系统架构层次

1. **硬件抽象层 (HAL)**
   - CMSIS 标准接口
   - N32L40X 设备驱动
   - 外设寄存器定义

2. **FreeRTOS 内核层**
   - 任务调度器
   - 内存管理 (heap_4)
   - 同步原语 (信号量、互斥量、队列)
   - 软件定时器

3. **应用程序层**
   - 用户任务
   - 业务逻辑
   - 外设控制

## FreeRTOS 配置

### 主要配置参数

| 配置项 | 值 | 说明 |
|--------|----|----|
| `configCPU_CLOCK_HZ` | 64000000 | 系统时钟频率 64MHz |
| `configTICK_RATE_HZ` | 1000 | 系统节拍频率 1000Hz (1ms) |
| `configMAX_PRIORITIES` | 5 | 最大任务优先级数量 |
| `configTOTAL_HEAP_SIZE` | 8192 | FreeRTOS 堆大小 8KB |
| `configMINIMAL_STACK_SIZE` | 128 | 最小任务堆栈大小 |
| `configUSE_PREEMPTION` | 1 | 启用抢占式调度 |
| `configUSE_MUTEXES` | 1 | 启用互斥量 |
| `configUSE_TIMERS` | 1 | 启用软件定时器 |

### 启用的功能模块

- ✅ 抢占式多任务调度
- ✅ 任务优先级管理
- ✅ 互斥量和信号量
- ✅ 消息队列
- ✅ 软件定时器
- ✅ 内存管理 (heap_4 算法)
- ✅ 堆栈溢出检测
- ✅ 运行时钩子函数

## 安装教程

### 环境要求

1. **开发工具**
   - Keil MDK-ARM V5.06 或更高版本
   - ARM Compiler V5 或 V6

2. **硬件要求**
   - N32L403KBQ 开发板或最小系统板
   - ST-Link 或 J-Link 调试器
   - USB 数据线

3. **软件包**
   - N32L40x_DFP 设备支持包

### 安装步骤

1. **安装 Keil MDK-ARM**
   ```
   下载并安装 Keil MDK-ARM 开发环境
   注册并激活许可证
   ```

2. **安装设备支持包**
   ```
   在 Keil 中打开 Pack Installer
   搜索并安装 Nationstech::N32L40x_DFP
   ```

3. **克隆项目**
   ```bash
   git clone [项目地址]
   cd FreeRTOS_N32L40X_TEM
   ```

4. **打开工程**
   ```
   使用 Keil MDK-ARM 打开 FreeRTOS_N32L40X_TEM.uvprojx
   ```

5. **编译工程**
   ```
   点击编译按钮或按 F7 编译工程
   确保编译无错误和警告
   ```

## 使用说明

### 快速开始

1. **连接硬件**
   - 将 N32L403KBQ 开发板通过调试器连接到 PC
   - 确保 PB0 引脚连接了 LED (低电平点亮)

2. **编译下载**
   ```
   在 Keil 中编译工程 (F7)
   下载程序到目标板 (F8)
   ```

3. **运行程序**
   - 复位开发板或点击运行
   - 观察 LED 以 1Hz 频率闪烁 (500ms 亮，500ms 灭)

### 代码结构说明

#### 主程序 (main.c)

```c
int main(void)
{
    /* 系统时钟配置 */
    SystemClock_Config();
    
    /* LED GPIO配置 */
    LED_GPIO_Config();
    
    /* 创建LED闪烁任务 */
    xTaskCreate(LED_Task,                    /* 任务函数 */
                "LED_Task",                  /* 任务名称 */
                LED_TASK_STACK_SIZE,         /* 任务堆栈大小 */
                NULL,                        /* 任务参数 */
                LED_TASK_PRIORITY,           /* 任务优先级 */
                NULL);                       /* 任务句柄 */
    
    /* 启动FreeRTOS调度器 */
    vTaskStartScheduler();
    
    while (1) { /* 不会执行到这里 */ }
}
```

#### LED 任务实现

```c
static void LED_Task(void *pvParameters)
{
    while (1)
    {
        LED_On();                           /* LED点亮 */
        vTaskDelay(pdMS_TO_TICKS(500));     /* 延时500ms */
        
        LED_Off();                          /* LED熄灭 */
        vTaskDelay(pdMS_TO_TICKS(500));     /* 延时500ms */
    }
}
```

### 自定义开发

#### 添加新任务

1. **定义任务函数**
   ```c
   static void MyTask(void *pvParameters)
   {
       while (1)
       {
           // 任务逻辑
           vTaskDelay(pdMS_TO_TICKS(100));  // 延时100ms
       }
   }
   ```

2. **创建任务**
   ```c
   xTaskCreate(MyTask, "MyTask", 128, NULL, 2, NULL);
   ```

#### 使用外设

1. **包含头文件**
   ```c
   #include "n32l40x_usart.h"  // USART外设
   #include "n32l40x_i2c.h"    // I2C外设
   ```

2. **初始化外设**
   ```c
   // 参考 firmware/n32l40x_std_periph_driver/src/ 中的示例
   ```

### 调试技巧

1. **使用断点调试**
   - 在关键代码处设置断点
   - 使用 Keil 调试器单步执行

2. **查看任务状态**
   ```c
   // 在调试时查看任务列表和堆栈使用情况
   vTaskList(pcWriteBuffer);
   vTaskGetRunTimeStats(pcWriteBuffer);
   ```

3. **内存使用监控**
   ```c
   // 查看剩余堆内存
   size_t freeHeap = xPortGetFreeHeapSize();
   ```

## 常见问题

### 编译问题

**Q: 编译时提示找不到头文件**
A: 检查工程设置中的包含路径是否正确配置

**Q: 链接时提示内存不足**
A: 检查 FreeRTOSConfig.h 中的堆大小设置，确保不超过芯片 SRAM 容量

### 运行问题

**Q: 程序下载后不运行**
A: 检查系统时钟配置，确保 HSE/HSI 配置正确

**Q: LED 不闪烁**
A: 检查 GPIO 配置和硬件连接，确保 PB0 连接了 LED

**Q: 系统死机或重启**
A: 检查堆栈溢出，增加任务堆栈大小或启用堆栈溢出检测

## 技术支持

### 相关资源

- [FreeRTOS 官方文档](https://www.freertos.org/Documentation/01-FreeRTOS-quick-start/01-Beginners-guide/02-Quick-start-guide)
- [N32L40X 系列数据手册](https://www.nationstech.com)
- [ARM Cortex-M4 技术参考手册](https://developer.arm.com/documentation/100166/0001)

### 开发工具

- **Keil MDK-ARM**: 主要开发环境
- **ST-Link Utility**: 程序下载和调试
- **串口调试助手**: 串口通信调试

## 版本历史

- **v1.0.0** (2024-12-xx)
  - 初始版本发布
  - 基于 FreeRTOS 202411.00
  - 支持 N32L403KBQ 芯片
  - 包含 LED 闪烁示例

## 许可证

本项目采用 MIT 许可证，详见 LICENSE 文件。

## 贡献指南

欢迎提交 Issue 和 Pull Request 来改进本项目：

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

---

**注意**: 本项目仅供学习和参考使用，在商业项目中使用前请仔细测试和验证。
