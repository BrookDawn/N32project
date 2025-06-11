/**
 * @file usart1.c
 * @brief USART1 Serial Communication Implementation
 * @version v1.0.0
 * @author N32G430 Project
 */

#include "usart.h"
#include "timer.h"
#include <string.h>

/* Private Variables */
static uint8_t usart1_initialized = 0;
static char tx_buffer[USART1_TX_BUFFER_SIZE];
static char rx_buffer[USART1_RX_BUFFER_SIZE];
static volatile uint16_t rx_write_index = 0;
static volatile uint16_t rx_read_index = 0;
static volatile uint16_t rx_count = 0;

/**
 * @brief Initialize USART1 with specified baudrate
 * @param baudrate: Serial communication baudrate
 * @retval None
 */
void USART1_Init(uint32_t baudrate)
{
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* Enable clocks */
    RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOA);
    RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_USART1);

    /* Configure USART1 TX (PA9) */
    GPIO_InitStructure.Pin = USART1_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Current = GPIO_DS_4MA;
    GPIO_InitStructure.GPIO_Alternate = USART1_TX_GPIO_AF;
    GPIO_Peripheral_Initialize(USART1_TX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure USART1 RX (PA10) */
    GPIO_InitStructure.Pin = USART1_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = USART1_RX_GPIO_AF;
    GPIO_Peripheral_Initialize(USART1_RX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure USART1 */
    USART_InitStructure.BaudRate = baudrate;
    USART_InitStructure.WordLength = USART_WL_8B;
    USART_InitStructure.StopBits = USART_STPB_1;
    USART_InitStructure.Parity = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode = USART_MODE_TX | USART_MODE_RX;
    USART_Initializes(USART1, &USART_InitStructure);

    /* Configure NVIC for USART1 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);

    /* Enable USART1 receive interrupt */
    USART_Interrput_Enable(USART1, USART_INT_RXDNE);

    /* Enable USART1 */
    USART_Enable(USART1);

    /* Clear buffers */
    memset(rx_buffer, 0, sizeof(rx_buffer));
    rx_write_index = 0;
    rx_read_index = 0;
    rx_count = 0;

    usart1_initialized = 1;
}

/**
 * @brief Deinitialize USART1
 * @param None
 * @retval None
 */
void USART1_DeInit(void)
{
    USART_Disable(USART1);
    usart1_initialized = 0;
}

/**
 * @brief Send a character via USART1
 * @param ch: Character to send
 * @retval None
 */
void USART1_Send_Char(char ch)
{
    if (!usart1_initialized) return;

    /* Wait for TX register to be empty */
    while (USART_Flag_Status_Get(USART1, USART_FLAG_TXDE) == RESET);

    /* Send character */
    USART_Data_Send(USART1, (uint8_t)ch);

    /* Wait for transmission complete */
    while (USART_Flag_Status_Get(USART1, USART_FLAG_TXC) == RESET);
}

/**
 * @brief Send a string via USART1
 * @param str: String to send
 * @retval None
 */
void USART1_Send_String(const char* str)
{
    if (!str) return;

    while (*str)
    {
        USART1_Send_Char(*str++);
    }
}

/**
 * @brief Send data array via USART1
 * @param data: Data array to send
 * @param length: Length of data
 * @retval None
 */
void USART1_Send_Data(const uint8_t* data, uint16_t length)
{
    if (!data || length == 0) return;

    for (uint16_t i = 0; i < length; i++)
    {
        USART1_Send_Char((char)data[i]);
    }
}

/**
 * @brief Receive a character from USART1 (blocking)
 * @param None
 * @retval Received character
 */
char USART1_Receive_Char(void)
{
    if (!usart1_initialized) return 0;

    /* Wait for data to be received */
    while (USART_Flag_Status_Get(USART1, USART_FLAG_RXDNE) == RESET);

    /* Return received data */
    return (char)USART_Data_Receive(USART1);
}

/**
 * @brief Receive a string from USART1 with timeout
 * @param buffer: Buffer to store received string
 * @param max_length: Maximum length of buffer
 * @param timeout_ms: Timeout in milliseconds
 * @retval Number of characters received
 */
uint16_t USART1_Receive_String(char* buffer, uint16_t max_length, uint32_t timeout_ms)
{
    if (!buffer || max_length == 0) return 0;

    uint16_t count = 0;
    uint32_t start_time = Get_Tick_Count();
    uint32_t timeout_us = timeout_ms * 1000;

    while (count < (max_length - 1))
    {
        if (USART_Flag_Status_Get(USART1, USART_FLAG_RXDNE) != RESET)
        {
            char ch = (char)USART_Data_Receive(USART1);
            
            if (ch == '\r' || ch == '\n')
            {
                break; /* End of line */
            }
            
            buffer[count++] = ch;
            start_time = Get_Tick_Count(); /* Reset timeout on each character */
        }
        
        /* Check timeout */
        if ((Get_Tick_Count() - start_time) > timeout_us)
        {
            break;
        }
        
        Delay_us(100); /* Small delay to prevent busy waiting */
    }

    buffer[count] = '\0'; /* Null terminate */
    return count;
}

/**
 * @brief Printf-style formatted output via USART1
 * @param format: Format string
 * @param ...: Variable arguments
 * @retval None
 */
void USART1_Printf(const char* format, ...)
{
    va_list args;
    int length;

    va_start(args, format);
    length = vsnprintf(tx_buffer, sizeof(tx_buffer), format, args);
    va_end(args);

    if (length > 0 && length < sizeof(tx_buffer))
    {
        USART1_Send_String(tx_buffer);
    }
}

/**
 * @brief Send hexadecimal value via USART1
 * @param value: Value to send in hex
 * @retval None
 */
void USART1_Send_Hex(uint32_t value)
{
    USART1_Printf("0x%08X", value);
}

/**
 * @brief Send decimal value via USART1
 * @param value: Value to send in decimal
 * @retval None
 */
void USART1_Send_Dec(uint32_t value)
{
    USART1_Printf("%u", value);
}

/**
 * @brief Send floating point value via USART1
 * @param value: Float value to send
 * @param decimals: Number of decimal places
 * @retval None
 */
void USART1_Send_Float(float value, uint8_t decimals)
{
    char format[16];
    snprintf(format, sizeof(format), "%%.%df", decimals);
    USART1_Printf(format, value);
}

/**
 * @brief Check if USART1 TX is ready
 * @param None
 * @retval 1 if ready, 0 if not
 */
uint8_t USART1_Is_TX_Ready(void)
{
    if (!usart1_initialized) return 0;
    return (USART_Flag_Status_Get(USART1, USART_FLAG_TXDE) != RESET) ? 1 : 0;
}

/**
 * @brief Check if USART1 RX has data
 * @param None
 * @retval 1 if data available, 0 if not
 */
uint8_t USART1_Is_RX_Ready(void)
{
    if (!usart1_initialized) return 0;
    return (USART_Flag_Status_Get(USART1, USART_FLAG_RXDNE) != RESET) ? 1 : 0;
}

/**
 * @brief Check if USART1 is initialized
 * @param None
 * @retval 1 if initialized, 0 if not
 */
uint8_t USART1_Is_Initialized(void)
{
    return usart1_initialized;
}

/**
 * @brief Enable USART1 interrupts
 * @param None
 * @retval None
 */
void USART1_IT_Enable(void)
{
    USART_Interrput_Enable(USART1, USART_INT_RXDNE);
}

/**
 * @brief Disable USART1 interrupts
 * @param None
 * @retval None
 */
void USART1_IT_Disable(void)
{
    USART_Interrput_Disable(USART1, USART_INT_RXDNE);
}

/**
 * @brief Get number of characters in RX buffer
 * @param None
 * @retval Number of characters available
 */
uint16_t USART1_Get_RX_Buffer_Count(void)
{
    return rx_count;
}

/**
 * @brief Read data from RX buffer
 * @param buffer: Buffer to store data
 * @param max_length: Maximum length to read
 * @retval Number of characters read
 */
uint16_t USART1_Read_RX_Buffer(char* buffer, uint16_t max_length)
{
    uint16_t read_count = 0;

    while (rx_count > 0 && read_count < max_length)
    {
        buffer[read_count++] = rx_buffer[rx_read_index];
        rx_read_index = (rx_read_index + 1) % USART1_RX_BUFFER_SIZE;
        rx_count--;
    }

    return read_count;
}

/**
 * @brief USART1 interrupt handler
 * @param None
 * @retval None
 */
void USART1_IRQHandler(void)
{
    /* Check if USART1 received data */
    if (USART_Interrupt_Status_Get(USART1, USART_INT_RXDNE) != RESET)
    {
        char received_char = (char)USART_Data_Receive(USART1);
        
        /* Store character in circular buffer */
        if (rx_count < USART1_RX_BUFFER_SIZE)
        {
            rx_buffer[rx_write_index] = received_char;
            rx_write_index = (rx_write_index + 1) % USART1_RX_BUFFER_SIZE;
            rx_count++;
        }
        
        /* Clear the interrupt flag */
        USART_Interrupt_Status_Clear(USART1, USART_INT_RXDNE);
    }
}

/**
 * @brief Test USART1 communication
 * @param None
 * @retval None
 */
void USART1_Test_Communication(void)
{
    USART1_Send_String("\r\n=== USART1 Communication Test ===\r\n");
    USART1_Send_String("Hello from N32G430!\r\n");
    
    USART1_Printf("System Clock: %u Hz\r\n", Timer_Get_System_Clock_Hz());
    USART1_Printf("Current Tick: %u us\r\n", Get_Tick_Count());
    
    USART1_Send_String("=== Test Complete ===\r\n\r\n");
}

/**
 * @brief Test USART1 echo functionality
 * @param None
 * @retval None
 */
void USART1_Test_Echo(void)
{
    char buffer[64];
    
    USART1_Send_String("=== Echo Test Mode ===\r\n");
    USART1_Send_String("Type something and press Enter:\r\n");
    
    while (1)
    {
        uint16_t received = USART1_Receive_String(buffer, sizeof(buffer), 1000);
        
        if (received > 0)
        {
            USART1_Printf("Echo: %s\r\n", buffer);
            
            if (strcmp(buffer, "exit") == 0)
            {
                USART1_Send_String("Exiting echo mode...\r\n");
                break;
            }
        }
    }
}

/**
 * @brief Send system information via USART1
 * @param None
 * @retval None
 */
void USART1_Test_Send_Info(void)
{
    RCC_ClocksType clocks;
    
    USART1_Send_String("\r\n=== N32G430 System Information ===\r\n");
    
    /* Get clock information */
    RCC_Clocks_Frequencies_Value_Get(&clocks);
    
    USART1_Printf("System Clock: %u Hz\r\n", clocks.SysclkFreq);
    USART1_Printf("AHB Clock: %u Hz\r\n", clocks.HclkFreq);
    USART1_Printf("APB1 Clock: %u Hz\r\n", clocks.Pclk1Freq);
    USART1_Printf("APB2 Clock: %u Hz\r\n", clocks.Pclk2Freq);
    
    USART1_Printf("USART1 Baudrate: %u\r\n", USART1_BAUDRATE);
    USART1_Printf("Timer Initialized: %s\r\n", Timer_Is_Initialized() ? "Yes" : "No");
    USART1_Printf("USART1 Initialized: %s\r\n", USART1_Is_Initialized() ? "Yes" : "No");
    
    uint32_t tick_count = Get_Tick_Count();
    USART1_Printf("System Uptime: %u us\r\n", tick_count);
    USART1_Printf("System Uptime: %u ms\r\n", tick_count / 1000);
    
    USART1_Send_String("=== Information Complete ===\r\n\r\n");
}

/* Printf redirection to USART1 */
int __io_putchar(int ch)
{
    if (usart1_initialized)
    {
        USART1_Send_Char((char)ch);
    }
    return ch;
}

int __io_getchar(void)
{
    if (usart1_initialized)
    {
        return (int)USART1_Receive_Char();
    }
    return 0;
} 