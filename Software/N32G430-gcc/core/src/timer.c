/**
 * @file timer.c
 * @brief SysTick Hardware Timer Delay Implementation
 * @version v1.0.0
 * @author N32G430 Project
 */

#include "timer.h"

/* Private Variables */
static volatile uint32_t tick_counter = 0;
static volatile uint32_t timer_start_tick = 0;
static uint32_t systick_reload_value = 0;

/**
 * @brief Initialize SysTick Timer for precise delay functions
 * @param None
 * @retval None
 * @note System clock is assumed to be 128MHz
 */
void Timer_Init(void)
{
    uint32_t system_clock_hz;
    
    /* Get system clock frequency */
    RCC_ClocksType clocks;
    RCC_Clocks_Frequencies_Value_Get(&clocks);
    system_clock_hz = clocks.SysclkFreq;
    
    /* Calculate reload value for 1MHz (1μs resolution) */
    systick_reload_value = system_clock_hz / SYSTICK_FREQUENCY_HZ - 1;
    
    /* Configure SysTick */
    SysTick->LOAD = systick_reload_value;           /* Set reload value */
    SysTick->VAL = 0;                               /* Clear current value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE |        /* Use processor clock */
                    SysTick_CTRL_TICKINT |          /* Enable interrupt */
                    SysTick_CTRL_ENABLE;            /* Enable SysTick */
    
    /* Reset tick counter */
    tick_counter = 0;
}

/**
 * @brief SysTick interrupt handler
 * @param None
 * @retval None
 */
void SysTick_Handler(void)
{
    tick_counter++;
}

/**
 * @brief Delay function in milliseconds
 * @param ms: delay time in milliseconds
 * @retval None
 */
void Delay_ms(uint32_t ms)
{
    if (ms == 0) return;
    
    Delay_us(ms * 1000U);
}

/**
 * @brief Delay function in microseconds
 * @param us: delay time in microseconds
 * @retval None
 */
void Delay_us(uint32_t us)
{
    if (us == 0) return;
    
    uint32_t start_tick = tick_counter;
    uint32_t target_ticks = us;
    
    /* Handle tick counter overflow */
    if (start_tick > (UINT32_MAX - target_ticks)) {
        /* Wait for overflow */
        while (tick_counter >= start_tick);
        start_tick = 0;
    }
    
    /* Wait for target ticks */
    while ((tick_counter - start_tick) < target_ticks);
}

/**
 * @brief Get current tick count in microseconds
 * @param None
 * @retval Current tick count
 */
uint32_t Get_Tick_Count(void)
{
    return tick_counter;
}

/**
 * @brief Reset tick counter
 * @param None
 * @retval None
 */
void Timer_Reset(void)
{
    tick_counter = 0;
}

/**
 * @brief Start timer for elapsed time measurement
 * @param None
 * @retval None
 */
void Timer_Start(void)
{
    timer_start_tick = tick_counter;
}

/**
 * @brief Stop timer (actually just record current tick)
 * @param None
 * @retval None
 */
void Timer_Stop(void)
{
    /* Timer continues running, this is just for API completeness */
}

/**
 * @brief Get elapsed time in microseconds since Timer_Start()
 * @param None
 * @retval Elapsed time in microseconds
 */
uint32_t Timer_Get_Elapsed_us(void)
{
    uint32_t current_tick = tick_counter;
    
    /* Handle overflow */
    if (current_tick >= timer_start_tick) {
        return (current_tick - timer_start_tick);
    } else {
        return (UINT32_MAX - timer_start_tick + current_tick + 1);
    }
}

/**
 * @brief Get elapsed time in milliseconds since Timer_Start()
 * @param None
 * @retval Elapsed time in milliseconds
 */
uint32_t Timer_Get_Elapsed_ms(void)
{
    return Timer_Get_Elapsed_us() / 1000U;
}

/**
 * @brief High precision delay using polling method (for very short delays)
 * @param us: delay time in microseconds (max 65535)
 * @retval None
 * @note This function uses polling and is more precise for short delays < 100us
 */
void Delay_us_Precise(uint32_t us)
{
    if (us == 0) return;
    
    /* For very short delays, use direct register polling */
    if (us < 100) {
        uint32_t system_clock_hz;
        RCC_ClocksType clocks;
        RCC_Clocks_Frequencies_Value_Get(&clocks);
        system_clock_hz = clocks.SysclkFreq;
        
        /* Calculate cycles needed */
        uint32_t cycles = (system_clock_hz / 1000000U) * us;
        cycles = cycles / 4; /* Approximate cycles per loop iteration */
        
        /* Simple delay loop */
        for (volatile uint32_t i = 0; i < cycles; i++) {
            __NOP(); /* No operation */
        }
    } else {
        /* Use SysTick for longer delays */
        Delay_us(us);
    }
}

/**
 * @brief Get system clock frequency
 * @param None
 * @retval System clock frequency in Hz
 */
uint32_t Timer_Get_System_Clock_Hz(void)
{
    RCC_ClocksType clocks;
    RCC_Clocks_Frequencies_Value_Get(&clocks);
    return clocks.SysclkFreq;
}

/**
 * @brief Check if timer is initialized
 * @param None
 * @retval 1 if initialized, 0 if not
 */
uint8_t Timer_Is_Initialized(void)
{
    return (SysTick->CTRL & SysTick_CTRL_ENABLE) ? 1 : 0;
} 