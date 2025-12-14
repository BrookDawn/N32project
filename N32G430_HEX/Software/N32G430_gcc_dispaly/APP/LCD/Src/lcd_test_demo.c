/**
 * @file lcd_test_demo.c
 * @brief LCD ST7735S test demo implementation
 */

#include "lcd_test_demo.h"
#include "lcd_init.h"
#include "lcd.h"
#include "spi.h"
#include "dma.h"
#include "hal_compat.h"

/* Test state machine */
typedef enum {
    LCD_TEST_COLOR_FILL,
    LCD_TEST_TEXT,
    LCD_TEST_GRAPHICS,
    LCD_TEST_COUNT
} LCD_TestState_t;

static LCD_TestState_t test_state = LCD_TEST_COLOR_FILL;
static uint32_t last_switch_time = 0;
static uint8_t color_index = 0;

/* Color list for testing */
static const uint16_t test_colors[] = {
    RED, GREEN, BLUE, WHITE, BLACK, YELLOW, CYAN, MAGENTA
};
#define NUM_COLORS (sizeof(test_colors) / sizeof(test_colors[0]))

/**
 * @brief Initialize LCD test demo
 */
void LCD_TestDemo_Init(void)
{
    /* Initialize DMA for SPI1 */
    MX_DMA_SPI1_Init();

    /* Initialize SPI1 */
    MX_SPI1_Init();

    /* Initialize LCD */
    LCD_Init();

    /* Clear screen to black */
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

    /* Display initial message */
    LCD_ShowString(10, 10, (const u8 *)"LCD Test Demo", WHITE, BLACK, 16, 0);
    LCD_ShowString(10, 30, (const u8 *)"N32G430 + SPI1", WHITE, BLACK, 16, 0);

    HAL_Delay(1000);

    last_switch_time = HAL_GetTick();
    test_state = LCD_TEST_COLOR_FILL;
}

/**
 * @brief LCD test demo main loop
 */
void LCD_TestDemo_Main(void)
{
    uint32_t current_time = HAL_GetTick();

    /* Switch test every 3 seconds */
    if (current_time - last_switch_time >= 3000)
    {
        last_switch_time = current_time;
        test_state = (LCD_TestState_t)((test_state + 1) % LCD_TEST_COUNT);
        color_index = 0;
    }

    switch (test_state)
    {
        case LCD_TEST_COLOR_FILL:
            LCD_TestDemo_ColorFill();
            break;

        case LCD_TEST_TEXT:
            LCD_TestDemo_Text();
            break;

        case LCD_TEST_GRAPHICS:
            LCD_TestDemo_Graphics();
            break;

        default:
            test_state = LCD_TEST_COLOR_FILL;
            break;
    }
}

/**
 * @brief LCD color fill test
 */
void LCD_TestDemo_ColorFill(void)
{
    static uint32_t last_color_time = 0;
    uint32_t current_time = HAL_GetTick();

    /* Change color every 500ms */
    if (current_time - last_color_time >= 500)
    {
        last_color_time = current_time;

        LCD_Fill(0, 0, LCD_W, LCD_H, test_colors[color_index]);

        /* Display color name */
        const char *color_names[] = {"RED", "GREEN", "BLUE", "WHITE", "BLACK", "YELLOW", "CYAN", "MAGENTA"};
        uint16_t text_color = (color_index == 4 || color_index == 2) ? WHITE : BLACK;  /* White text on dark colors */

        LCD_ShowString(30, 70, (const u8 *)color_names[color_index], text_color, test_colors[color_index], 16, 0);

        color_index = (color_index + 1) % NUM_COLORS;
    }
}

/**
 * @brief LCD text display test
 */
void LCD_TestDemo_Text(void)
{
    static uint8_t initialized = 0;

    if (!initialized)
    {
        LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

        /* Show different font sizes */
        LCD_ShowString(5, 10, (const u8 *)"Font 12px", WHITE, BLACK, 12, 0);
        LCD_ShowString(5, 30, (const u8 *)"Font 16px", GREEN, BLACK, 16, 0);
        LCD_ShowString(5, 55, (const u8 *)"Font 24", YELLOW, BLACK, 24, 0);
        LCD_ShowString(5, 85, (const u8 *)"32px", CYAN, BLACK, 32, 0);

        /* Show numbers */
        LCD_ShowIntNum(5, 125, 12345, 5, RED, BLACK, 16);
        LCD_ShowFloatNum1(70, 125, 3.14, 4, MAGENTA, BLACK, 16);

        initialized = 1;
    }
}

/**
 * @brief LCD graphics test
 */
void LCD_TestDemo_Graphics(void)
{
    static uint8_t initialized = 0;
    static uint8_t animation_step = 0;
    static uint32_t last_anim_time = 0;
    uint32_t current_time = HAL_GetTick();

    if (!initialized)
    {
        LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

        /* Draw static graphics */
        LCD_DrawRectangle(5, 5, 60, 40, RED);
        LCD_DrawRectangle(65, 5, 120, 40, GREEN);

        LCD_DrawLine(5, 50, 120, 50, YELLOW);
        LCD_DrawLine(5, 55, 120, 100, CYAN);

        Draw_Circle(30, 120, 20, MAGENTA);
        Draw_Circle(90, 120, 15, WHITE);

        initialized = 1;
        last_anim_time = current_time;
    }

    /* Animate a moving point */
    if (current_time - last_anim_time >= 100)
    {
        last_anim_time = current_time;

        /* Erase old point */
        uint8_t old_x = 10 + (animation_step > 0 ? animation_step - 1 : 0) * 2;
        if (old_x < LCD_W - 10)
        {
            LCD_DrawPoint(old_x, 145, BLACK);
        }

        /* Draw new point */
        uint8_t new_x = 10 + animation_step * 2;
        if (new_x < LCD_W - 10)
        {
            LCD_DrawPoint(new_x, 145, WHITE);
            animation_step++;
        }
        else
        {
            animation_step = 0;
        }
    }
}
