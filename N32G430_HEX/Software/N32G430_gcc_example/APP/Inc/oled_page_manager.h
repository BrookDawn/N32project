/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_page_manager.h
  * @brief   OLED页面管理系统头文件
  * @note    采用页面注册机制，每个页面有独立的绘制和按键处理函数
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __OLED_PAGE_MANAGER_H__
#define __OLED_PAGE_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "u8g2.h"
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/* 前向声明 */
typedef struct PageContext PageContext_t;

/**
  * @brief  页面ID枚举（根据实际需要添加）
  */
typedef enum {
    PAGE_MAIN = 0,          /* 主页面 */
    PAGE_MENU,              /* 菜单页面 */
    PAGE_GAS_TYPE,          /* 气体类型选择页面 */
    PAGE_EDIT,              /* 编辑页面 */
    PAGE_SETTINGS,          /* 设置页面 */
    PAGE_MAX                /* 页面总数（必须放最后） */
} PageID_t;

/**
  * @brief  按键ID枚举
  */
typedef enum {
    KEY_NONE = 0,
    KEY_LEFT,               /* 左键(PB10) */
    KEY_RIGHT,              /* 右键(PB11) */
    KEY_LEFT_LONG,          /* 左键长按 */
    KEY_RIGHT_LONG          /* 右键长按 */
} KeyID_t;

/**
  * @brief  页面回调函数类型定义
  */

/* 页面初始化函数 */
typedef void (*PageInitFunc_t)(PageContext_t *ctx);

/* 页面绘制函数 */
typedef void (*PageDrawFunc_t)(u8g2_t *u8g2, PageContext_t *ctx);

/* 页面按键处理函数 */
typedef void (*PageKeyFunc_t)(KeyID_t key, PageContext_t *ctx);

/* 页面进入回调 */
typedef void (*PageEnterFunc_t)(PageContext_t *ctx);

/* 页面退出回调 */
typedef void (*PageExitFunc_t)(PageContext_t *ctx);

/**
  * @brief  页面上下文结构体（用于页面间传递数据）
  */
struct PageContext {
    PageID_t current_page;      /* 当前页面ID */
    PageID_t prev_page;         /* 前一个页面ID */
    void *page_data;            /* 页面私有数据指针 */
    bool need_redraw;           /* 是否需要重绘 */
    uint32_t timestamp;         /* 页面进入时间戳 */
};

/**
  * @brief  页面定义结构体
  */
typedef struct {
    PageID_t page_id;           /* 页面ID */
    const char *page_name;      /* 页面名称（用于调试） */
    PageInitFunc_t init;        /* 初始化函数（可选） */
    PageEnterFunc_t enter;      /* 进入页面回调（可选） */
    PageExitFunc_t exit;        /* 退出页面回调（可选） */
    PageDrawFunc_t draw;        /* 绘制函数（必须） */
    PageKeyFunc_t handle_key;   /* 按键处理函数（必须） */
} PageDef_t;

/**
  * @brief  页面管理器结构体
  */
typedef struct {
    PageContext_t context;      /* 页面上下文 */
    const PageDef_t *pages[PAGE_MAX]; /* 页面定义数组 */
    uint8_t page_count;         /* 已注册页面数量 */
    u8g2_t *u8g2;              /* u8g2对象指针 */
} PageManager_t;

/* Exported constants --------------------------------------------------------*/

/* 按键引脚定义 */
#define KEY_LEFT_PIN        GPIO_PIN_10     /* PB10 */
#define KEY_LEFT_PORT       GPIOB
#define KEY_RIGHT_PIN       GPIO_PIN_11     /* PB11 */
#define KEY_RIGHT_PORT      GPIOB

/* 按键长按时间定义 (毫秒) */
#define KEY_LONG_PRESS_TIME_MS  1000

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  初始化页面管理器
  * @param  manager: 页面管理器指针
  * @param  u8g2: u8g2对象指针
  */
void PageManager_Init(PageManager_t *manager, u8g2_t *u8g2);

/**
  * @brief  注册页面
  * @param  manager: 页面管理器指针
  * @param  page_def: 页面定义指针
  * @retval 0=成功，1=失败
  */
uint8_t PageManager_RegisterPage(PageManager_t *manager, const PageDef_t *page_def);

/**
  * @brief  切换到指定页面
  * @param  manager: 页面管理器指针
  * @param  page_id: 目标页面ID
  */
void PageManager_SwitchTo(PageManager_t *manager, PageID_t page_id);

/**
  * @brief  返回上一个页面
  * @param  manager: 页面管理器指针
  */
void PageManager_GoBack(PageManager_t *manager);

/**
  * @brief  绘制当前页面
  * @param  manager: 页面管理器指针
  */
void PageManager_Draw(PageManager_t *manager);

/**
  * @brief  处理按键事件
  * @param  manager: 页面管理器指针
  * @param  key: 按键ID
  */
void PageManager_HandleKey(PageManager_t *manager, KeyID_t key);

/**
  * @brief  获取当前页面ID
  * @param  manager: 页面管理器指针
  * @retval 当前页面ID
  */
PageID_t PageManager_GetCurrentPage(PageManager_t *manager);

/**
  * @brief  设置页面私有数据
  * @param  manager: 页面管理器指针
  * @param  data: 数据指针
  */
void PageManager_SetPageData(PageManager_t *manager, void *data);

/**
  * @brief  获取页面私有数据
  * @param  manager: 页面管理器指针
  * @retval 数据指针
  */
void* PageManager_GetPageData(PageManager_t *manager);

/**
  * @brief  标记页面需要重绘
  * @param  manager: 页面管理器指针
  */
void PageManager_RequestRedraw(PageManager_t *manager);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_PAGE_MANAGER_H__ */
