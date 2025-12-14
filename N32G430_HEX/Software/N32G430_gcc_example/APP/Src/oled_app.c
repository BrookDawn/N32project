/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_app.c
  * @brief   OLED应用层实现 - 页面管理器应用
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "oled_app.h"
#include "oled_u8g2_adapter.h"
#include "oled_page_manager.h"
#include "oled_pages.h"
#include "oled_diagnose.h"
#include "i2c.h"
#include "hal_compat.h"

/* Private variables ---------------------------------------------------------*/

/* U8g2驱动句柄 */
static U8G2_OLED_HandleTypeDef g_u8g2_oled;

/* 页面管理器（全局，供各页面使用） */
PageManager_t g_page_manager;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  初始化OLED应用
  */
void OLED_App_Init(void)
{
    OLED_DiagnoseResult_t diag_result;
    uint8_t i2c_addr = 0x78;  /* 默认地址 */
    uint8_t status;

    /* 先诊断I2C和OLED连接 */
    status = OLED_Diagnose(&hi2c1, &diag_result);

    if(status == 0 && diag_result.device_detected)
    {
        /* 找到OLED设备，使用检测到的地址 */
        i2c_addr = diag_result.detected_address << 1;  /* 转换为8位写地址 */
    }
    else
    {
        /* 未找到设备，可能是地址问题，尝试常见地址 */
        /* 大多数OLED使用0x3C (7位) 或 0x78 (8位写地址) */
        i2c_addr = 0x78;
    }

    /* 短暂延时确保I2C总线稳定 */
    HAL_Delay(50);

    /* 初始化OLED硬件驱动 */
    /* 方式1: 使用SSD1306 128x64 I2C */
    U8G2_OLED_Init_SSD1306_128x64_I2C(&g_u8g2_oled, &hi2c1, i2c_addr);

    /* 方式2: 使用SH1106 128x64 I2C（根据实际芯片选择）*/
    // U8G2_OLED_Init_SH1106_128x64_I2C(&g_u8g2_oled, &hi2c1, i2c_addr);

    /* 再次延时，确保OLED初始化完成 */
    HAL_Delay(100);

    /* 获取u8g2对象 */
    u8g2_t *u8g2 = U8G2_OLED_GetU8g2(&g_u8g2_oled);

    if(u8g2 == NULL)
    {
        /* 初始化失败，可以在这里添加错误处理 */
        return;
    }

    /* 初始化页面管理器 */
    PageManager_Init(&g_page_manager, u8g2);

    /* 注册所有页面 */
    Pages_RegisterAll(&g_page_manager);

    /* 切换到主页面并绘制 */
    PageManager_SwitchTo(&g_page_manager, PAGE_MAIN);
    PageManager_Draw(&g_page_manager);
}

/**
  * @brief  按键中断回调处理
  * @param  GPIO_Pin: 引脚编号
  * @note   在HAL_GPIO_EXTI_Callback中调用此函数
  */
void OLED_App_KeyCallback(uint16_t GPIO_Pin)
{
    KeyID_t key = KEY_NONE;

    /* Check if this is a long press (MSB set) */
    uint8_t is_long_press = (GPIO_Pin & 0x8000) ? 1 : 0;
    GPIO_Pin &= 0x7FFF;  /* Clear MSB to get actual pin */

    /* 识别按键 */
    if(GPIO_Pin == GPIO_PIN_10)  /* PB10 - 左键 */
    {
        key = is_long_press ? KEY_LEFT_LONG : KEY_LEFT;
    }
    else if(GPIO_Pin == GPIO_PIN_11)  /* PB11 - 右键 */
    {
        key = is_long_press ? KEY_RIGHT_LONG : KEY_RIGHT;
    }

    /* 处理按键事件 */
    if(key != KEY_NONE)
    {
        PageManager_HandleKey(&g_page_manager, key);
    }
}

/**
  * @brief  主循环更新
  * @note   可选，用于动画等需要定时刷新的场景
  */
void OLED_App_Update(void)
{
    /* 检查长按事件 */
    extern void BSP_KEY_CheckLongPress(void);
    BSP_KEY_CheckLongPress();

    /* 如果需要定时刷新，可以在这里添加逻辑 */
    /* 例如：实时显示时间、传感器数据等 */
}
