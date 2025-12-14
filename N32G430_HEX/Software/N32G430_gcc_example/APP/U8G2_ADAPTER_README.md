# U8g2 OLED驱动适配层使用说明

## 概述

本适配层将现有的OLED驱动与U8g2图形库集成，支持SSD1306和SH1106两种驱动芯片。

## 文件结构

```
APP/
├── Inc/
│   ├── oled_u8g2_adapter.h    # U8g2适配层头文件
│   ├── oled_u8g2_ui.h         # UI绘制示例头文件
│   └── oled_u8g2_example.h    # 使用示例头文件
├── Src/
│   ├── oled_u8g2_adapter.c    # U8g2适配层实现
│   ├── oled_u8g2_ui.c         # UI绘制示例实现
│   └── oled_u8g2_example.c    # 使用示例实现
└── u8g2/                      # U8g2库核心文件（已提取）
    ├── u8g2.h
    ├── u8x8.h
    ├── u8g2_*.c               # U8g2核心源代码
    └── u8x8_*.c               # U8x8底层驱动代码
```

## 编译配置

### 1. 添加u8g2源文件到工程

需要将以下u8g2源文件添加到编译系统中：

**核心文件（必需）：**
- `APP/u8g2/u8g2_setup.c`
- `APP/u8g2/u8g2_d_setup.c`
- `APP/u8g2/u8g2_d_memory.c`
- `APP/u8g2/u8g2_buffer.c`
- `APP/u8g2/u8g2_font.c`
- `APP/u8g2/u8g2_fonts.c`
- `APP/u8g2/u8g2_hvline.c`
- `APP/u8g2/u8g2_ll_hvline.c`
- `APP/u8g2/u8g2_box.c`
- `APP/u8g2/u8g2_circle.c`
- `APP/u8g2/u8g2_line.c`
- `APP/u8g2/u8g2_polygon.c`
- `APP/u8g2/u8g2_bitmap.c`
- `APP/u8g2/u8g2_intersection.c`
- `APP/u8g2/u8g2_cleardisplay.c`
- `APP/u8g2/u8x8_setup.c`
- `APP/u8g2/u8x8_d_ssd1306_128x64_noname.c`  (SSD1306和SH1106驱动)
- `APP/u8g2/u8x8_cad.c`
- `APP/u8g2/u8x8_byte.c`
- `APP/u8g2/u8x8_8x8.c`

**适配层文件：**
- `APP/Src/oled_u8g2_adapter.c`
- `APP/Src/oled_u8g2_ui.c`
- `APP/Src/oled_u8g2_example.c`

### 2. 添加头文件包含路径

在编译器设置中添加以下头文件路径：
- `APP/u8g2`
- `APP/Inc`

### 3. Makefile配置示例

如果使用Makefile，可以参考以下配置：

```makefile
# C源文件
C_SOURCES += \
APP/Src/oled_u8g2_adapter.c \
APP/Src/oled_u8g2_ui.c \
APP/Src/oled_u8g2_example.c \
APP/u8g2/u8g2_setup.c \
APP/u8g2/u8g2_d_setup.c \
APP/u8g2/u8g2_d_memory.c \
APP/u8g2/u8g2_buffer.c \
APP/u8g2/u8g2_font.c \
APP/u8g2/u8g2_fonts.c \
APP/u8g2/u8g2_hvline.c \
APP/u8g2/u8g2_ll_hvline.c \
APP/u8g2/u8g2_box.c \
APP/u8g2/u8g2_circle.c \
APP/u8g2/u8g2_line.c \
APP/u8g2/u8g2_polygon.c \
APP/u8g2/u8g2_bitmap.c \
APP/u8g2/u8g2_intersection.c \
APP/u8g2/u8g2_cleardisplay.c \
APP/u8g2/u8x8_setup.c \
APP/u8g2/u8x8_d_ssd1306_128x64_noname.c \
APP/u8g2/u8x8_cad.c \
APP/u8g2/u8x8_byte.c \
APP/u8g2/u8x8_8x8.c

# 头文件包含路径
C_INCLUDES += \
-IAPP/u8g2 \
-IAPP/Inc
```

## 快速开始

### 1. 在main.c中使用

```c
#include "oled_u8g2_example.h"

int main(void)
{
    /* HAL初始化 */
    HAL_Init();
    SystemClock_Config();

    /* 初始化I2C */
    MX_I2C1_Init();

    /* 初始化U8g2 OLED */
    U8G2_OLED_Example_Init();

    while (1)
    {
        /* 更新显示（可选） */
        // U8G2_OLED_Example_Update();
        // HAL_Delay(100);
    }
}
```

