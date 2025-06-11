# N32G430 FreeRTOS 工程

这是一个基于国民技术 N32G430 微控制器的 FreeRTOS 工程示例。本工程展示了如何在 N32G430 裸机工程的基础上集成 FreeRTOS 操作系统。

## 开发环境

- **编译器**：GCC ARM
- **操作系统**：Windows 10
- **调试器**：J-Link
- **构建系统**：Makefile

## 硬件资源

- **主控芯片**：N32G430C8L7
- **内核**：ARM Cortex-M4F（带FPU）
- **时钟**：主频最高可达144MHz
- **Flash**：64KB
- **SRAM**：32KB
- **封装**：LQFP48

## 工程特点

1. 完整的 FreeRTOS 实时操作系统集成
2. 正确配置的硬件浮点单元（FPU）支持
3. 优化的中断优先级配置
4. 标准的 Makefile 构建系统
5. 包含完整的外设驱动库

## 目录结构

```
N32G430-gcc-RTOS/
├── README.md                       # 项目说明文档
├── Makefile                        # 主构建文件
├── build.bat                       # Windows批处理构建脚本
├── n32g430c8l7_flash.ld            # 链接脚本
├── build/                          # 编译输出目录
│   ├── *.o                         # 目标文件
│   ├── *.elf                       # 可执行文件
│   ├── *.hex                       # HEX格式文件
│   └── *.bin                       # 二进制文件
├── core/                           # 用户应用代码
│   ├── src/                        # 用户源代码
│   │   └── main.c                  # 主程序文件
│   └── inc/                        # 用户头文件
├── FreeRTOS/                       # FreeRTOS 操作系统源码
│   ├── Inc/                        # FreeRTOS 头文件
│   ├── Src/                        # FreeRTOS 核心源文件
│   ├── MemMang/                    # 内存管理源文件
│   └── portable/                   # 平台相关移植文件
├── CMSIS/                          # ARM CMSIS 库
│   ├── device/                     # N32G430 设备相关文件
│   │   ├── n32g430.h               # 设备头文件
│   │   ├── n32g430_conf.h          # 配置文件
│   │   ├── system_n32g430.c        # 系统初始化
│   │   └── startup/                # 启动文件
│   └── core/                       # ARM Cortex-M4 核心文件
└── n32g430_std_periph_driver/      # 标准外设驱动库
    ├── inc/                        # 驱动头文件
    └── src/                        # 驱动源文件
```

## 关键配置

### FPU 配置

工程使用以下 FPU 相关配置：
- **编译选项**：`-mfloat-abi=hard -mfpu=fpv4-sp-d16`
- **宏定义**：
  - `__FPU_PRESENT=1`
  - `__FPU_USED=1`

### 中断优先级配置

为确保 FreeRTOS 正常运行，配置了以下中断优先级：
- **PendSV**：最低优先级（15）
- **SysTick**：次低优先级（14）
- **其他外设中断**：根据实际需求配置（0-13）

### FreeRTOS 配置

- **任务调度**：抢占式调度
- **系统时钟节拍**：1000Hz
- **内存管理**：heap_4.c
- **最大任务优先级**：5

## 编译说明

### 前置要求

1. **GCC ARM工具链**：需要安装 arm-none-eabi-gcc
2. **Make工具**：Windows 环境可使用 MSYS2 或 MinGW

### 编译步骤

1. 确保已安装 ARM GCC 工具链
2. 在工程根目录下执行：
   ```bash
   make
   ```
   或者在 Windows 环境下执行：
   ```cmd
   build.bat
   ```
3. 编译成功后将在 `build/` 目录下生成以下文件：
   - `*.elf` - ELF可执行文件
   - `*.hex` - Intel HEX格式文件
   - `*.bin` - 二进制文件
   - `*.map` - 链接映射文件

### 清理构建文件

```bash
make clean
```

## 烧录和调试

### 使用 J-Link

```bash
JLinkExe -device N32G430C8L7 -if SWD -speed 4000 -autoconnect 1
J-Link> loadfile build/*.hex
J-Link> r
J-Link> g
```

### 使用 OpenOCD

```bash
openocd -f interface/jlink.cfg -f target/stm32f4x.cfg -c "program build/*.elf verify reset exit"
```

## 使用说明

### 创建新任务

```c
#include "FreeRTOS.h"
#include "task.h"

void vTaskFunction(void *pvParameters)
{
    for(;;)
    {
        // 任务代码
        vTaskDelay(pdMS_TO_TICKS(100)); // 延时100ms
    }
}

// 在main函数中创建任务
xTaskCreate(vTaskFunction, "TaskName", 128, NULL, 1, NULL);
```

### 使用外设驱动

```c
#include "n32g430_gpio.h"
#include "n32g430_rcc.h"

// 初始化GPIO
RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
GPIO_InitType GPIO_InitStructure;
GPIO_InitStruct(&GPIO_InitStructure);
GPIO_InitStructure.Pin = GPIO_PIN_13;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
```

## 注意事项

1. **中断向量表修改**：使用 FreeRTOS 时，系统中断向量表已被修改，原有的 `SysTick_Handler` 被替换为 `xPortSysTickHandler`
2. **FPU 配置一致性**：请确保 FPU 相关配置正确，避免硬件配置与编译器选项不匹配
3. **任务栈空间**：创建任务时注意栈空间的合理分配，防止栈溢出
4. **中断优先级**：确保使用 FreeRTOS API 的中断优先级在 `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` 以下

## 常见问题

### 1. FPU 相关编译错误

如果出现 FPU 相关的编译错误，请检查：
- 编译器的 FPU 选项配置是否正确
- FPU 相关宏定义是否正确
- 是否正确包含了 FPU 相关的头文件

### 2. 系统 HardFault

如果系统出现 HardFault，请检查：
- 中断优先级配置是否正确
- 任务栈空间是否足够
- 中断向量表配置是否正确
- 是否在中断中调用了不允许的 FreeRTOS API

### 3. 任务无法正常运行

请检查：
- 是否正确调用了 `vTaskStartScheduler()`
- 任务优先级配置是否合理
- 系统时钟配置是否正确

## 版本历史

- **v1.0** - 初始版本，包含基本的 FreeRTOS 集成
- **v1.1** - 修复 FPU 配置问题
- **v1.2** - 优化中断优先级配置

## 贡献

欢迎提交问题和改进建议。请遵循以下步骤：
1. Fork 本项目
2. 创建功能分支
3. 提交更改
4. 创建 Pull Request

## 许可

本项目基于 MIT 许可证发布。详情请参阅 LICENSE 文件。
