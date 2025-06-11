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

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* Private function prototypes */
void SystemClock_Config(void);
static void LED_Task(void *pvParameters);
static void ConfigureNVIC(void);

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Configure the system clock */
    SystemClock_Config();

    /* Configure NVIC for FreeRTOS */
    ConfigureNVIC();

    /* Initialize SysTick Timer */
    Timer_Init();

    /* Configure GPIO for LED */
    LED_GPIO_Config();

    /* Initialize USART1 */
    USART1_Init(USART1_BAUDRATE);

    /* Send startup message */
    USART1_Test_Communication();

    /* Create LED task */
    xTaskCreate(LED_Task, "LED Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    /* Start the scheduler */
    vTaskStartScheduler();

    /* Should never reach here */
    while (1)
    {
    }
}

/**
 * @brief Configure NVIC for FreeRTOS
 * @note  Sets priorities for FreeRTOS system interrupts
 */
static void ConfigureNVIC(void)
{
    /* Configure PendSV with lowest priority */
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    
    /* Configure SysTick with second lowest priority */
    NVIC_SetPriority(SysTick_IRQn, 0xFE);
}

/**
 * @brief LED Task
 * @param pvParameters Task parameters (not used)
 */
static void LED_Task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xDelay = pdMS_TO_TICKS(1000);

    /* Initialize the xLastWakeTime variable with the current time */
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        /* Toggle LED */
        GPIO_Pin_Toggle(GPIOC, GPIO_PIN_13);

        /* Send status via USART */
        USART1_Printf("LED Toggle - Tick: %u ms\r\n", xTaskGetTickCount());

        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, xDelay);
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

/**
 * @brief  Stack overflow hook
 * @param  xTask Handle of the task that has overflowed its stack
 * @param  pcTaskName Name of the task that has overflowed its stack
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* Handle stack overflow error */
    while(1)
    {
        /* Infinite loop */
    }
}

/**
 * @brief  Malloc failed hook
 */
void vApplicationMallocFailedHook(void)
{
    /* Handle memory allocation failure */
    while(1)
    {
        /* Infinite loop */
    }
}

