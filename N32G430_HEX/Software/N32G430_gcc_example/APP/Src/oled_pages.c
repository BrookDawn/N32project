/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_pages.c
  * @brief   OLED所有页面实现文件
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "oled_pages.h"
#include <string.h>
#include <stdio.h>

/* Private defines -----------------------------------------------------------*/

/* 图标位图数据 */
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

/* 主页面数据 */
static MainPageData_t main_page_data = {
    .gas_text = "CH",
    .gas_number = "4",
    .info = "save success"
};

/* 菜单页面数据 */
static const char *menu_items[] = {"GasType", "Settings", "About"};
static MenuPageData_t menu_page_data = {
    .selected_item = 0,
    .total_items = 3,
    .items = menu_items
};

/* Settings页面数据 */
static const char *settings_items[] = {"Brightness", "Contrast", "Backlight", "Reset"};
static MenuPageData_t settings_page_data = {
    .selected_item = 0,
    .total_items = 4,
    .items = settings_items
};

/* Settings值 */
static uint8_t brightness_value = 128;  /* 0-255 */
static uint8_t contrast_value = 128;    /* 0-255 */
static uint8_t backlight_on = 1;        /* 0=off, 1=on */

/* Private function prototypes -----------------------------------------------*/

/* ============================================================================
   主页面 (PAGE_MAIN)
   显示：Menu > CH_4 > save success
   左键：进入菜单
   右键：进入GasType页面
   ============================================================================ */

static void Page_Main_Init(PageContext_t *ctx)
{
    /* 主页面初始化（可选） */
}

static void Page_Main_Enter(PageContext_t *ctx)
{
    /* 进入主页面时的处理 */
    ctx->page_data = &main_page_data;
}

static void Page_Main_Draw(u8g2_t *u8g2, PageContext_t *ctx)
{
    MainPageData_t *data = (MainPageData_t *)ctx->page_data;
    if(data == NULL) return;

    /* 清空缓冲区 */
    u8g2_ClearBuffer(u8g2);
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);

    /* 绘制菜单文本 */
    u8g2_SetFont(u8g2, u8g2_font_t0_12_tr);
    u8g2_DrawStr(u8g2, 0, MAIN_PAGE_MENU_Y, "Menu");

    /* 绘制左图标（向下箭头） */
    u8g2_DrawXBM(u8g2, MAIN_PAGE_ICON_LEFT_X, MAIN_PAGE_ICON_LEFT_Y,
                 14, 15, image_ArrowDownFilled_bits);

    /* 绘制右图标（确认） */
    u8g2_DrawXBM(u8g2, MAIN_PAGE_ICON_RIGHT_X, MAIN_PAGE_ICON_RIGHT_Y,
                 16, 9, image_KeyBackspace_bits);

    /* 绘制主显示区 */
    u8g2_SetFont(u8g2, u8g2_font_profont29_tr);
    u8g2_DrawStr(u8g2, MAIN_PAGE_MAIN_TEXT_X, MAIN_PAGE_MAIN_TEXT_Y, data->gas_text);

    u8g2_SetFont(u8g2, u8g2_font_profont12_tr);
    u8g2_DrawStr(u8g2, MAIN_PAGE_NUMBER_X, MAIN_PAGE_NUMBER_Y, data->gas_number);

    /* 绘制信息区 */
    u8g2_SetFont(u8g2, u8g2_font_timR10_tr);
    u8g2_DrawStr(u8g2, MAIN_PAGE_INFO_X, MAIN_PAGE_INFO_Y, data->info);

    /* 发送缓冲区 */
    u8g2_SendBuffer(u8g2);
}

static void Page_Main_HandleKey(KeyID_t key, PageContext_t *ctx)
{
    extern PageManager_t g_page_manager;  /* 需要在应用层定义 */

    if(key == KEY_LEFT)
    {
        /* 左键：进入菜单页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_MENU);
    }
    else if(key == KEY_RIGHT)
    {
        /* 右键：进入气体类型页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_GAS_TYPE);
    }
    else if(key == KEY_LEFT_LONG)
    {
        /* 左键长按：直接进入设置页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_SETTINGS);
    }
    else if(key == KEY_RIGHT_LONG)
    {
        /* 右键长按：保留用于其他功能 */
        /* 例如：快速保存当前设置 */
    }
}

/* 主页面定义 */
const PageDef_t page_main_def = {
    .page_id = PAGE_MAIN,
    .page_name = "Main",
    .init = Page_Main_Init,
    .enter = Page_Main_Enter,
    .exit = NULL,
    .draw = Page_Main_Draw,
    .handle_key = Page_Main_HandleKey
};

/* ============================================================================
   菜单页面 (PAGE_MENU)
   显示：Menu > GasType > Edit
   左键：切换菜单项
   右键：确认并进入子页面
   ============================================================================ */

