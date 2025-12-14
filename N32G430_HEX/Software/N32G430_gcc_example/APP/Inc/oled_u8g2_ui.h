/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_u8g2_ui.h
  * @brief   U8g2 UI系统头文件 - 包含菜单系统和区域管理
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __OLED_U8G2_UI_H__
#define __OLED_U8G2_UI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "u8g2.h"
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  UI区域定义
  */
typedef struct {
    uint8_t x;      /* X坐标 */
    uint8_t y;      /* Y坐标 */
    uint8_t width;  /* 宽度 */
    uint8_t height; /* 高度 */
} UI_Region_t;

/**
  * @brief  按键图标类型
  */
typedef enum {
    UI_ICON_NONE = 0,       /* 无图标 */
    UI_ICON_ARROW_DOWN,     /* 向下箭头（左键-遍历） */
    UI_ICON_CONFIRM         /* 确认图标（右键-确认） */
} UI_IconType_t;

/**
  * @brief  菜单级别
  */
typedef enum {
    MENU_LEVEL_1 = 0,  /* 一级菜单: Menu */
    MENU_LEVEL_2,      /* 二级菜单: GasType */
    MENU_LEVEL_3       /* 三级菜单: Edit */
} MenuLevel_t;

/**
  * @brief  菜单项定义
  */
typedef struct {
    const char *text;       /* 菜单文本 */
    MenuLevel_t level;      /* 菜单级别 */
} MenuItem_t;

/**
  * @brief  UI状态结构体
  */
typedef struct {
    /* 菜单状态 */
    MenuLevel_t current_level;      /* 当前菜单级别 */
    uint8_t current_option;         /* 当前选项索引 */

    /* 主显示区域内容 */
    char main_text[16];             /* 主显示文本（如"CH"） */
    char main_number[8];            /* 主显示数字（如"4"） */

    /* 信息区域内容 */
    char info_text[32];             /* 信息文本（如"save success"） */

    /* 按键图标状态 */
    UI_IconType_t left_icon;        /* 左侧图标类型 */
    UI_IconType_t right_icon;       /* 右侧图标类型 */

} UI_State_t;

/* Exported constants --------------------------------------------------------*/

/* UI区域位置定义 */
#define UI_REGION_MENU_BAR_X        0     /* 菜单栏X坐标 */
#define UI_REGION_MENU_BAR_Y        0     /* 菜单栏Y坐标 */
#define UI_REGION_MENU_BAR_H        12    /* 菜单栏高度 */

#define UI_REGION_LEFT_ICON_X       6     /* 左图标X坐标 */
#define UI_REGION_LEFT_ICON_Y       22    /* 左图标Y坐标 */
#define UI_REGION_LEFT_ICON_W       14    /* 左图标宽度 */
#define UI_REGION_LEFT_ICON_H       15    /* 左图标高度 */

#define UI_REGION_RIGHT_ICON_X      104   /* 右图标X坐标 */
#define UI_REGION_RIGHT_ICON_Y      25    /* 右图标Y坐标 */
#define UI_REGION_RIGHT_ICON_W      16    /* 右图标宽度 */
#define UI_REGION_RIGHT_ICON_H      9     /* 右图标高度 */

#define UI_REGION_MAIN_DISPLAY_X    44    /* 主显示区X坐标 */
#define UI_REGION_MAIN_DISPLAY_Y    20    /* 主显示区Y坐标 */
#define UI_REGION_MAIN_DISPLAY_W    60    /* 主显示区宽度 */
#define UI_REGION_MAIN_DISPLAY_H    30    /* 主显示区高度 */

#define UI_REGION_INFO_X            1     /* 信息区X坐标 */
#define UI_REGION_INFO_Y            59    /* 信息区Y坐标（基线） */
#define UI_REGION_INFO_W            126   /* 信息区宽度 */
#define UI_REGION_INFO_H            5     /* 信息区高度 */

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  初始化UI系统
  * @param  ui_state: UI状态结构体指针
  */
void UI_Init(UI_State_t *ui_state);

/**
  * @brief  绘制完整UI界面
  * @param  u8g2: u8g2结构体指针
  * @param  ui_state: UI状态结构体指针
  */
void UI_Draw(u8g2_t *u8g2, const UI_State_t *ui_state);

/**
  * @brief  处理左键按下（向下遍历）
  * @param  ui_state: UI状态结构体指针
  */
void UI_HandleLeftKey(UI_State_t *ui_state);

/**
  * @brief  处理右键按下（确认/进入下级）
  * @param  ui_state: UI状态结构体指针
  */
void UI_HandleRightKey(UI_State_t *ui_state);

/**
  * @brief  设置主显示内容
  * @param  ui_state: UI状态结构体指针
  * @param  text: 主文本（如"CH"）
  * @param  number: 数字文本（如"4"）
  */
void UI_SetMainDisplay(UI_State_t *ui_state, const char *text, const char *number);

/**
  * @brief  设置信息区内容
  * @param  ui_state: UI状态结构体指针
  * @param  info: 信息文本
  */
void UI_SetInfo(UI_State_t *ui_state, const char *info);

/**
  * @brief  获取当前菜单级别文本
  * @param  level: 菜单级别
  * @retval 菜单文本字符串
  */
const char* UI_GetMenuText(MenuLevel_t level);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_U8G2_UI_H__ */
