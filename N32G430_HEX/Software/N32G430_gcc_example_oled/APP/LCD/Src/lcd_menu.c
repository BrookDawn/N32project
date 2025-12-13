/**
 * @file lcd_menu.c
 * @brief Windows 11 style menu system - Fluent Design with low saturation blue
 */

#include "lcd_menu.h"
#include "lcd_init.h"
#include "hal_compat.h"
#include "bsp_usart.h"
#include "n32g430.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

/* Windows 11 Fluent Design Color Palette */
#define WIN11_BG           0xFFFF    /* Pure white background */
#define WIN11_BG_GRAY      0xF7BE    /* #F3F3F3 Light gray */
#define WIN11_ACCENT       0x03DA    /* #0078D4 Windows blue */
#define WIN11_ACCENT_LIGHT 0x4E1C    /* #9DCBF0 Light blue */
#define WIN11_ACCENT_PALE  0xE73C    /* #E3F2FD Very light blue */
#define WIN11_TEXT_PRIMARY 0x18E7    /* #1F1F1F Near black */
#define WIN11_TEXT_SECOND  0x632C    /* #666666 Gray */
#define WIN11_BORDER       0xE71C    /* #E5E5E5 Border gray */
#define WIN11_SHADOW       0xD69A    /* #D0D0D0 Subtle shadow */
#define WIN11_HOVER_BG     0xF7DF    /* #F5F5F5 Hover state */

/* Menu context */
static MenuContext_t menu_ctx = {0};

/**
 * @brief Draw soft rounded rectangle (Windows 11 style - larger radius)
 */
static void LCD_FillRoundRect(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    /* Fill main rectangle */
    LCD_Fill(x1 + 3, y1, x2 - 3, y2, color);      /* Top/bottom margins */
    LCD_Fill(x1 + 2, y1 + 1, x2 - 2, y2 - 1, color);
    LCD_Fill(x1 + 1, y1 + 2, x2 - 1, y2 - 2, color);
    LCD_Fill(x1, y1 + 3, x2, y2 - 3, color);      /* Full height center */
}

/**
 * @brief Draw rounded border only (Windows 11 style)
 */
static void LCD_DrawRoundRectBorder(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    /* Horizontal lines */
    LCD_DrawLine(x1 + 4, y1, x2 - 4, y1, color);
    LCD_DrawLine(x1 + 4, y2 - 1, x2 - 4, y2 - 1, color);

    /* Vertical lines */
    LCD_DrawLine(x1, y1 + 4, x1, y2 - 4, color);
    LCD_DrawLine(x2 - 1, y1 + 4, x2 - 1, y2 - 4, color);

    /* Corner pixels for smooth radius */
    LCD_DrawLine(x1 + 2, y1 + 1, x1 + 3, y1 + 1, color);
    LCD_DrawLine(x2 - 4, y1 + 1, x2 - 3, y1 + 1, color);
    LCD_DrawLine(x1 + 1, y1 + 2, x1 + 1, y1 + 3, color);
    LCD_DrawLine(x2 - 2, y1 + 2, x2 - 2, y1 + 3, color);

    LCD_DrawLine(x1 + 2, y2 - 2, x1 + 3, y2 - 2, color);
    LCD_DrawLine(x2 - 4, y2 - 2, x2 - 3, y2 - 2, color);
    LCD_DrawLine(x1 + 1, y2 - 4, x1 + 1, y2 - 3, color);
    LCD_DrawLine(x2 - 2, y2 - 4, x2 - 2, y2 - 3, color);
}

/* Helper: Ease-out cubic function */
static float ease_out_cubic(float t) {
    float f = t - 1.0f;
    return f * f * f + 1.0f;
}

/**
 * @brief Initialize menu system
 */
void Menu_Init(void)
{
    memset(&menu_ctx, 0, sizeof(menu_ctx));
    menu_ctx.selected_index     = 0;
    menu_ctx.target_index       = 0;
    menu_ctx.scroll_position    = 0.0f;
    menu_ctx.scroll_velocity    = 0.0f;
    menu_ctx.is_animating       = false;
    menu_ctx.item_count         = 0;
    menu_ctx.needs_render       = true;  /* Initial render needed */

    /* Clear screen once at init */
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
}

/**
 * @brief Add menu item
 */
bool Menu_AddItem(const char *text, void (*callback)(void))
{
    if (menu_ctx.item_count >= MENU_MAX_ITEMS) {
        return false;
    }

    MenuItem_t *item = &menu_ctx.items[menu_ctx.item_count];
    item->text = text;
    item->callback = callback;
    item->enabled = true;

    menu_ctx.item_count++;
    return true;
}

