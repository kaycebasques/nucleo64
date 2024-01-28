LIBNAME = opencm3_stm32f4
OPENCM3_DIR = deps/libopencm3
DEFS += -DSTM32F4
DEFS += -I$(OPENCM3_DIR)/include

FLOATING_POINT_FLAGS ?= -mfloat-abi=hard -mfpu=fpv4-sp-d16
ARCH_FLAGS = -mthumb -mcpu=cortex-m4 $(FLOATING_POINT_FLAGS)

OPT := -Os
DEBUG := -ggdb3

CXX := arm-none-eabi-g++
CXX_FLAGS += $(OPT) $(DEBUG) $(ARCH_FLAGS) -Wextra -Wshadow -Wredundant-decls  -Weffc++
CXX_FLAGS += -fno-common -ffunction-sections -fdata-sections

PREPROCESSOR_FLAGS += -MD -Wall -Wundef $(DEFS)

LINKER := arm-none-eabi-gcc
LINKER_SCRIPT = src/stm32f401re.ld
LINKER_LIBS += -l$(LIBNAME) -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
LINKER_FLAGS += -L$(OPENCM3_DIR)/lib
LINKER_FLAGS += --static -nostartfiles -T$(LINKER_SCRIPT) $(ARCH_FLAGS) $(DEBUG)
LINKER_FLAGS += -Wl,-Map=$(APP).map -Wl,--cref -Wl,--gc-sections

# APP = blinky
APP = blinky_systick

all: $(APP).elf

$(APP).elf: $(APP).o
	$(LINKER) $(LINKER_FLAGS) src/apps/$(APP).o $(LINKER_LIBS) -o $(APP).elf

$(APP).o:
	$(CXX) $(CXX_FLAGS) $(PREPROCESSOR_FLAGS) -o src/apps/$(APP).o -c src/apps/$(APP).cpp

flash:
	$(shell pwd)/flash.sh $(APP)

clean:
	rm src/apps/$(APP).o
	rm src/apps/$(APP).d
	rm $(APP).elf
	rm $(APP).map
