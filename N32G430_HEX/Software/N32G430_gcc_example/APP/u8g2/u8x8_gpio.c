/*

  u8x8_gpio.c

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

*/

#include "u8x8.h"

/*================================================*/
/* call gpio and delay procedures */

/* u8x8_gpio_call: function declaration matches u8x8.h line 786 */
void u8x8_gpio_call(u8x8_t *u8x8, uint8_t msg, uint8_t arg)
{
  if ( u8x8->gpio_and_delay_cb != NULL )
  {
    u8x8->gpio_and_delay_cb(u8x8, msg, arg, NULL);
  }
}

/* Note: u8x8_gpio_Delay is defined as a macro in u8x8.h line 788, not a function */
/* #define u8x8_gpio_Delay(u8x8, msg, dly) u8x8_gpio_call((u8x8), (msg), (dly)) */
