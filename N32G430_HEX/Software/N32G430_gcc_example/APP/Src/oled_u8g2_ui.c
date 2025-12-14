/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_u8g2_ui.c
  * @brief   U8g2 UI系统实现文件 - 包含菜单系统和区域管理
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "oled_u8g2_ui.h"
#include <string.h>
#include <stdio.h>

/* Private defines -----------------------------------------------------------*/

/* 图像位图数据 */
static const uint8_t image_ArrowDownFilled_bits[] = {
    0xf8,0x07,0x08,0x04,0xe8,0x05,0x68,0x05,
    0xa8,0x05,0x68,0x05,0xa8,0x05,0x6f,0x3d,
    0xa1,0x21,0xfa,0x17,0xf4,0x0b,0xe8,0x05,
    0xd0,0x02,0x20,0x01,0xc0,0x00
};

static const uint8_t image_download_bits[] = {
    0x01,0x03,0x07,0x0f,0x07,0x03,0x01
};

static const uint8_t image_KeyBackspace_bits[] = {
    0xfe,0x7f,0x01,0x80,0x11,0x80,0x19,0x80,
    0xfd,0xbf,0x19,0x80,0x11,0x80,0x01,0x80,
    0xfe,0x7f
};

/* Private variables ---------------------------------------------------------*/

/* 菜单文本定义 */
static const char* menu_level_text[] = {
    "Menu",      /* MENU_LEVEL_1 */
    "GasType",   /* MENU_LEVEL_2 */
    "Edit"       /* MENU_LEVEL_3 */
};

/* Private function prototypes -----------------------------------------------*/
static void UI_DrawMenuBar(u8g2_t *u8g2, MenuLevel_t level);
static void UI_DrawLeftIcon(u8g2_t *u8g2, UI_IconType_t icon_type);
static void UI_DrawRightIcon(u8g2_t *u8g2, UI_IconType_t icon_type);
static void UI_DrawMainDisplay(u8g2_t *u8g2, const char *text, const char *number);
static void UI_DrawInfo(u8g2_t *u8g2, const char *info);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  初始化UI系统
  * @param  ui_state: UI状态结构体指针
  */
void UI_Init(UI_State_t *ui_state)
{
    if(ui_state == NULL) return;

    /* 初始化菜单状态 */
    ui_state->current_level = MENU_LEVEL_1;
    ui_state->current_option = 0;

    /* 初始化主显示区域 */
    strcpy(ui_state->main_text, "CH");
    strcpy(ui_state->main_number, "4");

    /* 初始化信息区域 */
    strcpy(ui_state->info_text, "save success");

    /* 初始化图标状态 */
    ui_state->left_icon = UI_ICON_ARROW_DOWN;   /* 左键：向下遍历 */
    ui_state->right_icon = UI_ICON_CONFIRM;     /* 右键：确认 */
}

/**
  * @brief  绘制完整UI界面
  * @param  u8g2: u8g2结构体指针
  * @param  ui_state: UI状态结构体指针
  */
void UI_Draw(u8g2_t *u8g2, const UI_State_t *ui_state)
{
    if(u8g2 == NULL || ui_state == NULL) return;

    /* 清空缓冲区 */
    u8g2_ClearBuffer(u8g2);

    /* 设置位图和字体模式 */
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);

    /* 绘制各个区域 */
    UI_DrawMenuBar(u8g2, ui_state->current_level);
    UI_DrawLeftIcon(u8g2, ui_state->left_icon);
    UI_DrawRightIcon(u8g2, ui_state->right_icon);
    UI_DrawMainDisplay(u8g2, ui_state->main_text, ui_state->main_number);
    UI_DrawInfo(u8g2, ui_state->info_text);

    /* 发送缓冲区到显示屏 */
    u8g2_SendBuffer(u8g2);
}

/**
  * @brief  处理左键按下（向下遍历）
  * @param  ui_state: UI状态结构体指针
  */
void UI_HandleLeftKey(UI_State_t *ui_state)
{
    if(ui_state == NULL) return;

    /* 在当前菜单级别中向下遍历选项 */
    ui_state->current_option++;

    /* 这里可以根据实际菜单项数量进行循环 */
    /* 示例：假设每个菜单级别有3个选项 */
    if(ui_state->current_option >= 3)
    {
        ui_state->current_option = 0;
    }

    /* 更新信息显示 */
    sprintf(ui_state->info_text, "Option %d", ui_state->current_option);
}

/**
  * @brief  处理右键按下（确认/进入下级）
  * @param  ui_state: UI状态结构体指针
  */
void UI_HandleRightKey(UI_State_t *ui_state)
{
    if(ui_state == NULL) return;

    /* 进入下一级菜单 */
    if(ui_state->current_level < MENU_LEVEL_3)
    {
        ui_state->current_level++;
        ui_state->current_option = 0;  /* 重置选项索引 */

        /* 更新信息显示 */
        sprintf(ui_state->info_text, "Enter %s", UI_GetMenuText(ui_state->current_level));
    }
    else
    {
        /* 已经在最后一级菜单，执行确认操作 */
        sprintf(ui_state->info_text, "Confirmed");
    }
}

/**
  * @brief  设置主显示内容
  * @param  ui_state: UI状态结构体指针
  * @param  text: 主文本（如"CH"）
  * @param  number: 数字文本（如"4"）
  */