/**
 * @brief Remove all menu items
 */
void Menu_Clear(void)
{
    menu_ctx.item_count = 0;
    menu_ctx.selected_index = 0;
    menu_ctx.target_index = 0;
    menu_ctx.scroll_position = 0.0f;
    menu_ctx.scroll_velocity = 0.0f;
    menu_ctx.is_animating = false;
}

/**
 * @brief Navigate to next item
 */
void Menu_SelectNext(void)
{
    if (menu_ctx.item_count == 0) return;

    menu_ctx.target_index = (menu_ctx.selected_index + 1) % menu_ctx.item_count;
    menu_ctx.anim_start_time = HAL_GetTick();
    menu_ctx.is_animating = true;
    menu_ctx.needs_render = true;  /* Mark for re-render */
}

/**
 * @brief Navigate to previous item
 */
void Menu_SelectPrevious(void)
{
    if (menu_ctx.item_count == 0) return;

    menu_ctx.target_index = (menu_ctx.selected_index - 1 + menu_ctx.item_count) % menu_ctx.item_count;
    menu_ctx.anim_start_time = HAL_GetTick();
    menu_ctx.is_animating = true;
    menu_ctx.needs_render = true;  /* Mark for re-render */
}

/**
 * @brief Execute selected item callback
 */
void Menu_ExecuteSelected(void)
{
    if (menu_ctx.selected_index >= 0 && menu_ctx.selected_index < menu_ctx.item_count) {
        MenuItem_t *item = &menu_ctx.items[menu_ctx.selected_index];
        if (item->enabled && item->callback) {
            item->callback();
        }
    }
}

/**
 * @brief Get currently selected item index
 */
int8_t Menu_GetSelectedIndex(void)
{
    return menu_ctx.selected_index;
}

/**
 * @brief Handle button input
 */
void Menu_HandleInput(void)
{
    ButtonEvent_t event = Button_GetEvent();

    if (event == BUTTON_EVENT_PRESS) {
        /* Short press: Navigate to next item */
        USART1_SendString("[MENU] Short press - Next\r\n");
        Menu_SelectNext();
    } else if (event == BUTTON_EVENT_LONG_PRESS) {
        /* Long press: Navigate to previous item */
        USART1_SendString("[MENU] Long press - Previous\r\n");
        Menu_SelectPrevious();
    }
}

/**
 * @brief Update menu animation with damping
 */
void Menu_Update(void)
{
    if (!menu_ctx.is_animating) {
        return;
    }

    uint32_t current_time = HAL_GetTick();
    uint32_t elapsed = current_time - menu_ctx.anim_start_time;

    if (elapsed >= MENU_ANIM_DURATION_MS) {
        /* Animation complete */
        menu_ctx.selected_index = menu_ctx.target_index;
        menu_ctx.scroll_position = (float)menu_ctx.selected_index;
        menu_ctx.scroll_velocity = 0.0f;
        menu_ctx.is_animating = false;
        menu_ctx.needs_render = true;  /* Final render needed */
    } else {
        /* Calculate animation progress with ease-out cubic */
        float progress = (float)elapsed / (float)MENU_ANIM_DURATION_MS;
        float eased_progress = ease_out_cubic(progress);

        /* Interpolate scroll position */
        float start_pos = (float)menu_ctx.selected_index;
        float end_pos = (float)menu_ctx.target_index;

        /* Handle wrapping for circular menu */
        float delta = end_pos - start_pos;
        if (fabsf(delta) > (float)menu_ctx.item_count / 2.0f) {
            if (delta > 0) {
                start_pos += (float)menu_ctx.item_count;
            } else {
                end_pos += (float)menu_ctx.item_count;
            }
        }

        menu_ctx.scroll_position = start_pos + (end_pos - start_pos) * eased_progress;

        /* Apply damping to velocity for natural feel */
        menu_ctx.scroll_velocity = (end_pos - menu_ctx.scroll_position) * (1.0f - eased_progress) * MENU_DAMPING_FACTOR;
        menu_ctx.needs_render = true;  /* Render during animation */
    }
}

/**
 * @brief Render menu to LCD - Windows 11 Fluent Design style
 */
