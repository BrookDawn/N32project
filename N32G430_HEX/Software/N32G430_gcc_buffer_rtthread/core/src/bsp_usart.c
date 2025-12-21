/**
 * @file bsp_usart.c
 * @author brookdawn
 * @brief 串口初始化
 * @version 0.1
 * @date 2025-07-06
 * @copyright Copyright (c) 2025
 */

#include "n32g430.h"
#include "bsp_usart.h"
#include "bsp_delay.h"
#include "usart_interface.h"

/* 接收缓冲区 */
static uint8_t usart1_rx_buffer[USART1_RX_BUFFER_SIZE];
static uint16_t rx_head = 0;
static uint16_t rx_tail = 0;
static uint16_t rx_count = 0;

/**
 *\*\name   USART1_Init.
 *\*\fun    Initialize USART1.
 *\*\param  none.
 *\*\return none.
**/
void USART1_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* 使能GPIOA和USART1时钟 */
    RCC_AHB_Peripheral_Clock_Enable(USART1_GPIO_CLK);
    RCC_APB2_Peripheral_Clock_Enable(USART1_CLK);

    /* 配置USART1 TX引脚 (PA9) */
    GPIO_InitStructure.Pin              = USART1_TX_PIN;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_InitStructure.GPIO_Alternate   = GPIO_AF5_USART1;
    GPIO_Peripheral_Initialize(USART1_GPIO_PORT, &GPIO_InitStructure);

    /* 配置USART1 RX引脚 (PA10) */
    GPIO_InitStructure.Pin              = USART1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current     = GPIO_DS_8MA;
    GPIO_InitStructure.GPIO_Alternate   = GPIO_AF5_USART1;
    GPIO_Peripheral_Initialize(USART1_GPIO_PORT, &GPIO_InitStructure);

    /* USART1配置 */
    USART_InitStructure.BaudRate        = USART1_BAUDRATE;
    USART_InitStructure.WordLength      = USART1_WORDLENGTH;
    USART_InitStructure.StopBits        = USART1_STOPBITS;
    USART_InitStructure.Parity          = USART1_PARITY;
    USART_InitStructure.HardwareFlowControl = USART1_HARDWAREFLOWCTL;
    USART_InitStructure.Mode            = USART1_MODE;
    USART_Initializes(USART1, &USART_InitStructure);

    /* 使能USART1接收中断 */
    USART_Interrput_Enable(USART1, USART_INT_RXDNE);

    /* 配置USART1中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);

    /* 使能USART1 */
    USART_Enable(USART1);

    /* 初始化接收缓冲区 */
    rx_head = 0;
    rx_tail = 0;
    rx_count = 0;
}

/**
 *\*\name   USART1_SendByte.
 *\*\fun    Send one byte via USART1.
 *\*\param  data: byte to send.
 *\*\return none.
**/
void USART1_SendByte(uint8_t data)
{
    /* 等待发送数据寄存器为空 */
    while(USART_Flag_Status_Get(USART1, USART_FLAG_TXDE) == RESET);

    /* 发送数据 */
    USART_Data_Send(USART1, data);
}

/**
 *\*\name   USART1_SendString.
 *\*\fun    Send string via USART1.
 *\*\param  str: string to send.
 *\*\return none.
**/
void USART1_SendString(const char* str)
{
    while(*str)
    {
        USART1_SendByte(*str++);
    }
}

/**
 *\*\name   USART1_SendArray.
 *\*\fun    Send array via USART1.
 *\*\param  data: array to send.
 *\*\param  length: length of array.
 *\*\return none.
**/
void USART1_SendArray(uint8_t* data, uint16_t length)
{
    uint16_t i;
    for(i = 0; i < length; i++)
    {
        USART1_SendByte(data[i]);
    }
}

/**
 *\*\name   USART1_ReceiveByte.
 *\*\fun    Receive one byte from USART1.
 *\*\param  none.
 *\*\return received byte.
**/
uint8_t USART1_ReceiveByte(void)
{
    /* 等待接收数据 */
    while(USART_Flag_Status_Get(USART1, USART_FLAG_RXDNE) == RESET);

    /* 返回接收到的数据 */
    return (uint8_t)USART_Data_Receive(USART1);
}

