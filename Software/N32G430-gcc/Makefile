######################################
# target
######################################
TARGET = n32g430c8l7_project

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization levels: -Og (debug), -O1 (size), -O2 (speed), -Os (size)
OPT = -Og

# Enable parallel compilation
MAKEFLAGS += --no-print-directory
.PHONY: all clean fast

# Build type selection
ifeq ($(MAKECMDGOALS),fast)
OPT = -O1
CFLAGS_EXTRA = -pipe
endif

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
core/src/main.c \
core/src/timer.c \
core/src/gpio.c \
core/src/usart.c \
core/src/syscalls.c \
core/src/N32_assert.c \
CMSIS/device/system_n32g430.c \
n32g430_std_periph_driver/src/n32g430_rcc.c \
n32g430_std_periph_driver/src/n32g430_gpio.c \
n32g430_std_periph_driver/src/n32g430_exti.c \
n32g430_std_periph_driver/src/n32g430_usart.c \
n32g430_std_periph_driver/src/n32g430_tim.c \
n32g430_std_periph_driver/src/n32g430_dma.c \
n32g430_std_periph_driver/src/n32g430_adc.c \
n32g430_std_periph_driver/src/n32g430_spi.c \
n32g430_std_periph_driver/src/n32g430_i2c.c \
n32g430_std_periph_driver/src/n32g430_flash.c \
n32g430_std_periph_driver/src/n32g430_pwr.c \
n32g430_std_periph_driver/src/misc.c

# ASM sources
ASM_SOURCES =  \
CMSIS/device/startup/startup_n32g430_gcc.s

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=soft

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DN32G430C8L7 \
-DUSE_STDPERIPH_DRIVER

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES =  \
-Icore/src \
-Icore/inc \
-ICMSIS/device \
-ICMSIS/core \
-In32g430_std_periph_driver/inc

# compile gcc flags with optimization
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections \
         -ffast-math -fno-common $(CFLAGS_EXTRA)

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = n32g430c8l7_flash.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) \
          -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections \
          -Wl,--no-warn-rwx-segments

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# fast build target
fast: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
	@echo "Fast build completed!"

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
