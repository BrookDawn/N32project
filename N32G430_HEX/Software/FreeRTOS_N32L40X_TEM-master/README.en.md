# FreeRTOS N32L403KBQ Template Project

## Project Introduction

This project is a FreeRTOS real-time operating system template project based on the Nations Technologies N32L403KBQ microcontroller. The project provides a complete development framework, including FreeRTOS kernel, hardware abstraction layer, peripheral driver library, and sample applications, helping developers quickly get started with embedded application development based on N32L403KBQ.

### Key Features

- **Target Chip**: N32L403KBQ (ARM Cortex-M4F core)
- **RTOS Version**: FreeRTOS 202411.00
- **Development Environment**: Keil MDK-ARM
- **System Clock**: 64MHz (maximum frequency)
- **Memory Configuration**: 128KB Flash, 24KB SRAM
- **Sample Function**: LED blinking task demonstration

## Chip Specifications

### N32L403KBQ Technical Parameters

| Parameter | Specification |
|-----------|---------------|
| **Core** | ARM Cortex-M4F (32-bit) |
| **Max Frequency** | Up to 64MHz |
| **Flash** | 128KB |
| **SRAM** | 24KB |
| **Package** | LQFP32 |
| **Operating Voltage** | 1.8V - 3.6V |
| **Operating Temperature** | -40°C ~ +85°C |

### Peripheral Resources

- **GPIO**: Up to 25 I/O pins
- **Timers**: 8 timers (TIM1-TIM9)
- **Communication Interfaces**: 
  - 3 USART
  - 2 I2C
  - 2 SPI
  - 1 CAN
  - 1 USB 2.0 FS
- **Analog Peripherals**:
  - 12-bit ADC (16 channels)
  - 12-bit DAC (2 channels)
  - 2 Comparators
  - 2 Operational Amplifiers
- **Other Peripherals**:
  - RTC (Real-Time Clock)
  - WWDG/IWDG (Watchdog)
  - DMA (8 channels)
  - LCD Controller
  - Low Power Timer (LPTIM)
  - Low Power UART (LPUART)

## Software Architecture

```
FreeRTOS_N32L40X_TEM/
├── firmware/                    # Firmware library
│   ├── CMSIS/                  # ARM CMSIS standard interface
│   │   ├── core/              # Cortex-M4 core files
│   │   └── device/            # N32L40X device-related files
│   ├── n32l40x_std_periph_driver/  # Standard peripheral driver library
│   │   ├── inc/               # Header files
│   │   └── src/               # Source files
│   └── n32l40x_usbfs_driver/   # USB full-speed device driver
├── RTOS/                       # FreeRTOS real-time operating system
│   ├── inc/                   # FreeRTOS header files
│   ├── src/                   # FreeRTOS source files
│   │   └── FreeRTOSConfig.h   # FreeRTOS configuration file
│   └── port/                  # ARM Cortex-M4 porting layer
├── USER/                       # User application
│   ├── inc/                   # User header files
│   │   ├── main.h
│   │   └── n32l40x_it.h
│   └── src/                   # User source files
│       ├── main.c             # Main program
│       └── n32l40x_it.c       # Interrupt service routines
├── Objects/                    # Compilation output files
├── Listings/                   # Compilation listing files
└── *.uvprojx                  # Keil project files
```

### System Architecture Layers

1. **Hardware Abstraction Layer (HAL)**
   - CMSIS standard interface
   - N32L40X device drivers
   - Peripheral register definitions

2. **FreeRTOS Kernel Layer**
   - Task scheduler
   - Memory management (heap_4)
   - Synchronization primitives (semaphores, mutexes, queues)
   - Software timers

3. **Application Layer**
   - User tasks
   - Business logic
   - Peripheral control

## FreeRTOS Configuration

### Main Configuration Parameters

| Configuration Item | Value | Description |
|-------------------|-------|-------------|
| `configCPU_CLOCK_HZ` | 64000000 | System clock frequency 64MHz |
| `configTICK_RATE_HZ` | 1000 | System tick frequency 1000Hz (1ms) |
| `configMAX_PRIORITIES` | 5 | Maximum number of task priorities |
| `configTOTAL_HEAP_SIZE` | 8192 | FreeRTOS heap size 8KB |
| `configMINIMAL_STACK_SIZE` | 128 | Minimum task stack size |
| `configUSE_PREEMPTION` | 1 | Enable preemptive scheduling |
| `configUSE_MUTEXES` | 1 | Enable mutexes |
| `configUSE_TIMERS` | 1 | Enable software timers |

### Enabled Feature Modules

- ✅ Preemptive multitasking scheduling
- ✅ Task priority management
- ✅ Mutexes and semaphores
- ✅ Message queues
- ✅ Software timers
- ✅ Memory management (heap_4 algorithm)
- ✅ Stack overflow detection
- ✅ Runtime hook functions

## Installation Tutorial

### Environment Requirements

1. **Development Tools**
   - Keil MDK-ARM V5.06 or higher
   - ARM Compiler V5 or V6

2. **Hardware Requirements**
   - N32L403KBQ development board or minimum system board
   - ST-Link or J-Link debugger
   - USB data cable