void Menu_Render(void)
{
    /* Only render if needed */
    if (!menu_ctx.needs_render) {
        return;
    }
    menu_ctx.needs_render = false;

    uint16_t center_y = LCD_H / 2;

    /* Clear menu area with white background */
    LCD_Fill(0, 0, LCD_W, LCD_H, WIN11_BG);

    /* Draw clean title bar - Windows 11 style */
    static bool title_drawn = false;
    if (!title_drawn) {
        /* Title bar background - solid Windows blue */
        LCD_Fill(0, 0, LCD_W, 24, WIN11_ACCENT);

        /* Title text */
        LCD_ShowString(8, 6, (const u8 *)"MENU", WIN11_BG, WIN11_ACCENT, 12, 0);

        title_drawn = true;
    }

    /* Calculate visible item range */
    float center_offset = menu_ctx.scroll_position;
    int center_item = (int)(center_offset + 0.5f) % menu_ctx.item_count;
    if (center_item < 0) center_item += menu_ctx.item_count;

    /* Render visible items */
    for (int i = 0; i < MENU_VISIBLE_ITEMS; i++) {
        int item_index = (center_item - MENU_VISIBLE_ITEMS / 2 + i + menu_ctx.item_count) % menu_ctx.item_count;
        if (item_index < 0 || item_index >= menu_ctx.item_count) continue;

        MenuItem_t *item = &menu_ctx.items[item_index];

        /* Calculate item position with smooth offset */
        float item_offset = (float)item_index - center_offset;

        /* Wrap offset for circular menu */
        if (item_offset > (float)menu_ctx.item_count / 2.0f) {
            item_offset -= (float)menu_ctx.item_count;
        } else if (item_offset < -(float)menu_ctx.item_count / 2.0f) {
            item_offset += (float)menu_ctx.item_count;
        }

        int y_pos = center_y + (int)(item_offset * (float)MENU_ITEM_HEIGHT);

        /* Skip items outside visible area */
        if (y_pos < 26 || y_pos >= LCD_H - MENU_ITEM_HEIGHT) continue;

        bool is_selected = (item_index == menu_ctx.target_index);

        /* Calculate fade effect for non-selected items based on distance */
        float distance = fabsf(item_offset);

        if (is_selected) {
            /* Selected item - Windows 11 flat design with rounded corners */
            int item_h = MENU_ITEM_HEIGHT - 6;
            int item_y = y_pos + 3;
            int item_x1 = 8;
            int item_x2 = LCD_W - 8;

            /* Fill with pale blue background */
            LCD_FillRoundRect(item_x1, item_y, item_x2, item_y + item_h, WIN11_ACCENT_PALE);

            /* Draw blue border */
            LCD_DrawRoundRectBorder(item_x1, item_y, item_x2, item_y + item_h, WIN11_ACCENT);

            /* Draw text - dark on light blue */
            LCD_ShowString(item_x1 + 10, y_pos + (MENU_ITEM_HEIGHT - MENU_FONT_SIZE) / 2,
                          (const u8 *)item->text, WIN11_TEXT_PRIMARY, WIN11_ACCENT_PALE, MENU_FONT_SIZE, 0);
        } else {
            /* Non-selected item - simple text on white */
            uint16_t text_color = (distance > 1.5f) ? WIN11_TEXT_SECOND : WIN11_TEXT_PRIMARY;

            LCD_ShowString(18, y_pos + (MENU_ITEM_HEIGHT - MENU_FONT_SIZE) / 2,
                          (const u8 *)item->text, text_color, WIN11_BG, MENU_FONT_SIZE, 0);

            /* Subtle separator line for distant items */
            if (distance > 0.8f) {
                LCD_DrawLine(24, y_pos + MENU_ITEM_HEIGHT - 1, LCD_W - 24,
                            y_pos + MENU_ITEM_HEIGHT - 1, WIN11_BORDER);
            }
        }
    }

    /* Draw clean scroll indicator - Windows 11 style */
    if (menu_ctx.item_count > MENU_VISIBLE_ITEMS) {
        uint16_t indicator_h = ((LCD_H - 30) * MENU_VISIBLE_ITEMS) / menu_ctx.item_count;
        uint16_t indicator_y = 28 + ((LCD_H - 30 - indicator_h) * menu_ctx.selected_index) / (menu_ctx.item_count - 1);

        /* Background track - light gray */
        LCD_Fill(LCD_W - 5, 28, LCD_W - 3, LCD_H - 2, WIN11_BG_GRAY);

        /* Active indicator - solid Windows blue */
        LCD_Fill(LCD_W - 5, indicator_y, LCD_W - 3, indicator_y + indicator_h, WIN11_ACCENT);
    }
}
