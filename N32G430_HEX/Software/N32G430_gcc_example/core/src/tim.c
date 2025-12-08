#include "tim.h"
#include "n32g430_rcc.h"
#include "misc.h"
#include "system_n32g430.h"

__attribute__((weak)) void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
extern uint32_t SystemCoreClock;

TIM_HandleTypeDef htim4;

void MX_TIM4_Init(void)
{
    uint32_t prescaler = (SystemCoreClock / 1000000U) - 1U;
    TIM_TimeBaseInitType TIM_BaseStruct;

    RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_TIM4);

    TIM_Base_Struct_Initialize(&TIM_BaseStruct);
    TIM_BaseStruct.Prescaler = (uint16_t)prescaler;
    TIM_BaseStruct.Period = 1000U - 1U;
    TIM_BaseStruct.CntMode = TIM_CNT_MODE_UP;
    TIM_BaseStruct.ClkDiv = TIM_CLK_DIV1;

    TIM_Base_Initialize(TIM4, &TIM_BaseStruct);
    TIM_Base_Count_Set(TIM4, 0);
    TIM_Interrupt_Status_Clear(TIM4, TIM_INT_UPDATE);

    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);

    htim4.Instance = TIM4;
    htim4.Prescaler = prescaler;
    htim4.Period = TIM_BaseStruct.Period;
}

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim)
{
    if (htim == NULL || htim->Instance == NULL)
    {
        return HAL_ERROR;
    }

    TIM_Interrupt_Status_Clear(htim->Instance, TIM_INT_UPDATE);
    TIM_Interrupt_Enable(htim->Instance, TIM_INT_UPDATE);
    TIM_On(htim->Instance);

    return HAL_OK;
}

void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim)
{
    if (TIM_Interrupt_Status_Get(htim->Instance, TIM_INT_UPDATE) == SET)
    {
        TIM_Interrupt_Status_Clear(htim->Instance, TIM_INT_UPDATE);
        HAL_TIM_PeriodElapsedCallback(htim);
    }
}

__attribute__((weak)) void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    (void)htim;
}