3. **Software Packages**
   - N32L40x_DFP device support package

### Installation Steps

1. **Install Keil MDK-ARM**
   ```
   Download and install Keil MDK-ARM development environment
   Register and activate license
   ```

2. **Install Device Support Package**
   ```
   Open Pack Installer in Keil
   Search and install Nationstech::N32L40x_DFP
   ```

3. **Clone Project**
   ```bash
   git clone [project_address]
   cd FreeRTOS_N32L40X_TEM
   ```

4. **Open Project**
   ```
   Use Keil MDK-ARM to open FreeRTOS_N32L40X_TEM.uvprojx
   ```

5. **Compile Project**
   ```
   Click compile button or press F7 to compile project
   Ensure compilation without errors and warnings
   ```

## Usage Instructions

### Quick Start

1. **Connect Hardware**
   - Connect N32L403KBQ development board to PC via debugger
   - Ensure PB0 pin is connected to LED (active low)

2. **Compile and Download**
   ```
   Compile project in Keil (F7)
   Download program to target board (F8)
   ```

3. **Run Program**
   - Reset development board or click run
   - Observe LED blinking at 1Hz frequency (500ms on, 500ms off)

### Code Structure Description

#### Main Program (main.c)

```c
int main(void)
{
    /* System clock configuration */
    SystemClock_Config();
    
    /* LED GPIO configuration */
    LED_GPIO_Config();
    
    /* Create LED blinking task */
    xTaskCreate(LED_Task,                    /* Task function */
                "LED_Task",                  /* Task name */
                LED_TASK_STACK_SIZE,         /* Task stack size */
                NULL,                        /* Task parameters */
                LED_TASK_PRIORITY,           /* Task priority */
                NULL);                       /* Task handle */
    
    /* Start FreeRTOS scheduler */
    vTaskStartScheduler();
    
    while (1) { /* Should never reach here */ }
}
```

#### LED Task Implementation

```c
static void LED_Task(void *pvParameters)
{
    while (1)
    {
        LED_On();                           /* Turn on LED */
        vTaskDelay(pdMS_TO_TICKS(500));     /* Delay 500ms */
        
        LED_Off();                          /* Turn off LED */
        vTaskDelay(pdMS_TO_TICKS(500));     /* Delay 500ms */
    }
}
```

### Custom Development

#### Adding New Tasks

1. **Define Task Function**
   ```c
   static void MyTask(void *pvParameters)
   {
       while (1)
       {
           // Task logic
           vTaskDelay(pdMS_TO_TICKS(100));  // Delay 100ms
       }
   }
   ```

2. **Create Task**
   ```c
   xTaskCreate(MyTask, "MyTask", 128, NULL, 2, NULL);
   ```

#### Using Peripherals

1. **Include Header Files**
   ```c
   #include "n32l40x_usart.h"  // USART peripheral
   #include "n32l40x_i2c.h"    // I2C peripheral
   ```

2. **Initialize Peripherals**
   ```c
   // Refer to examples in firmware/n32l40x_std_periph_driver/src/
   ```

### Debugging Tips

1. **Use Breakpoint Debugging**
   - Set breakpoints at key code locations
   - Use Keil debugger for step-by-step execution

2. **View Task Status**
   ```c
   // View task list and stack usage during debugging
   vTaskList(pcWriteBuffer);
   vTaskGetRunTimeStats(pcWriteBuffer);
   ```

3. **Memory Usage Monitoring**
   ```c
   // Check remaining heap memory
   size_t freeHeap = xPortGetFreeHeapSize();
   ```

## Common Issues

### Compilation Issues

**Q: Header file not found during compilation**
A: Check if include paths are correctly configured in project settings

**Q: Insufficient memory during linking**
A: Check heap size settings in FreeRTOSConfig.h, ensure it doesn't exceed chip SRAM capacity

### Runtime Issues

**Q: Program doesn't run after download**
A: Check system clock configuration, ensure HSE/HSI configuration is correct

**Q: LED doesn't blink**
A: Check GPIO configuration and hardware connections, ensure PB0 is connected to LED

**Q: System crashes or resets**
A: Check for stack overflow, increase task stack size or enable stack overflow detection

## Technical Support

### Related Resources

- [FreeRTOS Official Documentation](https://www.freertos.org/Documentation/01-FreeRTOS-quick-start/01-Beginners-guide/02-Quick-start-guide)
- [N32L40X Series Datasheet](https://www.nationstech.com)
- [ARM Cortex-M4 Technical Reference Manual](https://developer.arm.com/documentation/100166/0001)

### Development Tools

- **Keil MDK-ARM**: Primary development environment
- **ST-Link Utility**: Program download and debugging
- **Serial Debug Assistant**: Serial communication debugging

## Version History

- **v1.0.0** (2024-12-xx)
  - Initial release
  - Based on FreeRTOS 202411.00
  - Support for N32L403KBQ chip
  - Includes LED blinking example

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Welcome to submit Issues and Pull Requests to improve this project:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Create a Pull Request

---

**Note**: This project is for learning and reference purposes only. Please test and verify thoroughly before using in commercial projects.
