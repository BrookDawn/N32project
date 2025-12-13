/**
 * @file button_test_demo.h
 * @brief Button test demo with UART logging
 */

#ifndef BUTTON_TEST_DEMO_H
#define BUTTON_TEST_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize button test demo
 */
void ButtonTest_Init(void);

/**
 * @brief Main loop for button test demo
 */
void ButtonTest_Main(void);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_TEST_DEMO_H */
