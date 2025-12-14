# U8g2 OLED UI系统使用说明

## 概述

本UI系统基于u8g2图形库，提供了一个完整的三级菜单管理系统和固定区域布局的UI框架。

## UI布局

### 屏幕区域划分 (128x64像素)

```
┌────────────────────────────────────────────────────┐
│ 菜单栏区域 (0, 0, 128, 12)                         │
│ Menu > GasType > Edit                              │
├────────────────────────────────────────────────────┤
│           │                        │               │
│  左图标   │   主显示区域           │   右图标      │
│  (6,22)   │   (44, 20, 60, 30)     │   (104,25)   │
│   [↓]     │      CH_4              │     [→]      │
│           │                        │               │
├────────────────────────────────────────────────────┤
│ 信息区域 (1, 59, 126, 5)                           │
│ save success                                       │
└────────────────────────────────────────────────────┘
```

### 各区域说明

1. **菜单栏区域** (顶部12像素)
   - 显示当前菜单路径
   - 三级菜单：Menu → GasType → Edit
   - 使用箭头图标分隔各级菜单

2. **左图标区域** (6, 22, 14x15)
   - 显示左键功能图标
   - 向下箭头：表示遍历当前菜单选项

3. **右图标区域** (104, 25, 16x9)
   - 显示右键功能图标
   - 确认图标：表示确认/进入下级菜单

4. **主显示区域** (44, 20, 60x30)
   - 显示主要内容
   - 示例：CH_4（气体类型和编号）
   - 使用大字体显示

5. **信息区域** (1, 59, 126x5)
   - 显示状态信息、提示等
   - 示例：save success

## 快速开始

### 1. 在main.c中初始化

```c
#include "oled_u8g2_example.h"

int main(void)
{
    /* HAL和外设初始化 */
    HAL_Init();
    SystemClock_Config();
    MX_I2C1_Init();

    /* 初始化OLED UI系统 */
    U8G2_OLED_Example_Init();

    while (1)
    {
        /* 主循环 */
    }
}
```

### 2. 处理按键输入

在按键中断或轮询函数中调用：

```c
/* 左键按下：遍历当前级别的选项 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == LEFT_KEY_Pin)
    {
        U8G2_OLED_HandleLeftKeyPress();
    }
    else if(GPIO_Pin == RIGHT_KEY_Pin)
    {
        U8G2_OLED_HandleRightKeyPress();
    }
}
```

### 3. 更新显示内容

```c
/* 更新主显示区域 */
U8G2_OLED_UpdateMainDisplay("CH", "4");

/* 更新信息区域 */
U8G2_OLED_UpdateInfo("save success");
```

## 菜单系统

### 菜单层级

```
Menu (一级菜单)
  └─> GasType (二级菜单)
        └─> Edit (三级菜单)
```

### 按键功能

- **左键（向下箭头图标）**: 在当前菜单级别中向下遍历选项
- **右键（确认图标）**: 确认当前选项并进入下一级菜单

### 菜单导航示例

```
初始状态: Menu
├─ 按右键 → 进入 GasType 级别
│   ├─ 按左键 → 遍历 GasType 下的选项
│   └─ 按右键 → 进入 Edit 级别
│       ├─ 按左键 → 遍历 Edit 下的选项
│       └─ 按右键 → 执行确认操作
```

## API参考

### 初始化函数

```c
void U8G2_OLED_Example_Init(void);
```
初始化OLED硬件和UI系统。

### 按键处理函数

```c
void U8G2_OLED_HandleLeftKeyPress(void);
void U8G2_OLED_HandleRightKeyPress(void);
```
处理左键和右键按下事件。

### 内容更新函数

```c
void U8G2_OLED_UpdateMainDisplay(const char *text, const char *number);
void U8G2_OLED_UpdateInfo(const char *info);
```
更新主显示区域和信息区域的内容。

### UI状态访问

```c
UI_State_t* U8G2_OLED_GetUIState(void);
```
获取UI状态结构体，用于高级自定义。

## 高级用法

### 自定义菜单逻辑

修改 `APP/Src/oled_u8g2_ui.c` 中的以下函数：

