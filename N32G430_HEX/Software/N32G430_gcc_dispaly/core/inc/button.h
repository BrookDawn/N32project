/**
 * @file button.h
 * @brief Button input capture with DMA debouncing
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "n32g430.h"
#include "hal_compat.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Button debounce configuration */
#define BUTTON_SAMPLE_SIZE          30      /* Number of samples for short press (30ms @ 1kHz) */
#define BUTTON_PRESS_THRESHOLD      21      /* Threshold for button press (30 * 0.7 = 21) */
#define BUTTON_LONG_PRESS_SAMPLES   70      /* Number of samples for long press (70ms @ 1kHz) */
#define BUTTON_DEBOUNCE_MS          50      /* Minimum time between button events */

/* Button events */
typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_PRESS,         /* Short press detected */
    BUTTON_EVENT_RELEASE,       /* Button released */
    BUTTON_EVENT_LONG_PRESS     /* Long press detected (70ms) */
} ButtonEvent_t;

/* Button state */
typedef struct {
    uint16_t sample_buffer[BUTTON_SAMPLE_SIZE]; /* Buffer for sampling GPIO */
    volatile bool capture_complete;             /* Sample capture complete flag */
    uint32_t last_event_time;                   /* Last button event timestamp */
    uint32_t press_start_time;                  /* Button press start time (for long press detection) */
    bool is_pressed;                            /* Current button state */
    bool long_press_triggered;                  /* Long press already triggered */
    ButtonEvent_t pending_event;                /* Pending event */
    uint8_t sample_index;                       /* Current sample index */
} ButtonContext_t;

/* Public API */

/**
 * @brief Initialize button input capture with DMA
 */
void Button_Init(void);

/**
 * @brief Start button sampling
 */
void Button_StartSampling(void);

/**
 * @brief Stop button sampling
 */
void Button_StopSampling(void);

/**
 * @brief Get button event (call from main loop)
 * @return Button event
 */
ButtonEvent_t Button_GetEvent(void);

/**
 * @brief Get current button state
 * @return true if pressed, false otherwise
 */
bool Button_IsPressed(void);

/**
 * @brief Get button debug info (for diagnostics)
 * @param info_buf: buffer to store debug info string
 * @param buf_size: size of buffer
 */
void Button_GetDebugInfo(char *info_buf, uint16_t buf_size);

/**
 * @brief TIM2 IRQ handler (called from interrupt)
 */
void Button_TIM2_IRQHandler(void);

/**
 * @brief DMA IRQ handler (called from interrupt)
 */
void Button_DMA_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H */
