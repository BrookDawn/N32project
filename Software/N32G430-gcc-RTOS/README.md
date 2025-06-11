# N32G430C8L7 GCC Makefile 工程

这是一个基于GCC编译器和Makefile构建系统的N32G430C8L7微控制器工程模板。

## 工程结构

```
firmware/
├── Makefile                    # 主构建文件
├── n32g430c8l7_flash.ld      # 链接脚本
├── build.bat                  # Windows构建脚本
├── core/                      # 核心应用代码
│   ├── src/                   # 源代码目录
│   │   ├── main.c             # 主程序文件
│   │   └── stm32_assert.c     # 断言处理
│   └── inc/                   # 用户头文件目录
├── CMSIS/                     # CMSIS库文件
│   ├── device/                # 设备相关文件
│   │   ├── n32g430.h          # 设备头文件
│   │   ├── n32g430_conf.h     # 配置文件
│   │   ├── system_n32g430.c   # 系统初始化
│   │   └── startup/           # 启动文件
│   └── core/                  # ARM Cortex-M4 核心文件
└── n32g430_std_periph_driver/ # 标准外设驱动库
    ├── inc/                   # 驱动头文件
    └── src/                   # 驱动源文件
```

## 芯片特性

- **型号**: N32G430C8L7
- **内核**: ARM Cortex-M4F @ 144MHz
- **Flash**: 64KB
- **SRAM**: 32KB
- **封装**: LQFP48

## 前置要求

1. **GCC ARM工具链**: 需要安装arm-none-eabi-gcc
   ```bash
   # Ubuntu/Debian
   sudo apt-get install gcc-arm-none-eabi

   # Windows
   # 下载并安装 GNU Arm Embedded Toolchain
   ```

2. **Make工具**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install make

   # Windows
   # 可以使用MSYS2或者MinGW
   ```

## 编译说明

### 基本编译
```bash
make
```

### 清理构建文件
```bash
make clean
```

### 指定GCC路径（可选）
```bash
make GCC_PATH=/path/to/your/gcc/bin
```

## 输出文件

编译成功后，在`build/`目录下会生成以下文件：
- `n32g430c8l7_project.elf` - ELF可执行文件
- `n32g430c8l7_project.hex` - Intel HEX格式文件
- `n32g430c8l7_project.bin` - 二进制文件
- `n32g430c8l7_project.map` - 链接映射文件

## 烧录方法

### 1. 使用OpenOCD
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program build/n32g430c8l7_project.elf verify reset exit"
```

### 2. 使用ST-Link
```bash
st-flash write build/n32g430c8l7_project.bin 0x08000000
```

### 3. 使用J-Link
```bash
JLinkExe -device N32G430C8L7 -if SWD -speed 4000 -autoconnect 1
J-Link> loadfile build/n32g430c8l7_project.hex
J-Link> r
J-Link> g
```

## 示例程序说明

默认的`main.c`程序实现了以下功能：
1. 系统时钟配置为144MHz（使用外部8MHz晶振）
2. 配置PC13引脚作为输出（通常连接板载LED）
3. 无限循环中切换LED状态，实现闪烁效果

## 添加新文件

### 添加C源文件
1. 将源文件放入`core/src/`目录
2. 在Makefile的`C_SOURCES`变量中添加文件路径

### 添加头文件
1. 将头文件放入`core/inc/`目录
2. 确保`C_INCLUDES`变量包含`-Icore/inc`

### 使用标准外设驱动
标准外设驱动已经包含在Makefile中，可以直接使用：
```c
#include "n32g430_gpio.h"
#include "n32g430_rcc.h"
#include "n32g430_usart.h"
// 等等...
```

## 调试配置

### GDB调试
```bash
# 启动OpenOCD（在一个终端中）
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# 启动GDB（在另一个终端中）
arm-none-eabi-gdb build/n32g430c8l7_project.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

## 配置选项

### 编译选项
- `DEBUG=1` - 启用调试信息（默认）
- `OPT=-Og` - 优化级别（调试优化）

### 芯片定义
- `N32G430C8L7` - 芯片型号定义
- `USE_STDPERIPH_DRIVER` - 使用标准外设驱动

## 常见问题

### 1. 编译错误"command not found"
确保arm-none-eabi-gcc在系统PATH中，或使用GCC_PATH指定路径。

### 2. 链接错误"undefined reference"
检查是否所有需要的源文件都添加到了Makefile的C_SOURCES变量中。

### 3. 烧录失败
确保开发板正确连接，SWD/JTAG接口工作正常。

## 许可证

本工程基于Nations N32G430标准外设库构建，请遵循相应的许可证条款。
