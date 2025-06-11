/**
 * @file main.c
 * @brief N32G430C8L7 Main Application
 * @version v1.0.0
 */

#include "n32g430.h"
#include "n32g430_conf.h"
#include "timer.h"
#include "usart.h"
#include "gpio.h"

/* Private function prototypes */
void SystemClock_Config(void);

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize SysTick Timer */
    Timer_Init();

    /* Configure GPIO for LED */
    LED_GPIO_Config();

    /* Initialize USART1 */
    USART1_Init(USART1_BAUDRATE);

    /* Send startup message */
    USART1_Test_Communication();

    /* Infinite loop */
    while (1)
    {
        /* Toggle LED */
        GPIO_Pin_Toggle(GPIOC, GPIO_PIN_13);

        /* Send status via USART */
        USART1_Printf("LED Toggle - Tick: %u ms\r\n", Get_Tick_Count() / 1000);

        /* Delay */
        Delay_ms(1000);
    }
}

/**
 * @brief  System Clock Configuration
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
    ErrorStatus HSEStartUpStatus;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
    /* Enable HSE */
    RCC_HSE_Config(RCC_HSE_ENABLE);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_HSE_Stable_Wait();

    if (HSEStartUpStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
        FLASH_Prefetch_Buffer_Enable();

        /* Flash 2 wait state */
        FLASH_Latency_Set(FLASH_LATENCY_2);

        /* HCLK = SYSCLK */
        RCC_Hclk_Config(RCC_SYSCLK_DIV1);

        /* PCLK2 = HCLK */
        RCC_Pclk2_Config(RCC_HCLK_DIV1);

        /* PCLK1 = HCLK/2 */
        RCC_Pclk1_Config(RCC_HCLK_DIV2);

        /* PLLCLK = 8MHz * 16 = 128 MHz */
        RCC_PLL_Config(RCC_PLL_SRC_HSE_DIV1, RCC_PLL_MUL_16);

        /* Enable PLL */
        RCC_PLL_Enable();

        /* Wait till PLL is ready */
        while (RCC_Flag_Status_Get(RCC_FLAG_PLLRD) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_Sysclk_Config(RCC_SYSCLK_SRC_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_Sysclk_Source_Get() != 0x08)
        {
        }
    }
}

