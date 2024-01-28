# This Makefile was adapted from the URL below. The values and configuration
# should hopefully be the same; I just tried to make the variable names easier
# to grok and the build logic easier to follow. Every flag is also commented
# because I want to understand every aspect of the compiler/linker commands.
#
# https://github.com/lowbyteproductions/bare-metal-series/blob/main/app/Makefile

OPENCM3_LIBNAME = opencm3_stm32f4
OPENCM3_DIR = deps/libopencm3
OPENCM3_DEVICE_NAME = STM32F4

ARCH_FLAGS = -mthumb  # Use the Arm Thumb instruction set
ARCH_FLAGS += -mcpu=cortex-m4  # Optimize for the Cortex-M4 processor arch
ARCH_FLAGS += -mfloat-abi=hard  # Assume that the processor has a hardware floating point unit (FPU)
ARCH_FLAGS += -mfpu=fpv4-sp-d16  # FPU is Vector Floating Point arch v4, Single-Precision

OPTIMIZATION_FLAG = -Os  # Optimize to reduce code size
DEBUG_FLAG = -ggdb3  # Provide the highest level of detail for GDB debugging info

COMPILER = arm-none-eabi-g++
COMPILER_FLAGS += $(OPTIMIZATION_FLAG) $(DEBUG_FLAG) $(ARCH_FLAGS)
COMPILER_FLAGS += -Wextra  # Enable extra warnings
COMPILER_FLAGS += -Wshadow  # Detect shadowed variables
COMPILER_FLAGS += -Wredundant-decls  # Detect redundant declarations
COMPILER_FLAGS += -Weffc++  # Enforce guidelines from Effective C++ by Scott Meyers
COMPILER_FLAGS += -fno-common  # Disable the "common model" behavior for globals
COMPILER_FLAGS += -ffunction-sections
COMPILER_FLAGS += -fdata-sections

PREPROCESSOR_FLAGS = -MD
PREPROCESSOR_FLAGS += -Wall
PREPROCESSOR_FLAGS += -Wundef
PREPROCESSOR_FLAGS += -D$(OPENCM3_DEVICE_NAME) -I$(OPENCM3_DIR)/include

LINKER = arm-none-eabi-gcc
LINKER_SCRIPT = src/stm32f401re.ld
LINKER_LIBS = -l$(OPENCM3_LIBNAME)
LINKER_LIBS += -Wl,--start-group
LINKER_LIBS += -lc
LINKER_LIBS += -lgcc
LINKER_LIBS += -lnosys
LINKER_LIBS += -Wl,--end-group
LINKER_FLAGS += -L$(OPENCM3_DIR)/lib
LINKER_FLAGS += --static -nostartfiles -T$(LINKER_SCRIPT) $(ARCH_FLAGS) $(DEBUG_FLAG)
LINKER_FLAGS += -Wl,-Map=$(APP).map -Wl,--cref -Wl,--gc-sections

# APP = blinky
APP = blinky_systick

all: $(APP).elf

$(APP).elf: $(APP).o
	$(LINKER) $(LINKER_FLAGS) src/apps/$(APP).o $(LINKER_LIBS) -o $(APP).elf

$(APP).o:
	$(COMPILER) $(COMPILER_FLAGS) $(PREPROCESSOR_FLAGS) -o src/apps/$(APP).o -c src/apps/$(APP).cpp

flash:
	$(shell pwd)/flash.sh $(APP)

clean:
	rm src/apps/$(APP).o
	rm src/apps/$(APP).d
	rm $(APP).elf
	rm $(APP).map