static void Page_Menu_Enter(PageContext_t *ctx)
{
    ctx->page_data = &menu_page_data;
}

static void Page_Menu_Draw(u8g2_t *u8g2, PageContext_t *ctx)
{
    MenuPageData_t *data = (MenuPageData_t *)ctx->page_data;
    if(data == NULL) return;

    u8g2_ClearBuffer(u8g2);
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);

    /* 绘制菜单路径 */
    u8g2_SetFont(u8g2, u8g2_font_t0_12_tr);
    u8g2_DrawStr(u8g2, 0, 9, "Menu");
    u8g2_DrawXBM(u8g2, 28, 2, 4, 7, image_download_bits);
    u8g2_DrawStr(u8g2, 35, 9, data->items[data->selected_item]);

    /* 绘制左图标 */
    u8g2_DrawXBM(u8g2, 6, 22, 14, 15, image_ArrowDownFilled_bits);

    /* 绘制右图标 */
    u8g2_DrawXBM(u8g2, 104, 25, 16, 9, image_KeyBackspace_bits);

    /* 绘制高亮框 */
    u8g2_DrawRFrame(u8g2, 38, 20, 60, 25, 3);

    /* 绘制当前选项 */
    u8g2_SetFont(u8g2, u8g2_font_profont29_tr);
    char str[32];
    sprintf(str, "%d/%d", data->selected_item + 1, data->total_items);
    u8g2_DrawStr(u8g2, 44, 40, str);

    /* 绘制信息 */
    u8g2_SetFont(u8g2, u8g2_font_timR10_tr);
    u8g2_DrawStr(u8g2, 1, 59, "< Next   Enter >");

    u8g2_SendBuffer(u8g2);
}