/**
 *\*\name   USART1_ReceiveString.
 *\*\fun    Receive string from USART1 buffer.
 *\*\param  buffer: buffer to store received string.
 *\*\param  max_length: maximum length of buffer.
 *\*\return actual length of received string.
**/
uint16_t USART1_ReceiveString(char* buffer, uint16_t max_length)
{
    uint16_t length = 0;

    /* 从缓冲区读取数据 */
    while(rx_count > 0 && length < max_length - 1)
    {
        buffer[length] = usart1_rx_buffer[rx_tail];
        rx_tail = (rx_tail + 1) % USART1_RX_BUFFER_SIZE;
        rx_count--;
        length++;
    }

    buffer[length] = '\0';
    return length;
}

/**
 *\*\name   USART1_EnableInterrupt.
 *\*\fun    Enable USART1 interrupt.
 *\*\param  none.
 *\*\return none.
**/
void USART1_EnableInterrupt(void)
{
    USART_Interrput_Enable(USART1, USART_INT_RXDNE);
}

/**
 *\*\name   USART1_DisableInterrupt.
 *\*\fun    Disable USART1 interrupt.
 *\*\param  none.
 *\*\return none.
**/
void USART1_DisableInterrupt(void)
{
    USART_Interrput_Disable(USART1, USART_INT_RXDNE);
}

/**
 *\*\name   USART1_IRQHandler.
 *\*\fun    USART1 interrupt handler.
 *\*\param  none.
 *\*\return none.
**/
void USART1_IRQHandler(void)
{
    if(USART_Interrupt_Status_Get(USART1, USART_INT_RXDNE) != RESET)
    {
        /* 清除中断标志 */
        USART_Interrupt_Status_Clear(USART1, USART_INT_RXDNE);

        /* 检查缓冲区是否已满 */
        if(rx_count < USART1_RX_BUFFER_SIZE)
        {
            /* 读取接收到的数据并存储到缓冲区 */
            usart1_rx_buffer[rx_head] = (uint8_t)USART_Data_Receive(USART1);
            rx_head = (rx_head + 1) % USART1_RX_BUFFER_SIZE;
            rx_count++;
        }
        else
        {
            /* 缓冲区已满，丢弃数据 */
            USART_Data_Receive(USART1);
        }
    }
}


/*=========================== [Printf Redirection] ======================*/

/**
 *\*\name   usart_bsp_init.
 *\*\fun    BSP weak function override for usart_interface initialization.
 *\*\param  config: USART configuration (ignored, using USART1_Init).
 *\*\return USART_STATUS_OK.
**/
usart_status_t usart_bsp_init(const usart_config_t *config)
{
    (void)config;  /* Ignore config, USART1 already initialized in board.c */
    /* USART1 已经在 board.c 的 rt_hw_board_init() 中初始化 */
    /* 直接返回成功，允许 interface 层正常工作 */
    return USART_STATUS_OK;
}

/**
 *\*\name   usart_bsp_putchar.
 *\*\fun    BSP weak function override for usart_interface.
 *\*\param  ch: character to send.
 *\*\return sent character.
**/
int usart_bsp_putchar(int ch)
{
    USART1_SendByte((uint8_t)ch);
    return ch;
}

#ifdef __ARMCC_VERSION      /* Keil compiler */
    #pragma import(__use_no_semihosting_swi)
    void _sys_exit(int return_code)
    {
        (void)return_code;
    }

    struct __FILE
    {
        int handle;
    };

    FILE __stdout;

    int fputc(int ch, FILE *f)
    {
        (void)f;
        return usart_interface_putchar(ch);
    }

#elif defined (__GNUC__)    /* GCC compiler */

    int __io_putchar(int ch)
    {
        return usart_interface_putchar(ch);
    }

    int _write(int file, char *ptr, int len)
    {
        (void)file;
        int i = 0;
        for (i = 0; i < len; i++) {
            __io_putchar(*ptr++);
        }
        return len;
    }

#elif defined (__ICCARM__)  /* IAR compiler */

    size_t __write(int handle, const unsigned char *buf, size_t size)
    {
        (void)handle;
        size_t i = 0;
        for (i = 0; i < size; i++) {
            usart_interface_putchar(buf[i]);
        }
        return size;
    }

#endif