void UI_SetMainDisplay(UI_State_t *ui_state, const char *text, const char *number)
{
    if(ui_state == NULL) return;

    if(text != NULL)
    {
        strncpy(ui_state->main_text, text, sizeof(ui_state->main_text) - 1);
        ui_state->main_text[sizeof(ui_state->main_text) - 1] = '\0';
    }

    if(number != NULL)
    {
        strncpy(ui_state->main_number, number, sizeof(ui_state->main_number) - 1);
        ui_state->main_number[sizeof(ui_state->main_number) - 1] = '\0';
    }
}

/**
  * @brief  设置信息区内容
  * @param  ui_state: UI状态结构体指针
  * @param  info: 信息文本
  */
void UI_SetInfo(UI_State_t *ui_state, const char *info)
{
    if(ui_state == NULL || info == NULL) return;

    strncpy(ui_state->info_text, info, sizeof(ui_state->info_text) - 1);
    ui_state->info_text[sizeof(ui_state->info_text) - 1] = '\0';
}

/**
  * @brief  获取当前菜单级别文本
  * @param  level: 菜单级别
  * @retval 菜单文本字符串
  */
const char* UI_GetMenuText(MenuLevel_t level)
{
    if(level >= MENU_LEVEL_1 && level <= MENU_LEVEL_3)
    {
        return menu_level_text[level];
    }
    return "";
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  绘制菜单栏
  * @param  u8g2: u8g2结构体指针
  * @param  level: 当前菜单级别
  */
static void UI_DrawMenuBar(u8g2_t *u8g2, MenuLevel_t level)
{
    uint8_t x_pos = UI_REGION_MENU_BAR_X;

    /* 设置菜单栏字体 */
    u8g2_SetFont(u8g2, u8g2_font_t0_12_tr);

    /* 绘制第一级菜单 "Menu" */
    u8g2_DrawStr(u8g2, x_pos, 9, "Menu");
    x_pos += u8g2_GetStrWidth(u8g2, "Menu");

    if(level >= MENU_LEVEL_2)
    {
        /* 绘制分隔图标 (download icon) */
        u8g2_DrawXBM(u8g2, x_pos + 3, 2, 4, 7, image_download_bits);
        x_pos += 12;

        /* 绘制第二级菜单 "GasType" */
        u8g2_DrawStr(u8g2, x_pos, 9, "GasType");
        x_pos += u8g2_GetStrWidth(u8g2, "GasType");
    }

    if(level >= MENU_LEVEL_3)
    {
        /* 绘制分隔图标 (download icon) */
        u8g2_DrawXBM(u8g2, x_pos + 3, 2, 4, 7, image_download_bits);
        x_pos += 12;

        /* 绘制第三级菜单 "Edit" */
        u8g2_DrawStr(u8g2, x_pos, 9, "Edit");
    }
}

/**
  * @brief  绘制左侧图标
  * @param  u8g2: u8g2结构体指针
  * @param  icon_type: 图标类型
  */
static void UI_DrawLeftIcon(u8g2_t *u8g2, UI_IconType_t icon_type)
{
    switch(icon_type)
    {
        case UI_ICON_ARROW_DOWN:
            /* 绘制向下箭头（遍历图标） */
            u8g2_DrawXBM(u8g2,
                        UI_REGION_LEFT_ICON_X,
                        UI_REGION_LEFT_ICON_Y,
                        UI_REGION_LEFT_ICON_W,
                        UI_REGION_LEFT_ICON_H,
                        image_ArrowDownFilled_bits);
            break;

        case UI_ICON_NONE:
        default:
            /* 不绘制图标 */
            break;
    }
}

/**
  * @brief  绘制右侧图标
  * @param  u8g2: u8g2结构体指针
  * @param  icon_type: 图标类型
  */
static void UI_DrawRightIcon(u8g2_t *u8g2, UI_IconType_t icon_type)
{
    switch(icon_type)
    {
        case UI_ICON_CONFIRM:
            /* 绘制确认图标（KeyBackspace） */
            u8g2_DrawXBM(u8g2,
                        UI_REGION_RIGHT_ICON_X,
                        UI_REGION_RIGHT_ICON_Y,
                        UI_REGION_RIGHT_ICON_W,
                        UI_REGION_RIGHT_ICON_H,
                        image_KeyBackspace_bits);
            break;

        case UI_ICON_NONE:
        default:
            /* 不绘制图标 */
            break;
    }
}

/**
  * @brief  绘制主显示区域
  * @param  u8g2: u8g2结构体指针
  * @param  text: 主文本（如"CH"）
  * @param  number: 数字（如"4"）
  */
static void UI_DrawMainDisplay(u8g2_t *u8g2, const char *text, const char *number)
{
    if(text == NULL || number == NULL) return;

    /* 绘制大号主文本 "CH" */
    u8g2_SetFont(u8g2, u8g2_font_profont29_tr);
    u8g2_DrawStr(u8g2, UI_REGION_MAIN_DISPLAY_X, 40, text);

    /* 绘制数字 "4" */
    u8g2_SetFont(u8g2, u8g2_font_profont12_tr);
    u8g2_DrawStr(u8g2, 77, 40, number);
}

/**
  * @brief  绘制信息区域
  * @param  u8g2: u8g2结构体指针
  * @param  info: 信息文本
  */
static void UI_DrawInfo(u8g2_t *u8g2, const char *info)
{
    if(info == NULL) return;

    /* 设置信息区字体 */
    u8g2_SetFont(u8g2, u8g2_font_timR10_tr);

    /* 绘制信息文本 */
    u8g2_DrawStr(u8g2, UI_REGION_INFO_X, UI_REGION_INFO_Y, info);
}
