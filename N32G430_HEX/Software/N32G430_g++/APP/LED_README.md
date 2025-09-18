# LED控制类系统

## 概述

本LED控制系统采用分层架构设计，实现了硬件抽象层（HAL），使LED控制与具体的MCU平台解耦。系统支持多种LED操作，包括开关控制、状态切换、闪烁控制等功能。

## 系统架构

```
应用层 (main.cpp)
    ↓
LED抽象层 (Led.h/Led.cpp)
    ↓
硬件适配层 (LedAdapter_N32G430.h/LedAdapter_N32G430.cpp)
    ↓
BSP层 (bsp_led.h/bsp_led.c)
    ↓
硬件层 (N32G430 GPIO)
```

## 文件结构

### 核心文件
- `APP/Ins/Led.h` - LED控制类接口定义
- `APP/Src/Led.cpp` - LED控制类实现
- `APP/Ins/LedAdapter_N32G430.h` - N32G430平台适配器头文件
- `APP/Src/LedAdapter_N32G430.cpp` - N32G430平台适配器实现

### 特性

1. **硬件抽象**: 通过接口抽象，支持不同MCU平台
2. **状态管理**: 自动跟踪LED状态
3. **闪烁控制**: 支持可配置的闪烁周期
4. **错误处理**: 完善的错误代码和状态检查
5. **资源管理**: 自动初始化和清理资源

## 使用方法

### 1. 系统初始化

```cpp
#include "Led.h"
#include "LedAdapter_N32G430.h"

int main(void) {
    // 初始化LED系统
    if (LedAdapter_N32G430_InitAllLeds() == LED_OK) {
        // 系统初始化成功
    }
}
```

### 2. 基本LED控制

```cpp
// 开启LED
Led_On(LED_ID_1);

// 关闭LED
Led_Off(LED_ID_1);

// 切换LED状态
Led_Toggle(LED_ID_1);

// 获取LED状态
LedState_t state = Led_GetState(LED_ID_1);
```

### 3. 闪烁控制

```cpp
// 设置闪烁周期
Led_SetBlinkPeriod(LED_ID_STATUS, LED_BLINK_SLOW);   // 1000ms
Led_SetBlinkPeriod(LED_ID_ERROR, LED_BLINK_FAST);    // 100ms
Led_SetBlinkPeriod(LED_ID_COMM, LED_BLINK_NORMAL);   // 500ms

// 在主循环或定时器中更新闪烁状态
uint32_t current_time = get_system_time_ms();
Led_BlinkUpdate(LED_ID_STATUS, current_time);
```

### 4. 自定义LED配置

```cpp
LedConfig_t config = {
    .led_id = LED_ID_1,
    .active_low = false,        // 高电平有效
    .blink_period = 200         // 200ms闪烁周期
};

Led_Init(LED_ID_1, &config);
```

## 预定义LED ID

```cpp
#define LED_ID_1        0       // 通用LED1
#define LED_ID_2        1       // 通用LED2  
#define LED_ID_3        2       // 通用LED3
#define LED_ID_STATUS   LED_ID_1  // 状态指示LED
#define LED_ID_ERROR    LED_ID_2  // 错误指示LED
#define LED_ID_COMM     LED_ID_3  // 通信指示LED
```

## 预定义闪烁周期

```cpp
#define LED_BLINK_FAST    100   // 快速闪烁 100ms
#define LED_BLINK_NORMAL  500   // 正常闪烁 500ms
#define LED_BLINK_SLOW    1000  // 慢速闪烁 1000ms
```

## 错误代码

```cpp
typedef enum {
    LED_OK = 0,                 // 操作成功
    LED_ERROR = -1,             // 一般错误
    LED_ERROR_INVALID_ID = -2,  // 无效的LED ID
    LED_ERROR_NOT_INIT = -3     // LED未初始化
} LedError_t;
```

## 移植到其他平台

要移植到新的MCU平台，需要：

1. 创建新的适配器文件 `LedAdapter_XXX.cpp`
2. 实现 `LedInterface_t` 接口中的所有函数
3. 配置硬件相关的GPIO设置
4. 注册新的硬件接口

### 适配器接口

```cpp
typedef struct {
    LedError_t (*init)(uint8_t led_id, const LedConfig_t* config);
    LedError_t (*set_state)(uint8_t led_id, LedState_t state);
    LedState_t (*get_state)(uint8_t led_id);
    LedError_t (*deinit)(uint8_t led_id);
} LedInterface_t;
```

## 注意事项

1. **线程安全**: 当前实现不是线程安全的，如需在多线程环境使用，请添加互斥锁
2. **定时器**: 闪烁功能需要定期调用 `Led_BlinkUpdate()`，建议使用系统定时器
3. **资源限制**: 最大支持8个LED（可通过修改 `MAX_LED_COUNT` 调整）
4. **内存使用**: 每个LED控制器占用约40字节内存

## 示例代码

完整的使用示例请参考 `core/src/main.cpp` 文件。

## 版本历史

- v1.0.0 (2025-09-18): 初始版本，支持N32G430平台
