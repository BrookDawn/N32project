//
// Created by Dawn on 25-6-10.
//
#include "gpio.h"

/**
 * @brief  Configure the GPIO Pins.
 * @param  None
 * @retval None
 */
void LED_GPIO_Config(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIOC clock */
    RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOC);

    /* Configure PC13 as output push-pull for LED */
    GPIO_InitStructure.Pin              = GPIO_PIN_13;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_4MA;
    GPIO_InitStructure.GPIO_Alternate   = 0;
    GPIO_Peripheral_Initialize(GPIOC, &GPIO_InitStructure);
}