### 2. 选择芯片类型

在 `oled_u8g2_example.c` 的 `U8G2_OLED_Example_Init()` 函数中：

**使用SSD1306：**
```c
U8G2_OLED_Init_SSD1306_128x64_I2C(&g_u8g2_oled, &hi2c1, 0x78);
```

**使用SH1106：**
```c
U8G2_OLED_Init_SH1106_128x64_I2C(&g_u8g2_oled, &hi2c1, 0x78);
```

### 3. 自定义UI绘制

```c
#include "oled_u8g2_adapter.h"

void my_custom_draw(void)
{
    /* 获取u8g2结构体 */
    extern U8G2_OLED_HandleTypeDef g_u8g2_oled;
    u8g2_t *u8g2 = U8G2_OLED_GetU8g2(&g_u8g2_oled);

    if(u8g2 != NULL)
    {
        /* 清空缓冲区 */
        u8g2_ClearBuffer(u8g2);

        /* 设置字体 */
        u8g2_SetFont(u8g2, u8g2_font_ncenB14_tr);

        /* 绘制文字 */
        u8g2_DrawStr(u8g2, 0, 20, "Hello World!");

        /* 绘制图形 */
        u8g2_DrawCircle(u8g2, 64, 32, 20, U8G2_DRAW_ALL);

        /* 发送到显示屏 */
        u8g2_SendBuffer(u8g2);
    }
}
```

## U8g2常用API

### 缓冲区操作
- `u8g2_ClearBuffer(u8g2)` - 清空缓冲区
- `u8g2_SendBuffer(u8g2)` - 发送缓冲区到显示屏

### 文字绘制
- `u8g2_SetFont(u8g2, font)` - 设置字体
- `u8g2_DrawStr(u8g2, x, y, str)` - 绘制字符串
- `u8g2_DrawUTF8(u8g2, x, y, str)` - 绘制UTF-8字符串

### 图形绘制
- `u8g2_DrawPixel(u8g2, x, y)` - 绘制像素点
- `u8g2_DrawLine(u8g2, x1, y1, x2, y2)` - 绘制直线
- `u8g2_DrawBox(u8g2, x, y, w, h)` - 绘制实心矩形
- `u8g2_DrawFrame(u8g2, x, y, w, h)` - 绘制空心矩形
- `u8g2_DrawCircle(u8g2, x, y, r, opt)` - 绘制空心圆
- `u8g2_DrawDisc(u8g2, x, y, r, opt)` - 绘制实心圆
- `u8g2_DrawXBM(u8g2, x, y, w, h, bitmap)` - 绘制XBM位图

### 显示控制
- `u8g2_SetContrast(u8g2, value)` - 设置对比度(0-255)
- `u8g2_SetPowerSave(u8g2, is_enable)` - 省电模式(0=关闭，1=开启)

## 注意事项

1. **I2C地址**：确保使用正确的I2C地址（常见：0x78或0x7A）

2. **内存占用**：全缓冲模式(_f结尾的初始化函数)占用1024字节RAM（128x64屏幕）

3. **延时精度**：`u8x8_gpio_and_delay`函数中的微秒延时使用NOP指令实现，精度取决于CPU频率

4. **字体选择**：u8g2提供大量内置字体，选择合适的字体以节省Flash空间

5. **位图格式**：使用XBM格式的位图数据

## 示例UI效果

提供的示例UI包含：
- 菜单导航图标
- 多种字体大小的文本
- 自定义位图图标
- 状态提示信息

可以根据实际需求修改 `U8G2_Draw_Example_UI()` 函数来自定义UI。

## 常见问题

**Q: 显示屏没有显示？**
- 检查I2C地址是否正确
- 检查I2C总线硬件连接
- 确认选择了正确的芯片类型（SSD1306或SH1106）

**Q: 显示偏移或错位？**
- SH1106芯片可能需要列偏移，尝试使用带vcomh0或winstar变体的初始化函数

**Q: 编译错误：找不到u8g2.h？**
- 确保已添加 `APP/u8g2` 到头文件包含路径

## 技术支持

如有问题，请参考：
- U8g2官方文档：https://github.com/olikraus/u8g2
- U8g2 Wiki：https://github.com/olikraus/u8g2/wiki
