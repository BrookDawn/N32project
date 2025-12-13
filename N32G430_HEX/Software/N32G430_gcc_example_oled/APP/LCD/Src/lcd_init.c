/**
 * @file lcd_init.c
 * @brief LCD ST7735S initialization - using SPI1 hardware interface
 */

#include "lcd_init.h"
#include "main.h"

/**
 * @brief LCD GPIO initialization (handled by MX_SPI1_Init)
 */
void LCD_GPIO_Init(void)
{
    /* GPIO initialization is done in MX_SPI1_Init() */
}

/**
 * @brief Write a byte via SPI1 hardware
 * @param dat: data byte to send
 * @note CS should be controlled by caller, not here
 */
void LCD_Writ_Bus(u8 dat)
{
    HAL_SPI_Transmit(&hspi1, &dat, 1, 100);
}

/**
 * @brief Write 8-bit data
 * @param dat: data byte
 */
void LCD_WR_DATA8(u8 dat)
{
    LCD_CS_Clr();
    LCD_DC_Set();
    LCD_Writ_Bus(dat);
    LCD_CS_Set();
}

/**
 * @brief Write 16-bit data (high byte first)
 * @param dat: 16-bit data
 */
void LCD_WR_DATA(u16 dat)
{
    LCD_CS_Clr();
    LCD_DC_Set();
    LCD_Writ_Bus(dat >> 8);
    LCD_Writ_Bus(dat);
    LCD_CS_Set();
}

/**
 * @brief Write data buffer via DMA (blocking with yield)
 * @param data: pointer to data buffer
 * @param len: data length in bytes
 */
void LCD_WR_DATA_DMA(u8 *data, u16 len)
{
    extern DMA_HandleTypeDef hdma_spi1_tx;

    LCD_CS_Clr();
    LCD_DC_Set();
    HAL_SPI_Transmit_DMA(&hspi1, data, len);

    /* Wait for DMA transfer to complete, but yield to allow interrupts */
    uint32_t timeout = HAL_GetTick() + 100;  /* 100ms timeout */
    while (hdma_spi1_tx.State != HAL_DMA_STATE_READY) {
        /* Check timeout to prevent infinite loop */
        if (HAL_GetTick() > timeout) {
            break;
        }
        /* Small delay to allow other interrupts (like TIM2) to run */
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

    LCD_CS_Set();
}

/**
 * @brief Write command byte
 * @param dat: command byte
 */
void LCD_WR_REG(u8 dat)
{
    LCD_CS_Clr();
    LCD_DC_Clr();
    LCD_Writ_Bus(dat);
    LCD_DC_Set();
    LCD_CS_Set();
}

/**
 * @brief Set display address window
 * @param x1,y1: start coordinates
 * @param x2,y2: end coordinates
 */
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    if(USE_HORIZONTAL == 0)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1 + 2);
        LCD_WR_DATA(x2 + 2);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1 + 1);
        LCD_WR_DATA(y2 + 1);
        LCD_WR_REG(0x2c);
    }
    else if(USE_HORIZONTAL == 1)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1 + 2);
        LCD_WR_DATA(x2 + 2);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1 + 1);
        LCD_WR_DATA(y2 + 1);
        LCD_WR_REG(0x2c);
    }
    else if(USE_HORIZONTAL == 2)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1 + 1);
        LCD_WR_DATA(x2 + 1);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1 + 2);
        LCD_WR_DATA(y2 + 2);
        LCD_WR_REG(0x2c);
    }
    else
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1 + 1);
        LCD_WR_DATA(x2 + 1);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1 + 2);
        LCD_WR_DATA(y2 + 2);
        LCD_WR_REG(0x2c);
    }
}

/**
 * @brief Initialize LCD (ST7735S)
 */
void LCD_Init(void)
{
    /* Hardware reset */
    LCD_RES_Clr();
    HAL_Delay(100);
    LCD_RES_Set();
    HAL_Delay(100);

    /* Turn on backlight */
    LCD_BLK_Set();
    HAL_Delay(100);

    /* ST7735S initialization sequence */
    LCD_WR_REG(0x11);  /* Sleep out */
    HAL_Delay(120);

    /* Frame Rate */
    LCD_WR_REG(0xB1);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3C);
    LCD_WR_DATA8(0x3C);
    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3C);
    LCD_WR_DATA8(0x3C);
    LCD_WR_REG(0xB3);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3C);
    LCD_WR_DATA8(0x3C);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3C);
    LCD_WR_DATA8(0x3C);

    /* Dot inversion */
    LCD_WR_REG(0xB4);
    LCD_WR_DATA8(0x03);

    /* Power Sequence */
    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x04);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA8(0xC0);
    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x00);
    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x8D);
    LCD_WR_DATA8(0x2A);
    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x8D);
    LCD_WR_DATA8(0xEE);

    /* VCOM */
    LCD_WR_REG(0xC5);
    LCD_WR_DATA8(0x1A);

    /* MX, MY, RGB mode */
    LCD_WR_REG(0x36);
    if(USE_HORIZONTAL == 0)
        LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL == 1)
        LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL == 2)
        LCD_WR_DATA8(0x70);
    else
        LCD_WR_DATA8(0xA0);

    /* Gamma Sequence */
    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x22);
    LCD_WR_DATA8(0x07);
    LCD_WR_DATA8(0x0A);
    LCD_WR_DATA8(0x2E);
    LCD_WR_DATA8(0x30);
    LCD_WR_DATA8(0x25);
    LCD_WR_DATA8(0x2A);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x26);
    LCD_WR_DATA8(0x2E);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x01);
    LCD_WR_DATA8(0x03);
    LCD_WR_DATA8(0x13);
    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x16);
    LCD_WR_DATA8(0x06);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x2D);
    LCD_WR_DATA8(0x26);
    LCD_WR_DATA8(0x23);
    LCD_WR_DATA8(0x27);
    LCD_WR_DATA8(0x27);
    LCD_WR_DATA8(0x25);
    LCD_WR_DATA8(0x2D);
    LCD_WR_DATA8(0x3B);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x01);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x13);

    /* 65k color mode */
    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    /* Display on */
    LCD_WR_REG(0x29);
}
