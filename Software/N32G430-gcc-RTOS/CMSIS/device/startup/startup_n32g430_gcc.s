/**
 ******************************************************************************
 * @file      startup_n32g430_gcc.s
 * @author    Nations
 * @version   v1.0.0
 * @copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
 * @brief     N32G430 devices vector table for GCC toolchain.
 *            This module performs:
 *                - Set the initial SP
 *                - Set the initial PC == Reset_Handler,
 *                - Set the vector table entries with the exceptions ISR address
 *                - Configure the clock system and the external SRAM mounted on
 *                  STM3210E-EVAL board to be used as data memory (optional,
 *                  to be enabled by user)
 *                - Branches to main in the C library (which eventually
 *                  calls main()).
 *            After Reset the Cortex-M4 processor is in Thread mode,
 *            priority is Privileged, and the Stack is set to Main.
 ******************************************************************************
 */

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:

/* Copy the data segment initializers from flash to SRAM */
  movs  r1, #0
  b  LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit
  ldr  r2, =_sbss
  b  LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss

/* Call the clock system intitialization function.*/
  bl  System_Initializes
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
  bl  main
  bx  lr
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None
 * @retval None
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
*******************************************************************************/
   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler

  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  vPortSVCHandler         /* SVC Handler - Changed for FreeRTOS */
  .word  DebugMon_Handler
  .word  0
  .word  xPortPendSVHandler      /* PendSV Handler - Changed for FreeRTOS */
  .word  xPortSysTickHandler     /* SysTick Handler - Changed for FreeRTOS */

  /* External Interrupts */
  .word     WWDG_IRQHandler               /* Window WatchDog              */
  .word     PVD_IRQHandler                /* PVD through EXTI Line detection */
  .word     RTC_TAMPER_STAMP_IRQHandler   /* Tamper and TimeStamps through the EXTI line */
  .word     RTC_WKUP_IRQHandler           /* RTC Wakeup through the EXTI line */
  .word     FLASH_IRQHandler              /* FLASH                        */
  .word     RCC_IRQHandler                /* RCC                          */
  .word     EXTI0_IRQHandler              /* EXTI Line0                   */
  .word     EXTI1_IRQHandler              /* EXTI Line1                   */
  .word     EXTI2_IRQHandler              /* EXTI Line2                   */
  .word     EXTI3_IRQHandler              /* EXTI Line3                   */
  .word     EXTI4_IRQHandler              /* EXTI Line4                   */
  .word     DMA_Channel1_IRQHandler       /* DMA Channel 1                */
  .word     DMA_Channel2_IRQHandler       /* DMA Channel 2                */
  .word     DMA_Channel3_IRQHandler       /* DMA Channel 3                */
  .word     DMA_Channel4_IRQHandler       /* DMA Channel 4                */
  .word     DMA_Channel5_IRQHandler       /* DMA Channel 5                */
  .word     DMA_Channel6_IRQHandler       /* DMA Channel 6                */
  .word     DMA_Channel7_IRQHandler       /* DMA Channel 7                */
  .word     DMA_Channel8_IRQHandler       /* DMA Channel 8                */
  .word     ADC_IRQHandler                /* ADC                          */
  .word     MMU_IRQHandler                /* MMU                          */
  .word     COMP_1_2_3_IRQHandler         /* COMP1, COMP2 and COMP3       */
  .word     EXTI9_5_IRQHandler            /* External Line[9:5]s          */
  .word     TIM1_BRK_IRQHandler           /* TIM1 Break                   */
  .word     TIM1_UP_IRQHandler            /* TIM1 Update                  */
  .word     TIM1_TRG_COM_IRQHandler       /* TIM1 Trigger and Commutation */
  .word     TIM1_CC_IRQHandler            /* TIM1 Capture Compare         */
  .word     TIM2_IRQHandler               /* TIM2                         */
  .word     TIM3_IRQHandler               /* TIM3                         */
  .word     TIM4_IRQHandler               /* TIM4                         */
  .word     I2C1_EV_IRQHandler            /* I2C1 Event                   */
  .word     I2C1_ER_IRQHandler            /* I2C1 Error                   */
  .word     I2C2_EV_IRQHandler            /* I2C2 Event                   */
  .word     I2C2_ER_IRQHandler            /* I2C2 Error                   */
  .word     SPI1_IRQHandler               /* SPI1                         */
  .word     SPI2_IRQHandler               /* SPI2                         */
  .word     USART1_IRQHandler             /* USART1                       */
  .word     USART2_IRQHandler             /* USART2                       */
  .word     UART3_IRQHandler              /* UART3                        */
  .word     EXTI15_10_IRQHandler          /* External Line[15:10]s        */
  .word     RTCAlarm_IRQHandler           /* RTC Alarm (A and B) through EXTI Line */
  .word     LPTIM_WKUP_IRQHandler         /* LPTIM_WKUP                   */
  .word     TIM8_BRK_IRQHandler           /* TIM8 Break                   */
  .word     TIM8_UP_IRQHandler            /* TIM8 Update                  */
  .word     TIM8_TRG_COM_IRQHandler       /* TIM8 Trigger and Commutation */
  .word     TIM8_CC_IRQHandler            /* TIM8 Capture Compare         */
  .word     UART4_IRQHandler              /* UART4                        */
  .word     TIM5_IRQHandler               /* TIM5                         */
  .word     TIM6_IRQHandler               /* TIM6                         */
  .word     CAN_TX_IRQHandler             /* CAN TX                       */
  .word     CAN_RX0_IRQHandler            /* CAN RX0                      */
  .word     CAN_RX1_IRQHandler            /* CAN RX1                      */
  .word     CAN_SCE_IRQHandler            /* CAN SCE                      */

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/
   .weak      NMI_Handler
   .thumb_set NMI_Handler,Default_Handler

   .weak      HardFault_Handler
   .thumb_set HardFault_Handler,Default_Handler

   .weak      MemManage_Handler
   .thumb_set MemManage_Handler,Default_Handler

   .weak      BusFault_Handler
   .thumb_set BusFault_Handler,Default_Handler

   .weak      UsageFault_Handler
   .thumb_set UsageFault_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   /* Remove the weak definitions for FreeRTOS handlers */
   /* .weak      SVC_Handler */
   /* .thumb_set SVC_Handler,Default_Handler */

   /* .weak      PendSV_Handler */
   /* .thumb_set PendSV_Handler,Default_Handler */

   /* .weak      SysTick_Handler */
   /* .thumb_set SysTick_Handler,Default_Handler */

   .weak      WWDG_IRQHandler
   .thumb_set WWDG_IRQHandler,Default_Handler

   .weak      PVD_IRQHandler
   .thumb_set PVD_IRQHandler,Default_Handler

   .weak      RTC_TAMPER_STAMP_IRQHandler
   .thumb_set RTC_TAMPER_STAMP_IRQHandler,Default_Handler

   .weak      RTC_WKUP_IRQHandler
   .thumb_set RTC_WKUP_IRQHandler,Default_Handler

   .weak      FLASH_IRQHandler
   .thumb_set FLASH_IRQHandler,Default_Handler

   .weak      RCC_IRQHandler
   .thumb_set RCC_IRQHandler,Default_Handler

   .weak      EXTI0_IRQHandler
   .thumb_set EXTI0_IRQHandler,Default_Handler

   .weak      EXTI1_IRQHandler
   .thumb_set EXTI1_IRQHandler,Default_Handler

   .weak      EXTI2_IRQHandler
   .thumb_set EXTI2_IRQHandler,Default_Handler

   .weak      EXTI3_IRQHandler
   .thumb_set EXTI3_IRQHandler,Default_Handler

   .weak      EXTI4_IRQHandler
   .thumb_set EXTI4_IRQHandler,Default_Handler

   .weak      DMA_Channel1_IRQHandler
   .thumb_set DMA_Channel1_IRQHandler,Default_Handler

   .weak      DMA_Channel2_IRQHandler
   .thumb_set DMA_Channel2_IRQHandler,Default_Handler

   .weak      DMA_Channel3_IRQHandler
   .thumb_set DMA_Channel3_IRQHandler,Default_Handler

   .weak      DMA_Channel4_IRQHandler
   .thumb_set DMA_Channel4_IRQHandler,Default_Handler

   .weak      DMA_Channel5_IRQHandler
   .thumb_set DMA_Channel5_IRQHandler,Default_Handler

   .weak      DMA_Channel6_IRQHandler
   .thumb_set DMA_Channel6_IRQHandler,Default_Handler

   .weak      DMA_Channel7_IRQHandler
   .thumb_set DMA_Channel7_IRQHandler,Default_Handler

   .weak      DMA_Channel8_IRQHandler
   .thumb_set DMA_Channel8_IRQHandler,Default_Handler

   .weak      ADC_IRQHandler
   .thumb_set ADC_IRQHandler,Default_Handler

   .weak      MMU_IRQHandler
   .thumb_set MMU_IRQHandler,Default_Handler

   .weak      COMP_1_2_3_IRQHandler
   .thumb_set COMP_1_2_3_IRQHandler,Default_Handler

   .weak      EXTI9_5_IRQHandler
   .thumb_set EXTI9_5_IRQHandler,Default_Handler

   .weak      TIM1_BRK_IRQHandler
   .thumb_set TIM1_BRK_IRQHandler,Default_Handler

   .weak      TIM1_UP_IRQHandler
   .thumb_set TIM1_UP_IRQHandler,Default_Handler

   .weak      TIM1_TRG_COM_IRQHandler
   .thumb_set TIM1_TRG_COM_IRQHandler,Default_Handler

   .weak      TIM1_CC_IRQHandler
   .thumb_set TIM1_CC_IRQHandler,Default_Handler

   .weak      TIM2_IRQHandler
   .thumb_set TIM2_IRQHandler,Default_Handler

   .weak      TIM3_IRQHandler
   .thumb_set TIM3_IRQHandler,Default_Handler

   .weak      TIM4_IRQHandler
   .thumb_set TIM4_IRQHandler,Default_Handler

   .weak      I2C1_EV_IRQHandler
   .thumb_set I2C1_EV_IRQHandler,Default_Handler

   .weak      I2C1_ER_IRQHandler
   .thumb_set I2C1_ER_IRQHandler,Default_Handler

   .weak      I2C2_EV_IRQHandler
   .thumb_set I2C2_EV_IRQHandler,Default_Handler

   .weak      I2C2_ER_IRQHandler
   .thumb_set I2C2_ER_IRQHandler,Default_Handler

   .weak      SPI1_IRQHandler
   .thumb_set SPI1_IRQHandler,Default_Handler

   .weak      SPI2_IRQHandler
   .thumb_set SPI2_IRQHandler,Default_Handler

   .weak      USART1_IRQHandler
   .thumb_set USART1_IRQHandler,Default_Handler

   .weak      USART2_IRQHandler
   .thumb_set USART2_IRQHandler,Default_Handler

   .weak      UART3_IRQHandler
   .thumb_set UART3_IRQHandler,Default_Handler

   .weak      EXTI15_10_IRQHandler
   .thumb_set EXTI15_10_IRQHandler,Default_Handler

   .weak      RTCAlarm_IRQHandler
   .thumb_set RTCAlarm_IRQHandler,Default_Handler

   .weak      LPTIM_WKUP_IRQHandler
   .thumb_set LPTIM_WKUP_IRQHandler,Default_Handler

   .weak      TIM8_BRK_IRQHandler
   .thumb_set TIM8_BRK_IRQHandler,Default_Handler

   .weak      TIM8_UP_IRQHandler
   .thumb_set TIM8_UP_IRQHandler,Default_Handler

   .weak      TIM8_TRG_COM_IRQHandler
   .thumb_set TIM8_TRG_COM_IRQHandler,Default_Handler

   .weak      TIM8_CC_IRQHandler
   .thumb_set TIM8_CC_IRQHandler,Default_Handler

   .weak      UART4_IRQHandler
   .thumb_set UART4_IRQHandler,Default_Handler

   .weak      TIM5_IRQHandler
   .thumb_set TIM5_IRQHandler,Default_Handler

   .weak      TIM6_IRQHandler
   .thumb_set TIM6_IRQHandler,Default_Handler

   .weak      CAN_TX_IRQHandler
   .thumb_set CAN_TX_IRQHandler,Default_Handler

   .weak      CAN_RX0_IRQHandler
   .thumb_set CAN_RX0_IRQHandler,Default_Handler

   .weak      CAN_RX1_IRQHandler
   .thumb_set CAN_RX1_IRQHandler,Default_Handler

   .weak      CAN_SCE_IRQHandler
   .thumb_set CAN_SCE_IRQHandler,Default_Handler
