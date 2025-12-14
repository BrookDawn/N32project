/**
 * @file lcd_menu_demo.c
 * @brief Circular scrolling menu demo - scroll only, no submenus
 */

#include "lcd_menu_demo.h"
#include "lcd_init.h"
#include "lcd_menu.h"
#include "button.h"
#include "spi.h"
#include "dma.h"
#include "hal_compat.h"
#include "bsp_usart.h"
#include <stdio.h>

/**
 * @brief Initialize menu demo
 */
void LCD_MenuDemo_Init(void)
{
    /* Initialize UART1 for debugging */
    USART1_Init();
    USART1_SendString("\r\n=== N32G430 Circular Menu ===\r\n");
    USART1_SendString("Short press: Next item\r\n");
    USART1_SendString("Long press: Previous item\r\n\r\n");

    /* Initialize DMA for SPI1 */
    MX_DMA_SPI1_Init();

    /* Initialize SPI1 */
    MX_SPI1_Init();

    /* Initialize LCD hardware */
    LCD_Init();

    /* Clear screen */
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

    /* Welcome message */
    LCD_ShowString(35, 60, (const u8 *)"N32G430", CYAN, BLACK, 24, 0);
    LCD_ShowString(15, 90, (const u8 *)"Circular Menu", YELLOW, BLACK, 16, 0);
    LCD_ShowString(20, 115, (const u8 *)"Initializing...", WHITE, BLACK, 12, 0);

    /* Initialize button */
    USART1_SendString("Initializing button...\r\n");
    Button_Init();
    Button_StartSampling();
    USART1_SendString("Button ready!\r\n");

    /* Initialize menu system */
    USART1_SendString("Initializing menu...\r\n");
    Menu_Init();

    /* Add menu items - no callbacks needed for scroll-only demo */
    Menu_AddItem("Bouncing Balls", NULL);
    Menu_AddItem("Graphics Test", NULL);
    Menu_AddItem("Performance", NULL);
    Menu_AddItem("Plasma Effect", NULL);
    Menu_AddItem("Particles", NULL);
    Menu_AddItem("Settings", NULL);
    Menu_AddItem("About", NULL);
    Menu_AddItem("Demo 8", NULL);
    Menu_AddItem("Demo 9", NULL);
    Menu_AddItem("Demo 10", NULL);

    USART1_SendString("Menu ready!\r\n\r\n");

    /* Initial render - must call once to display menu */
    Menu_Render();
    USART1_SendString("Menu displayed!\r\n");
}

/**
 * @brief Main loop for circular menu demo
 */
void LCD_MenuDemo_Main(void)
{
    /* Optional debug output every 2 seconds */
    static uint32_t last_debug = 0;
    uint32_t now = HAL_GetTick();
    if (now - last_debug >= 2000) {
        char debug_buf[80];
        Button_GetDebugInfo(debug_buf, sizeof(debug_buf));
        USART1_SendString(debug_buf);
        USART1_SendString("\r\n");
        last_debug = now;
    }

    /* Handle button input */
    Menu_HandleInput();

    /* Update menu animation */
    Menu_Update();

    /* Render menu */
    Menu_Render();
}
