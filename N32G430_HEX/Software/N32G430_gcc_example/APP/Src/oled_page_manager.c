/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_page_manager.c
  * @brief   OLED页面管理系统实现文件
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "oled_page_manager.h"
#include <string.h>

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  初始化页面管理器
  * @param  manager: 页面管理器指针
  * @param  u8g2: u8g2对象指针
  */
void PageManager_Init(PageManager_t *manager, u8g2_t *u8g2)
{
    if(manager == NULL || u8g2 == NULL) return;

    /* 清零管理器 */
    memset(manager, 0, sizeof(PageManager_t));

    /* 保存u8g2指针 */
    manager->u8g2 = u8g2;

    /* 初始化上下文 */
    manager->context.current_page = PAGE_MAIN;
    manager->context.prev_page = PAGE_MAIN;
    manager->context.page_data = NULL;
    manager->context.need_redraw = true;
    manager->context.timestamp = 0;

    /* 页面计数初始化 */
    manager->page_count = 0;
}

/**
  * @brief  注册页面
  * @param  manager: 页面管理器指针
  * @param  page_def: 页面定义指针
  * @retval 0=成功，1=失败
  */
uint8_t PageManager_RegisterPage(PageManager_t *manager, const PageDef_t *page_def)
{
    if(manager == NULL || page_def == NULL) return 1;

    /* 检查页面ID是否有效 */
    if(page_def->page_id >= PAGE_MAX) return 1;

    /* 检查绘制函数和按键处理函数是否存在 */
    if(page_def->draw == NULL || page_def->handle_key == NULL) return 1;

    /* 注册页面 */
    manager->pages[page_def->page_id] = page_def;
    manager->page_count++;

    /* 如果有初始化函数，执行初始化 */
    if(page_def->init != NULL)
    {
        page_def->init(&manager->context);
    }

    return 0;
}

/**
  * @brief  切换到指定页面
  * @param  manager: 页面管理器指针
  * @param  page_id: 目标页面ID
  */
void PageManager_SwitchTo(PageManager_t *manager, PageID_t page_id)
{
    if(manager == NULL) return;

    /* 检查页面ID是否有效 */
    if(page_id >= PAGE_MAX) return;

    /* 检查页面是否已注册 */
    if(manager->pages[page_id] == NULL) return;

    /* 如果是同一个页面，不需要切换 */
    if(manager->context.current_page == page_id) return;

    /* 调用当前页面的退出回调 */
    const PageDef_t *current_page = manager->pages[manager->context.current_page];
    if(current_page != NULL && current_page->exit != NULL)
    {
        current_page->exit(&manager->context);
    }

    /* 保存前一个页面 */
    manager->context.prev_page = manager->context.current_page;

    /* 切换到新页面 */
    manager->context.current_page = page_id;
    manager->context.need_redraw = true;

    /* 调用新页面的进入回调 */
    const PageDef_t *new_page = manager->pages[page_id];
    if(new_page != NULL && new_page->enter != NULL)
    {
        new_page->enter(&manager->context);
    }
}

/**
  * @brief  返回上一个页面
  * @param  manager: 页面管理器指针
  */
void PageManager_GoBack(PageManager_t *manager)
{
    if(manager == NULL) return;

    /* 切换到前一个页面 */
    PageManager_SwitchTo(manager, manager->context.prev_page);
}

/**
  * @brief  绘制当前页面
  * @param  manager: 页面管理器指针
  */
void PageManager_Draw(PageManager_t *manager)
{
    if(manager == NULL || manager->u8g2 == NULL) return;

    /* 获取当前页面定义 */
    const PageDef_t *current_page = manager->pages[manager->context.current_page];
    if(current_page == NULL) return;

    /* 调用页面绘制函数 */
    if(current_page->draw != NULL)
    {
        current_page->draw(manager->u8g2, &manager->context);
    }

    /* 清除重绘标志 */
    manager->context.need_redraw = false;
}

/**
  * @brief  处理按键事件
  * @param  manager: 页面管理器指针
  * @param  key: 按键ID
  */
void PageManager_HandleKey(PageManager_t *manager, KeyID_t key)
{
    if(manager == NULL || key == KEY_NONE) return;

    /* 获取当前页面定义 */
    const PageDef_t *current_page = manager->pages[manager->context.current_page];
    if(current_page == NULL) return;

    /* 调用页面按键处理函数 */
    if(current_page->handle_key != NULL)
    {
        current_page->handle_key(key, &manager->context);
    }

    /* 如果需要重绘，立即绘制 */
    if(manager->context.need_redraw)
    {
        PageManager_Draw(manager);
    }
}

/**
  * @brief  获取当前页面ID
  * @param  manager: 页面管理器指针
  * @retval 当前页面ID
  */
PageID_t PageManager_GetCurrentPage(PageManager_t *manager)
{
    if(manager == NULL) return PAGE_MAIN;
    return manager->context.current_page;
}

/**
  * @brief  设置页面私有数据
  * @param  manager: 页面管理器指针
  * @param  data: 数据指针
  */
void PageManager_SetPageData(PageManager_t *manager, void *data)
{
    if(manager == NULL) return;
    manager->context.page_data = data;
}

/**
  * @brief  获取页面私有数据
  * @param  manager: 页面管理器指针
  * @retval 数据指针
  */
void* PageManager_GetPageData(PageManager_t *manager)
{
    if(manager == NULL) return NULL;
    return manager->context.page_data;
}

/**
  * @brief  标记页面需要重绘
  * @param  manager: 页面管理器指针
  */
void PageManager_RequestRedraw(PageManager_t *manager)
{
    if(manager == NULL) return;
    manager->context.need_redraw = true;
}
