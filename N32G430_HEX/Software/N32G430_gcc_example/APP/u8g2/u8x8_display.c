/*

  u8x8_display.c

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

*/

#include "u8x8.h"

/*================================================*/
/* display init */

void u8x8_InitDisplay(u8x8_t *u8x8)
{
  u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_INIT, 0, NULL);
}

/*================================================*/
/* power save */

void u8x8_SetPowerSave(u8x8_t *u8x8, uint8_t is_enable)
{
  if ( is_enable == 0 )
    u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_SET_POWER_SAVE, 0, NULL);
  else
    u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_SET_POWER_SAVE, 1, NULL);
}

/*================================================*/
/* set flip mode */

void u8x8_SetFlipMode(u8x8_t *u8x8, uint8_t mode)
{
  if ( mode == 0 )
  {
    u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_SET_FLIP_MODE, 0, NULL);
  }
  else
  {
    u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_SET_FLIP_MODE, 1, NULL);
  }
}

/*================================================*/
/* set contrast */

void u8x8_SetContrast(u8x8_t *u8x8, uint8_t value)
{
  u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_SET_CONTRAST, value, NULL);
}

/*================================================*/
/* draw tile */

uint8_t u8x8_DrawTile(u8x8_t *u8x8, uint8_t x, uint8_t y, uint8_t cnt, uint8_t *tile_ptr)
{
  u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_DRAW_TILE, x, (void *)(uint32_t)y);
  while( cnt > 0 )
  {
    u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_SEND, 8, (void *)tile_ptr);
    tile_ptr += 8;
    cnt--;
  }
  u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_END_TRANSFER, 0, NULL);
  return 1;
}

/*================================================*/
/* setup memory */

void u8x8_SetupMemory(u8x8_t *u8x8)
{
  u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_SETUP_MEMORY, 0, NULL);
}

/*================================================*/
/* refresh display */

void u8x8_RefreshDisplay(u8x8_t *u8x8)
{
  u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_REFRESH, 0, NULL);
}

/*================================================*/
/* clear display */

void u8x8_ClearDisplay(u8x8_t *u8x8)
{
  u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_DRAW_TILE, 0, (void *)0);
  u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_START_TRANSFER, 0, NULL);

  /* clear the display with zeros */
  uint8_t i, j;
  uint8_t zero[8] = {0,0,0,0,0,0,0,0};

  for( i = 0; i < u8x8->display_info->tile_height; i++ )
  {
    for( j = 0; j < u8x8->display_info->tile_width; j++ )
    {
      u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_SEND, 8, (void *)zero);
    }
  }

  u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_END_TRANSFER, 0, NULL);
  u8x8_RefreshDisplay(u8x8);
}

/*================================================*/
/* fill display */

void u8x8_FillDisplay(u8x8_t *u8x8)
{
  u8x8->display_cb(u8x8, U8X8_MSG_DISPLAY_DRAW_TILE, 0, (void *)0);
  u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_START_TRANSFER, 0, NULL);

  /* fill the display with 0xff */
  uint8_t i, j;
  uint8_t fill[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

  for( i = 0; i < u8x8->display_info->tile_height; i++ )
  {
    for( j = 0; j < u8x8->display_info->tile_width; j++ )
    {
      u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_SEND, 8, (void *)fill);
    }
  }

  u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_END_TRANSFER, 0, NULL);
  u8x8_RefreshDisplay(u8x8);
}
