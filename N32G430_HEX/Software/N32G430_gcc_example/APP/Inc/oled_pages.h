/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_pages.h
  * @brief   OLED所有页面定义头文件
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __OLED_PAGES_H__
#define __OLED_PAGES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "oled_page_manager.h"
#include "oled_u8g2_ui.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  主页面数据结构
  */
typedef struct {
    char gas_text[16];      /* 气体类型文本 */
    char gas_number[8];     /* 气体编号 */
    char info[32];          /* 信息文本 */
} MainPageData_t;

/**
  * @brief  菜单页面数据结构
  */
typedef struct {
    uint8_t selected_item;  /* 当前选中项 */
    uint8_t total_items;    /* 总项数 */
    const char **items;     /* 菜单项数组 */
} MenuPageData_t;

/* Exported constants --------------------------------------------------------*/

/* 主页面元素位置 */
#define MAIN_PAGE_MENU_Y        9
#define MAIN_PAGE_ICON_LEFT_X   6
#define MAIN_PAGE_ICON_LEFT_Y   22
#define MAIN_PAGE_ICON_RIGHT_X  104
#define MAIN_PAGE_ICON_RIGHT_Y  25
#define MAIN_PAGE_MAIN_TEXT_X   44
#define MAIN_PAGE_MAIN_TEXT_Y   40
#define MAIN_PAGE_NUMBER_X      77
#define MAIN_PAGE_NUMBER_Y      40
#define MAIN_PAGE_INFO_X        1
#define MAIN_PAGE_INFO_Y        59

/* Exported variables --------------------------------------------------------*/

/* 页面定义（外部引用） */
extern const PageDef_t page_main_def;
extern const PageDef_t page_menu_def;
extern const PageDef_t page_gas_type_def;
extern const PageDef_t page_edit_def;
extern const PageDef_t page_settings_def;

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  注册所有页面到管理器
  * @param  manager: 页面管理器指针
  */
void Pages_RegisterAll(PageManager_t *manager);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_PAGES_H__ */
