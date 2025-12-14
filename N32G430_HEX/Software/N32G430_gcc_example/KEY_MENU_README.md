# 按键和菜单系统说明

## 按键配置

本项目使用两个按键进行交互：
- **PB10**: 左键 (KEY_LEFT)
- **PB11**: 右键 (KEY_RIGHT)

### 按键功能

按键支持短按和长按两种操作：

#### 1. 主页面 (PAGE_MAIN)
- **左键短按**: 进入菜单页面
- **右键短按**: 进入气体类型选择页面
- **左键长按**: 直接进入设置页面（快捷方式）
- **右键长按**: 保留功能

#### 2. 菜单页面 (PAGE_MENU)
- **左键短按**: 切换到下一个菜单项（GasType, Settings, About）
- **右键短按**: 确认并进入选中的子页面
- **任意键长按**: 返回主页面

#### 3. 气体类型页面 (PAGE_GAS_TYPE)
- **左键短按**: 切换气体类型（CH4, O2, CO, CO2, H2S）
- **右键短按**: 进入编辑页面
- **任意键长按**: 返回主页面

#### 4. 编辑页面 (PAGE_EDIT)
- **左键短按**: 取消并返回上一页（不保存）
- **右键短按**: 保存并返回主页面
- **左键长按**: 取消并返回主页面（快捷方式）

#### 5. 设置页面 (PAGE_SETTINGS)
- **左键短按**: 切换设置项（Brightness, Contrast, Backlight, Reset）
- **右键短按**:
  - Brightness: 增加亮度值（0-255，步进32）
  - Contrast: 增加对比度值（0-255，步进32）
  - Backlight: 切换背光开关（ON/OFF）
  - Reset: 恢复默认值并返回主页面
- **任意键长按**: 返回主页面

## 按键硬件特性

### 去抖动
- 硬件去抖动时间：50ms
- 在此时间内的重复触发将被忽略

### 长按检测
- 长按时间阈值：1000ms (1秒)
- 长按检测通过主循环中的`BSP_KEY_CheckLongPress()`函数实现
- 长按触发后，释放按键时不会触发短按事件

### 中断配置
- EXTI中断线：EXTI10（PB10），EXTI11（PB11）
- 触发方式：双边沿触发（Rising_Falling）
- 优先级：抢占优先级2，子优先级0

## 视觉增强

菜单系统包含以下视觉增强：

1. **高亮框**: 在菜单和设置页面显示当前选中项的圆角矩形框
2. **面包屑导航**: 显示当前页面路径（Menu > Settings）
3. **操作提示**: 底部显示可用操作（< Next  Enter >）
4. **图标指示**: 左右按键的图标提示

## 代码结构

```
core/
├── inc/
│   └── bsp_key.h          # 按键BSP头文件
└── src/
    ├── bsp_key.c          # 按键BSP实现
    ├── main.c             # 主函数（初始化和主循环）
    └── n32g430_it.c       # 中断服务函数

APP/
├── Inc/
│   ├── oled_app.h         # OLED应用层头文件
│   ├── oled_page_manager.h # 页面管理器头文件
│   └── oled_pages.h       # 所有页面定义头文件
└── Src/
    ├── oled_app.c         # OLED应用层实现
    ├── oled_page_manager.c # 页面管理器实现
    └── oled_pages.c       # 所有页面实现
```

## 使用方法

1. **初始化**:
```c
BSP_KEY_Init();        // 初始化按键硬件
OLED_App_Init();       // 初始化OLED和页面管理器
```

2. **主循环**:
```c
while(1) {
    OLED_App_Update();  // 检查长按事件
    HAL_Delay(10);      // 减少CPU使用
}
```

3. **中断处理**:
   - EXTI中断自动触发
   - 通过`OLED_App_KeyCallback()`回调应用层
   - 页面管理器处理按键事件并更新显示

## 自定义页面

要添加新页面：

1. 在`oled_page_manager.h`中添加页面ID到`PageID_t`枚举
2. 在`oled_pages.c`中实现页面函数：
   - `Page_XXX_Init()`: 初始化（可选）
   - `Page_XXX_Enter()`: 进入页面时调用（可选）
   - `Page_XXX_Exit()`: 退出页面时调用（可选）
   - `Page_XXX_Draw()`: 绘制页面（必须）
   - `Page_XXX_HandleKey()`: 处理按键（必须）
3. 创建`PageDef_t`结构体
4. 在`Pages_RegisterAll()`中注册新页面

## 注意事项

1. 所有长按事件由主循环检测，确保主循环定期调用`OLED_App_Update()`
2. 页面切换时会自动调用前一页面的exit回调和新页面的enter回调
3. 使用`PageManager_RequestRedraw()`来触发页面重绘
4. 按键处理函数应尽可能快速，避免阻塞中断
