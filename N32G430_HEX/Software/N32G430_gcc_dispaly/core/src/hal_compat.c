#include "hal_compat.h"
#include "system_n32g430.h"

static volatile uint32_t s_hal_tick = 0;
uint32_t SystemCoreClock = 64000000;

void HAL_InitTick(void)
{
    SysTick_Config(SystemCoreClock / 1000U);
    NVIC_SetPriority(SysTick_IRQn, 0);
}

void HAL_IncTick(void)
{
    s_hal_tick++;
}

uint32_t HAL_GetTick(void)
{
    return s_hal_tick;
}

void HAL_Delay(uint32_t delay_ms)
{
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < delay_ms)
    {
        __NOP();
    }
}