```c
void UI_HandleLeftKey(UI_State_t *ui_state)
{
    /* 自定义左键处理逻辑 */
    // 根据当前菜单级别执行不同操作
    switch(ui_state->current_level)
    {
        case MENU_LEVEL_1:
            // 处理一级菜单的遍历
            break;
        case MENU_LEVEL_2:
            // 处理二级菜单的遍历
            break;
        case MENU_LEVEL_3:
            // 处理三级菜单的遍历
            break;
    }
}

void UI_HandleRightKey(UI_State_t *ui_state)
{
    /* 自定义右键处理逻辑 */
    // 进入下级菜单或执行确认操作
}
```

### 直接使用UI API

```c
#include "oled_u8g2_adapter.h"
#include "oled_u8g2_ui.h"

/* 获取UI状态 */
UI_State_t *ui_state = U8G2_OLED_GetUIState();

/* 修改菜单级别 */
ui_state->current_level = MENU_LEVEL_2;

/* 修改主显示内容 */
UI_SetMainDisplay(ui_state, "O2", "21");

/* 修改信息 */
UI_SetInfo(ui_state, "Measuring...");

/* 获取u8g2对象并绘制 */
extern U8G2_OLED_HandleTypeDef g_u8g2_oled;
u8g2_t *u8g2 = U8G2_OLED_GetU8g2(&g_u8g2_oled);
UI_Draw(u8g2, ui_state);
```

## UI区域位置宏定义

所有区域位置都在 `oled_u8g2_ui.h` 中定义，可根据需要调整：

```c
/* 菜单栏 */
#define UI_REGION_MENU_BAR_X        0
#define UI_REGION_MENU_BAR_Y        0
#define UI_REGION_MENU_BAR_H        12

/* 左图标 */
#define UI_REGION_LEFT_ICON_X       6
#define UI_REGION_LEFT_ICON_Y       22
#define UI_REGION_LEFT_ICON_W       14
#define UI_REGION_LEFT_ICON_H       15

/* 右图标 */
#define UI_REGION_RIGHT_ICON_X      104
#define UI_REGION_RIGHT_ICON_Y      25
#define UI_REGION_RIGHT_ICON_W      16
#define UI_REGION_RIGHT_ICON_H      9

/* 主显示区 */
#define UI_REGION_MAIN_DISPLAY_X    44
#define UI_REGION_MAIN_DISPLAY_Y    20
#define UI_REGION_MAIN_DISPLAY_W    60
#define UI_REGION_MAIN_DISPLAY_H    30

/* 信息区 */
#define UI_REGION_INFO_X            1
#define UI_REGION_INFO_Y            59
#define UI_REGION_INFO_W            126
#define UI_REGION_INFO_H            5
```

## 注意事项

1. **菜单层级限制**: 目前支持三级菜单，如需更多层级，需修改 `MenuLevel_t` 枚举

2. **字符串长度**:
   - 主文本：最多15字符
   - 数字文本：最多7字符
   - 信息文本：最多31字符

3. **字体选择**:
   - 菜单栏：`u8g2_font_t0_12_tr`
   - 主文本：`u8g2_font_profont29_tr`
   - 数字：`u8g2_font_profont12_tr`
   - 信息区：`u8g2_font_timR10_tr`

4. **图标位图**: 三个图标的位图数据在 `oled_u8g2_ui.c` 中定义，可以替换为自定义图标

## 示例应用

### 气体检测显示

```c
/* 初始化 */
U8G2_OLED_Example_Init();

/* 显示CH4气体，浓度4% */
U8G2_OLED_UpdateMainDisplay("CH", "4");
U8G2_OLED_UpdateInfo("Normal");

/* 用户按右键进入GasType菜单 */
// 按键中断自动调用 U8G2_OLED_HandleRightKeyPress()

/* 用户按左键选择不同气体类型 */
// 按键中断自动调用 U8G2_OLED_HandleLeftKeyPress()
```

## 故障排查

**Q: 显示不更新？**
- 确保调用了 `UI_Draw()` 函数
- 检查I2C通信是否正常

**Q: 菜单无法切换？**
- 确认按键中断正常工作
- 检查 `UI_HandleLeftKey()` 和 `UI_HandleRightKey()` 是否被调用

**Q: 显示位置不正确？**
- 检查 `oled_u8g2_ui.h` 中的区域位置宏定义
- 确认使用的字体大小是否匹配

## 文件结构

```
APP/
├── Inc/
│   ├── oled_u8g2_ui.h         # UI系统头文件
│   └── oled_u8g2_example.h    # 使用示例头文件
└── Src/
    ├── oled_u8g2_ui.c         # UI系统实现
    └── oled_u8g2_example.c    # 使用示例实现
```
