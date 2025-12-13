/**
 * @file button.c
 * @brief Button input capture with DMA debouncing implementation
 */

#include "button.h"
#include "n32g430_tim.h"
#include "n32g430_dma.h"
#include "n32g430_rcc.h"
#include "n32g430_gpio.h"
#include "misc.h"
#include "dma.h"
#include <stdio.h>

/* Button context - export for debug */
ButtonContext_t button_ctx = {0};

/**
 * @brief Initialize button input capture with software sampling
 */
void Button_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;

    /* Enable clocks */
    RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOB);
    RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_TIM2);

    /* Configure PB11 as GPIO input (not using TIM2_CH4 alternate function) */
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin              = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull        = GPIO_PULL_UP;  /* Pull-up for active low button */
    GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);

    /* Configure TIM2 time base - sample at 1kHz */
    TIM_Base_Struct_Initialize(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period    = 999;              /* 1ms period (128MHz/128/1000 = 1kHz) */
    TIM_TimeBaseStructure.Prescaler = 127;           /* 128MHz / 128 = 1MHz */
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    TIM_Base_Initialize(TIM2, &TIM_TimeBaseStructure);

    /* Enable TIM2 update interrupt for periodic sampling */
    TIM_Interrupt_Enable(TIM2, TIM_INT_UPDATE);

    /* Configure NVIC for TIM2 - Higher priority than DMA to ensure sampling */
    NVIC_InitStructure.NVIC_IRQChannel                      = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 0;  /* Highest priority! */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);

    /* Initialize button context */
    button_ctx.capture_complete     = false;
    button_ctx.is_pressed           = false;
    button_ctx.long_press_triggered = false;
    button_ctx.pending_event        = BUTTON_EVENT_NONE;
    button_ctx.last_event_time      = 0;
    button_ctx.press_start_time     = 0;
    button_ctx.sample_index         = 0;

    /* Start timer */
    TIM_On(TIM2);
}

/**
 * @brief Start button sampling
 */
void Button_StartSampling(void)
{
    button_ctx.sample_index = 0;
    button_ctx.capture_complete = false;
}

/**
 * @brief Stop button sampling
 */
void Button_StopSampling(void)
{
    /* Nothing to do - just stop incrementing sample_index */
}

/**
 * @brief Process debounce buffer and detect button state
 */
static bool Button_ProcessSamples(void)
{
    uint8_t zero_count = 0;

    /* Count number of zeros (button pressed = GPIO low = 0) */
    for (uint8_t i = 0; i < BUTTON_SAMPLE_SIZE; i++) {
        if (button_ctx.sample_buffer[i] == 0) {  /* 0 = button pressed */
            zero_count++;
        }
    }

    /* Button is pressed if zeros exceed threshold */
    return (zero_count >= BUTTON_PRESS_THRESHOLD);
}

/**
 * @brief Get button event (call from main loop)
 */
ButtonEvent_t Button_GetEvent(void)
{
    uint32_t current_time = HAL_GetTick();
    ButtonEvent_t event = button_ctx.pending_event;

    /* Clear pending event */
    button_ctx.pending_event = BUTTON_EVENT_NONE;

    /* Check for long press if button is currently pressed */
    if (button_ctx.is_pressed && !button_ctx.long_press_triggered) {
        uint32_t press_duration = current_time - button_ctx.press_start_time;
        if (press_duration >= BUTTON_LONG_PRESS_SAMPLES) {
            button_ctx.long_press_triggered = true;
            event = BUTTON_EVENT_LONG_PRESS;
            return event;
        }
    }

    /* Process DMA buffer if capture complete */
    if (button_ctx.capture_complete) {
        button_ctx.capture_complete = false;

        /* Check debounce timer */
        if ((current_time - button_ctx.last_event_time) >= BUTTON_DEBOUNCE_MS) {
            bool new_state = Button_ProcessSamples();

            /* Detect state change */
            if (new_state != button_ctx.is_pressed) {
                button_ctx.is_pressed = new_state;
                button_ctx.last_event_time = current_time;

                if (new_state) {
                    /* Button pressed */
                    button_ctx.press_start_time = current_time;
                    button_ctx.long_press_triggered = false;
                    event = BUTTON_EVENT_PRESS;
                } else {
                    /* Button released */
                    event = BUTTON_EVENT_RELEASE;
                }
            }
        }

        /* Restart sampling for next button event */
        Button_StartSampling();
    }

    return event;
}

/**
 * @brief Get current button state
 */
bool Button_IsPressed(void)
{
    return button_ctx.is_pressed;
}

/**
 * @brief Get button debug info (for diagnostics)
 * @param info_buf: buffer to store debug info string
 * @param buf_size: size of buffer
 */
void Button_GetDebugInfo(char *info_buf, uint16_t buf_size)
{
    if (info_buf && buf_size > 0) {
        snprintf(info_buf, buf_size,
                "[BTN] Idx:%d Flg:%d Pressed:%d",
                button_ctx.sample_index,
                button_ctx.capture_complete ? 1 : 0,
                button_ctx.is_pressed ? 1 : 0);
    }
}

/**
 * @brief TIM2 update interrupt handler - Sample GPIO every 1ms
 */
void Button_TIM2_IRQHandler(void)
{
    if (TIM_Interrupt_Status_Get(TIM2, TIM_INT_UPDATE) != RESET) {
        TIM_Interrupt_Status_Clear(TIM2, TIM_INT_UPDATE);

        /* Sample GPIO state if we're still collecting samples */
        if (button_ctx.sample_index < BUTTON_SAMPLE_SIZE) {
            /* Read PB11 state (1 = not pressed, 0 = pressed) */
            button_ctx.sample_buffer[button_ctx.sample_index] = (GPIOB->PID >> 11) & 0x1;
            button_ctx.sample_index++;

            /* Check if buffer is full */
            if (button_ctx.sample_index >= BUTTON_SAMPLE_SIZE) {
                button_ctx.capture_complete = true;
            }
        }
    }
}

/**
 * @brief DMA transfer complete callback (not used in software sampling mode)
 */
void Button_DMA_IRQHandler(void)
{
    /* Not used - keeping for compatibility */
}
