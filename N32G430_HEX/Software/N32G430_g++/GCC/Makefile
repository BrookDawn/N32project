# ------------------------------------------------
#
# @file Makefile (based on gcc)
# @author Dewin
# @version v1.0.0
#
# ChangeLog :
#   2021-11-20
# ------------------------------------------------

######################################
# target
######################################
TARGET = output

######################################
# building variables
######################################
# debug build
ifeq ($(release), y)
DEBUG = 0
else
DEBUG = 1
endif
# optimization
OPT = -Os

#######################################
# Build path
#######################################
BUILD_DIR = build

######################################
# chip platform info
######################################
TARGET_PLATFORM := n32g430
DEFS += -DN32G430
DEFS += -DUSE_STDPERIPH_DRIVER

######################################
# Algo libs
######################################
USELIB = 0

######################################
#TOOLS CHAIN
######################################
CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++

######################################
# C sources
######################################
C_DIRS = ../core/src
C_DIRS += ../APP/Src
C_DIRS += ../RTT/Src
C_DIRS += ../firmware/CMSIS/device
C_DIRS += ../firmware/n32g430_std_periph_driver/src
SRC_OBJS_DIRS = $(foreach DIR, $(C_DIRS), $(wildcard $(DIR)/*.c))
C_SOURCES = $(SRC_OBJS_DIRS)

######################################
# C++ sources
######################################
CPP_OBJS_DIRS = $(foreach DIR, $(C_DIRS), $(wildcard $(DIR)/*.cpp))
CPP_SOURCES = $(CPP_OBJS_DIRS)

######################################
# ASM sources
######################################
ASM_SOURCES = ../firmware/CMSIS/device/startup/startup_$(TARGET_PLATFORM)_gcc.s

######################################
# C includes
######################################
C_INCS = ../core/inc
C_INCS += ../APP/Ins
C_INCS += ../RTT/Ins
C_INCS += ../firmware/CMSIS/core
C_INCS += ../firmware/CMSIS/device
C_INCS += ../firmware/n32g430_std_periph_driver/inc

INCS_OBJS_DIR = $(foreach DIR2, $(C_INCS), $(wildcard $(DIR2)/*.h))
INCS_OBJS_PATH = $(sort $(dir $(INCS_OBJS_DIR)))
C_INCLUDES = $(addprefix -I,$(INCS_OBJS_PATH))

######################################
# Lib files
######################################
ifeq ($(USELIB), 1)
SRC_LIB_DIR += ../firmware/$(TARGET_PLATFORM)_algo_lib/src
INC_LIB_DIR += ../firmware/$(TARGET_PLATFORM)_algo_lib/inc
LIB_SOURCES = $(foreach DIR3, $(SRC_LIB_DIR), $(wildcard $(DIR3)/*.lib))
LIB_SOURCES_L = $(addprefix -L,$(LIB_SOURCES))
C_LIBS = $(LIB_SOURCES_L)
LIB_INCS = $(foreach DIR4, $(INC_LIB_DIR), $(wildcard $(DIR4)/*.h))
LIB_INCS_PATH = $(sort $(dir $(LIB_INCS)))
LIB_INCS_PATH_I = $(addprefix -I,$(LIB_INCS_PATH))
C_INCLUDES += $(LIB_INCS_PATH_I)
endif

######################################
# Compile & Link flags
######################################
# cpu
CPU = -mcpu=cortex-m4
# fpu
FPU = -mfpu=fpv4-sp-d16
# float-abi
FLOAT-ABI = -mfloat-abi=hard
# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

#CFLAGS
CFLAGS += $(MCU) -Wall -Wextra
CFLAGS += $(OPT)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wno-unused-parameter -Wno-missing-field-initializers
#DEBUG
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif
# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
#CXXFLAGS
CXXFLAGS += $(MCU) -Wall -Wextra
CXXFLAGS += $(OPT)
CXXFLAGS += -ffunction-sections -fdata-sections
CXXFLAGS += -Wno-unused-parameter -Wno-missing-field-initializers
ifeq ($(DEBUG), 1)
CXXFLAGS += -g -gdwarf-2
endif
# For embedded: avoid exceptions/RTTI/unwind to reduce size and deps
CXXFLAGS += -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -fno-unwind-tables -fno-asynchronous-unwind-tables
CXXFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
#LFLAGS
LFLAGS += $(MCU)
LFLAGS += -Wl,--gc-sections
LFLAGS += --specs=nosys.specs
LFLAGS += -Xlinker -Map=$(BUILD_DIR)/$(TARGET).map
# link script
LDSCRIPT = ../firmware/CMSIS/device/$(TARGET_PLATFORM)_flash.ld

######################################
# Objects
######################################
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of C++ objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEFS) $(C_INCLUDES) $(C_LIBS) -c -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEFS) $(C_INCLUDES) -c -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) $(LFLAGS) -T$(LDSCRIPT) -o $@
	$(CROSS_COMPILE)size $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(CROSS_COMPILE)objcopy -O binary -S $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(CROSS_COMPILE)objcopy -O ihex -S $< $@

$(BUILD_DIR):
	@-mkdir $@

SHELL=cmd.exe
#######################################
# clean up
#######################################
clean:
	@echo "Cleaning build directory..."
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

#######################################
# analysis targets
#######################################
size: $(BUILD_DIR)/$(TARGET).elf
	@echo "=== Size Analysis ==="
	$(CROSS_COMPILE)size $<
	@echo "=== Memory Usage ==="
	$(CROSS_COMPILE)nm --size-sort --radix=d $< | head -20

objdump: $(BUILD_DIR)/$(TARGET).elf
	$(CROSS_COMPILE)objdump -h -S $< > $(BUILD_DIR)/$(TARGET).lss

#######################################
# info targets
#######################################
info:
	@echo "=== Build Configuration ==="
	@echo "Target: $(TARGET)"
	@echo "Platform: $(TARGET_PLATFORM)"
	@echo "Debug: $(DEBUG)"
	@echo "Optimization: $(OPT)"
	@echo "Cross Compiler: $(CROSS_COMPILE)"
	@echo "C Sources: $(words $(C_SOURCES)) files"
	@echo "C++ Sources: $(words $(CPP_SOURCES)) files"

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

#######################################
# download .hex/.bin by jlink
#######################################
#Your JLink installation directory
PATH_WINPC = 'C:\Program Files\SEGGER\JLink_V840\'
#PATH_LINUX = /opt/SEGGER/JLink_V640b/JLinkExe
JK_DPATH = $(PATH_WINPC)
#Jlink script store directory
JKS_DIR = ../jlink
#Chip type
CHIP_TYPE = N32G430C8
download:
	@$(JK_DPATH)JLink.exe -device $(CHIP_TYPE) -if SWD -speed 4000 -autoconnect 1 -CommanderScript $(JKS_DIR)/flash.jlink
	@echo "Download Completed!"

debug:
	@$(JK_DPATH)JLinkGDBServer.exe -select USB -device $(CHIP_TYPE) -if SWD -speed auto -noir -LocalhostOnly

# *** EOF ***
