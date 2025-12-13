/**
 * @file button_test_simple.c
 * @brief Simplified button test - only key events to UART
 */

#include "button_test_demo.h"
#include "button.h"
#include "lcd_init.h"
#include "lcd.h"
#include "spi.h"
#include "dma.h"
#include "bsp_usart.h"
#include "hal_compat.h"
#include "n32g430_gpio.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Statistics */
static uint32_t press_count = 0;
static uint32_t long_press_count = 0;
static uint32_t release_count = 0;

/**
 * @brief UART print (simplified)
 */
static void uart_print(const char *str)
{
    while (*str) {
        while (USART_Flag_Status_Get(USART1, USART_FLAG_TXDE) == RESET);
        USART_Data_Send(USART1, *str++);
    }
}

/**
 * @brief Initialize button test
 */
void ButtonTest_Init(void)
{
    /* Initialize hardware */
    USART1_Init();          /* Initialize UART1 first! */
    MX_DMA_SPI1_Init();
    MX_SPI1_Init();
    LCD_Init();
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

    /* Title */
    LCD_Fill(0, 0, LCD_W, 25, BLUE);
    LCD_ShowString(20, 5, (const u8 *)"Key Test", WHITE, BLUE, 16, 0);

    /* Instructions */
    LCD_ShowString(5, 35, (const u8 *)"PB11: Button", CYAN, BLACK, 12, 0);
    LCD_ShowString(5, 55, (const u8 *)"Short: <70ms", WHITE, BLACK, 12, 0);
    LCD_ShowString(5, 75, (const u8 *)"Long:  >=70ms", YELLOW, BLACK, 12, 0);
    LCD_ShowString(5, 100, (const u8 *)"Check UART1", GREEN, BLACK, 12, 0);
    LCD_ShowString(5, 120, (const u8 *)"115200 bps", GREEN, BLACK, 12, 0);

    /* UART startup */
    uart_print("\r\n=== Button Test ===\r\n");
    uart_print("Sample: 30ms, Threshold: 0.7\r\n");
    uart_print("Long press: 70ms\r\n");
    uart_print("Initializing button...\r\n");

    /* Initialize button */
    Button_Init();
    uart_print("Button_Init() done\r\n");

    Button_StartSampling();
    uart_print("Button_StartSampling() done\r\n");
    uart_print("Ready! Press PB11...\r\n\r\n");
}

/**
 * @brief Main test loop
 */
void ButtonTest_Main(void)
{
    ButtonEvent_t event = Button_GetEvent();
    char msg[64];

    /* Debug: read PB11 state directly */
    static uint32_t last_debug_time = 0;
    static uint8_t last_pin_state = 1;
    uint32_t current_time = HAL_GetTick();

    if (current_time - last_debug_time >= 1000) {
        GPIO_Module *gpio = GPIOB;
        uint8_t pin_state = (gpio->PID & (1 << 11)) ? 1 : 0;

        if (pin_state != last_pin_state) {
            sprintf(msg, "[DEBUG] PB11 state: %d\r\n", pin_state);
            uart_print(msg);
            last_pin_state = pin_state;
        }
        last_debug_time = current_time;
    }

    if (event == BUTTON_EVENT_PRESS) {
        press_count++;
        sprintf(msg, "[%lu] SHORT PRESS #%lu\r\n", HAL_GetTick(), press_count);
        uart_print(msg);

        /* Visual feedback */
        LCD_Fill(0, 140, LCD_W, 160, GREEN);
        HAL_Delay(50);
        LCD_Fill(0, 140, LCD_W, 160, BLACK);

    } else if (event == BUTTON_EVENT_LONG_PRESS) {
        long_press_count++;
        sprintf(msg, "[%lu] LONG PRESS #%lu\r\n", HAL_GetTick(), long_press_count);
        uart_print(msg);

        /* Visual feedback */
        LCD_Fill(0, 140, LCD_W, 160, YELLOW);
        HAL_Delay(50);
        LCD_Fill(0, 140, LCD_W, 160, BLACK);

    } else if (event == BUTTON_EVENT_RELEASE) {
        release_count++;
        sprintf(msg, "[%lu] RELEASE #%lu\r\n\r\n", HAL_GetTick(), release_count);
        uart_print(msg);
    }

    HAL_Delay(5);
}
