/**
 * @file timer.h
 * @brief SysTick Hardware Timer Delay Functions
 * @version v1.0.0
 * @author N32G430 Project
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include "n32g430.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SysTick Timer Configuration */
#define SYSTICK_FREQUENCY_HZ    1000000U    /* 1MHz for microsecond precision */

/* Function Prototypes */
void Timer_Init(void);
void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);
uint32_t Get_Tick_Count(void);
void Timer_Reset(void);

/* Advanced Functions */
void Timer_Start(void);
void Timer_Stop(void);
uint32_t Timer_Get_Elapsed_us(void);
uint32_t Timer_Get_Elapsed_ms(void);
void Delay_us_Precise(uint32_t us);
uint32_t Timer_Get_System_Clock_Hz(void);
uint8_t Timer_Is_Initialized(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H__ */ 