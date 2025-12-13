/**
 * @file lcd_menu.h
 * @brief Smooth scrolling menu system for ST7735S LCD (landscape mode)
 */

#ifndef LCD_MENU_H
#define LCD_MENU_H

#include "lcd.h"
#include "button.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Menu configuration */
#define MENU_MAX_ITEMS          10      /* Maximum menu items */
#define MENU_VISIBLE_ITEMS      5       /* Number of visible items (1 page) */
#define MENU_ITEM_HEIGHT        32      /* Height of each menu item in pixels */
#define MENU_FONT_SIZE          16      /* Font size for menu items */

/* Animation configuration */
#define MENU_ANIM_DURATION_MS   300     /* Animation duration in ms */
#define MENU_DAMPING_FACTOR     0.8f    /* Damping factor for smooth stop (0-1) */
#define MENU_FPS                60      /* Target frame rate for animation */

/* Menu item structure */
typedef struct {
    const char *text;                   /* Menu item text */
    void (*callback)(void);             /* Callback function when selected */
    bool enabled;                       /* Item enabled/disabled */
} MenuItem_t;

/* Menu context */
typedef struct {
    MenuItem_t items[MENU_MAX_ITEMS];   /* Menu items array */
    uint8_t item_count;                 /* Number of items */
    int8_t selected_index;              /* Currently selected item */
    int8_t target_index;                /* Target index for animation */
    float scroll_position;              /* Current scroll position (float for smooth anim) */
    float scroll_velocity;              /* Scroll velocity for damping */
    uint32_t anim_start_time;           /* Animation start time */
    bool is_animating;                  /* Animation in progress */
    bool needs_render;                  /* Dirty flag - needs re-render */
} MenuContext_t;

/* Public API */

/**
 * @brief Initialize menu system
 */
void Menu_Init(void);

/**
 * @brief Add menu item
 * @param text: Menu item text
 * @param callback: Callback function (can be NULL)
 * @return true if added successfully
 */
bool Menu_AddItem(const char *text, void (*callback)(void));

/**
 * @brief Remove all menu items
 */
void Menu_Clear(void);

/**
 * @brief Handle button input and update menu state
 */
void Menu_HandleInput(void);

/**
 * @brief Update menu animation
 * @note Call this in main loop for smooth animation
 */
void Menu_Update(void);

/**
 * @brief Render menu to LCD
 */
void Menu_Render(void);

/**
 * @brief Get currently selected item index
 * @return Selected index, -1 if no selection
 */
int8_t Menu_GetSelectedIndex(void);

/**
 * @brief Navigate to next item
 */
void Menu_SelectNext(void);

/**
 * @brief Navigate to previous item
 */
void Menu_SelectPrevious(void);

/**
 * @brief Execute selected item callback
 */
void Menu_ExecuteSelected(void);

#ifdef __cplusplus
}
#endif

#endif /* LCD_MENU_H */
