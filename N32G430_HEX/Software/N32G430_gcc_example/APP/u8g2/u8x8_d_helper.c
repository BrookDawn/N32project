/*

  u8x8_d_helper.c

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

*/

#include "u8x8.h"

/*================================================*/
/* display helper: init */

void u8x8_d_helper_display_init(u8x8_t *u8x8)
{
  /* setup display */
  u8x8_cad_StartTransfer(u8x8);
}

/*================================================*/
/* display helper: setup memory */

void u8x8_d_helper_display_setup_memory(u8x8_t *u8x8, const u8x8_display_info_t *display_info)
{
  u8x8_SetupMemory(u8x8);
}