static void Page_Menu_HandleKey(KeyID_t key, PageContext_t *ctx)
{
    extern PageManager_t g_page_manager;
    MenuPageData_t *data = (MenuPageData_t *)ctx->page_data;
    if(data == NULL) return;

    if(key == KEY_LEFT)
    {
        /* 左键：切换到下一个菜单项 */
        data->selected_item++;
        if(data->selected_item >= data->total_items)
        {
            data->selected_item = 0;
        }
        PageManager_RequestRedraw(&g_page_manager);
    }
    else if(key == KEY_RIGHT)
    {
        /* 右键：根据选中项进入不同页面 */
        switch(data->selected_item)
        {
            case 0:  /* GasType */
                PageManager_SwitchTo(&g_page_manager, PAGE_GAS_TYPE);
                break;
            case 1:  /* Settings */
                PageManager_SwitchTo(&g_page_manager, PAGE_SETTINGS);
                break;
            case 2:  /* About */
                /* 返回主页面 */
                PageManager_SwitchTo(&g_page_manager, PAGE_MAIN);
                break;
        }
    }
    else if(key == KEY_LEFT_LONG || key == KEY_RIGHT_LONG)
    {
        /* 长按任意键：返回主页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_MAIN);
    }
}

/* 菜单页面定义 */
const PageDef_t page_menu_def = {
    .page_id = PAGE_MENU,
    .page_name = "Menu",
    .init = NULL,
    .enter = Page_Menu_Enter,
    .exit = NULL,
    .draw = Page_Menu_Draw,
    .handle_key = Page_Menu_HandleKey
};

/* ============================================================================
   气体类型页面 (PAGE_GAS_TYPE)
   显示：Menu > GasType > CH4
   左键：切换气体类型
   右键：进入编辑页面
   ============================================================================ */

static const char *gas_types[] = {"CH4", "O2", "CO", "CO2", "H2S"};
static uint8_t current_gas_index = 0;

static void Page_GasType_Enter(PageContext_t *ctx)
{
    /* 进入气体类型页面 */
}

static void Page_GasType_Draw(u8g2_t *u8g2, PageContext_t *ctx)
{
    u8g2_ClearBuffer(u8g2);
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);

    /* 绘制菜单路径 */
    u8g2_SetFont(u8g2, u8g2_font_t0_12_tr);
    u8g2_DrawStr(u8g2, 0, 9, "Menu");
    u8g2_DrawXBM(u8g2, 28, 2, 4, 7, image_download_bits);
    u8g2_DrawStr(u8g2, 35, 9, "GasType");

    /* 绘制图标 */
    u8g2_DrawXBM(u8g2, 6, 22, 14, 15, image_ArrowDownFilled_bits);
    u8g2_DrawXBM(u8g2, 104, 25, 16, 9, image_KeyBackspace_bits);

    /* 绘制高亮框 */
    u8g2_DrawRFrame(u8g2, 38, 20, 60, 25, 3);

    /* 绘制当前气体类型 */
    u8g2_SetFont(u8g2, u8g2_font_profont29_tr);
    u8g2_DrawStr(u8g2, 44, 40, gas_types[current_gas_index]);

    /* 绘制索引 */
    u8g2_SetFont(u8g2, u8g2_font_profont12_tr);
    char idx_str[8];
    sprintf(idx_str, "%d", current_gas_index + 1);
    u8g2_DrawStr(u8g2, 77, 40, idx_str);

    /* 绘制信息 */
    u8g2_SetFont(u8g2, u8g2_font_timR10_tr);
    u8g2_DrawStr(u8g2, 1, 59, "< Next   Edit >");

    u8g2_SendBuffer(u8g2);
}

static void Page_GasType_HandleKey(KeyID_t key, PageContext_t *ctx)
{
    extern PageManager_t g_page_manager;

    if(key == KEY_LEFT)
    {
        /* 左键：切换气体类型 */
        current_gas_index++;
        if(current_gas_index >= 5)
        {
            current_gas_index = 0;
        }
        PageManager_RequestRedraw(&g_page_manager);
    }
    else if(key == KEY_RIGHT)
    {
        /* 右键：进入编辑页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_EDIT);
    }
    else if(key == KEY_LEFT_LONG || key == KEY_RIGHT_LONG)
    {
        /* 长按任意键：返回主页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_MAIN);
    }
}

/* 气体类型页面定义 */
const PageDef_t page_gas_type_def = {
    .page_id = PAGE_GAS_TYPE,
    .page_name = "GasType",
    .init = NULL,
    .enter = Page_GasType_Enter,
    .exit = NULL,
    .draw = Page_GasType_Draw,
    .handle_key = Page_GasType_HandleKey
};

/* ============================================================================
   编辑页面 (PAGE_EDIT)
   显示：Menu > GasType > Edit
   左键：返回上一页
   右键：保存并返回
   ============================================================================ */

static void Page_Edit_Enter(PageContext_t *ctx)
{
    /* 进入编辑页面 */
}

static void Page_Edit_Draw(u8g2_t *u8g2, PageContext_t *ctx)
{
    u8g2_ClearBuffer(u8g2);
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);

    /* 绘制完整菜单路径 */
    u8g2_SetFont(u8g2, u8g2_font_t0_12_tr);
    u8g2_DrawStr(u8g2, 0, 9, "Menu");
    u8g2_DrawXBM(u8g2, 28, 2, 4, 7, image_download_bits);
    u8g2_DrawStr(u8g2, 35, 9, "GasType");
    u8g2_DrawXBM(u8g2, 81, 2, 4, 7, image_download_bits);
    u8g2_DrawStr(u8g2, 88, 9, "Edit");

    /* 绘制图标 */
    u8g2_DrawXBM(u8g2, 6, 22, 14, 15, image_ArrowDownFilled_bits);
    u8g2_DrawXBM(u8g2, 104, 25, 16, 9, image_KeyBackspace_bits);

    /* 绘制编辑内容 */
    u8g2_SetFont(u8g2, u8g2_font_profont29_tr);
    u8g2_DrawStr(u8g2, 44, 40, gas_types[current_gas_index]);

    u8g2_SetFont(u8g2, u8g2_font_profont12_tr);
    u8g2_DrawStr(u8g2, 77, 40, "E");

    /* 绘制信息 */
    u8g2_SetFont(u8g2, u8g2_font_timR10_tr);
    u8g2_DrawStr(u8g2, 1, 59, "Editing...");

    u8g2_SendBuffer(u8g2);
}

static void Page_Edit_HandleKey(KeyID_t key, PageContext_t *ctx)
{
    extern PageManager_t g_page_manager;

    if(key == KEY_LEFT)
    {
        /* 左键：返回上一页（不保存） */
        PageManager_GoBack(&g_page_manager);
    }
    else if(key == KEY_RIGHT)
    {
        /* 右键：保存并返回主页面 */
        /* 这里可以添加保存逻辑 */

        /* 更新主页面数据 */
        strcpy(main_page_data.gas_text, gas_types[current_gas_index]);
        strcpy(main_page_data.info, "save success");

        /* 返回主页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_MAIN);
    }
    else if(key == KEY_LEFT_LONG)
    {
        /* 左键长按：取消并返回主页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_MAIN);
    }
}

/* 编辑页面定义 */
const PageDef_t page_edit_def = {
    .page_id = PAGE_EDIT,
    .page_name = "Edit",
    .init = NULL,
    .enter = Page_Edit_Enter,
    .exit = NULL,
    .draw = Page_Edit_Draw,
    .handle_key = Page_Edit_HandleKey
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  注册所有页面到管理器
  * @param  manager: 页面管理器指针
  */
void Pages_RegisterAll(PageManager_t *manager)
{
    if(manager == NULL) return;

    /* 注册所有页面 */
    PageManager_RegisterPage(manager, &page_main_def);
    PageManager_RegisterPage(manager, &page_menu_def);
    PageManager_RegisterPage(manager, &page_gas_type_def);
    PageManager_RegisterPage(manager, &page_edit_def);
    PageManager_RegisterPage(manager, &page_settings_def);
}

/* ============================================================================
   Settings页面 (PAGE_SETTINGS)
   显示：Menu > Settings > [选项]
   左键：切换设置项
   右键：根据项目执行操作或返回
   ============================================================================ */

static void Page_Settings_Enter(PageContext_t *ctx)
{
    ctx->page_data = &settings_page_data;
}

static void Page_Settings_Draw(u8g2_t *u8g2, PageContext_t *ctx)
{
    MenuPageData_t *data = (MenuPageData_t *)ctx->page_data;
    if(data == NULL) return;

    u8g2_ClearBuffer(u8g2);
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);

    /* 绘制菜单路径 */
    u8g2_SetFont(u8g2, u8g2_font_t0_12_tr);
    u8g2_DrawStr(u8g2, 0, 9, "Menu");
    u8g2_DrawXBM(u8g2, 28, 2, 4, 7, image_download_bits);
    u8g2_DrawStr(u8g2, 35, 9, "Settings");

    /* 绘制图标 */
    u8g2_DrawXBM(u8g2, 6, 22, 14, 15, image_ArrowDownFilled_bits);
    u8g2_DrawXBM(u8g2, 104, 25, 16, 9, image_KeyBackspace_bits);

    /* 绘制高亮框 */
    u8g2_DrawRFrame(u8g2, 12, 25, 100, 20, 2);

    /* 绘制当前设置项 */
    u8g2_SetFont(u8g2, u8g2_font_profont15_tr);
    u8g2_DrawStr(u8g2, 18, 35, data->items[data->selected_item]);

    /* 绘制当前设置值 */
    u8g2_SetFont(u8g2, u8g2_font_profont12_tr);
    char value_str[16];
    switch(data->selected_item)
    {
        case 0:  /* Brightness */
            sprintf(value_str, "%d", brightness_value);
            break;
        case 1:  /* Contrast */
            sprintf(value_str, "%d", contrast_value);
            break;
        case 2:  /* Backlight */
            sprintf(value_str, "%s", backlight_on ? "ON" : "OFF");
            break;
        case 3:  /* Reset */
            sprintf(value_str, "Press OK");
            break;
        default:
            value_str[0] = '\0';
            break;
    }
    u8g2_DrawStr(u8g2, 22, 41, value_str);

    /* 绘制索引和提示 */
    char idx_str[20];
    sprintf(idx_str, "%d/%d < Next  OK >", data->selected_item + 1, data->total_items);
    u8g2_DrawStr(u8g2, 1, 59, idx_str);

    u8g2_SendBuffer(u8g2);
}

static void Page_Settings_HandleKey(KeyID_t key, PageContext_t *ctx)
{
    extern PageManager_t g_page_manager;
    MenuPageData_t *data = (MenuPageData_t *)ctx->page_data;
    if(data == NULL) return;

    if(key == KEY_LEFT)
    {
        /* 左键：切换设置项 */
        data->selected_item++;
        if(data->selected_item >= data->total_items)
        {
            data->selected_item = 0;
        }
        PageManager_RequestRedraw(&g_page_manager);
    }
    else if(key == KEY_RIGHT)
    {
        /* 右键：执行操作 */
        switch(data->selected_item)
        {
            case 0:  /* Brightness - 循环增加 */
                brightness_value += 32;
                if(brightness_value > 255) brightness_value = 0;
                PageManager_RequestRedraw(&g_page_manager);
                break;
            case 1:  /* Contrast - 循环增加 */
                contrast_value += 32;
                if(contrast_value > 255) contrast_value = 0;
                PageManager_RequestRedraw(&g_page_manager);
                break;
            case 2:  /* Backlight - 切换开关 */
                backlight_on = !backlight_on;
                PageManager_RequestRedraw(&g_page_manager);
                break;
            case 3:  /* Reset - 返回主菜单 */
                /* 恢复默认值 */
                brightness_value = 128;
                contrast_value = 128;
                backlight_on = 1;
                PageManager_SwitchTo(&g_page_manager, PAGE_MAIN);
                break;
        }
    }
    else if(key == KEY_LEFT_LONG || key == KEY_RIGHT_LONG)
    {
        /* 长按任意键：返回主页面 */
        PageManager_SwitchTo(&g_page_manager, PAGE_MAIN);
    }
}

/* Settings页面定义 */
const PageDef_t page_settings_def = {
    .page_id = PAGE_SETTINGS,
    .page_name = "Settings",
    .init = NULL,
    .enter = Page_Settings_Enter,
    .exit = NULL,
    .draw = Page_Settings_Draw,
    .handle_key = Page_Settings_HandleKey
};
