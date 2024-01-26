LIBNAME			= opencm3_stm32f4
OPENCM3_DIR    = deps/libopencm3
DEFS				+= -DSTM32F4
DEFS		+= -I$(OPENCM3_DIR)/include

FP_FLAGS		?= -mfloat-abi=hard -mfpu=fpv4-sp-d16
ARCH_FLAGS	= -mthumb -mcpu=cortex-m4 $(FP_FLAGS)

CXX		:= arm-none-eabi-g++
LD		:= arm-none-eabi-gcc

OPT		:= -Os
DEBUG		:= -ggdb3

TGT_CXXFLAGS	+= $(OPT) $(DEBUG) $(ARCH_FLAGS) -Wextra -Wshadow -Wredundant-decls  -Weffc++
TGT_CXXFLAGS	+= -fno-common -ffunction-sections -fdata-sections

TGT_CPPFLAGS	+= -MD -Wall -Wundef $(DEFS)

LDSCRIPT = src/stm32f401re.ld
LDLIBS		+= -l$(LIBNAME) -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
LDFLAGS		+= -L$(OPENCM3_DIR)/lib
TGT_LDFLAGS		+= --static -nostartfiles -T$(LDSCRIPT) $(ARCH_FLAGS) $(DEBUG)
TGT_LDFLAGS		+= -Wl,-Map=$(*).map -Wl,--cref -Wl,--gc-sections

# APP = blinky
APP = blinky_systick

all: $(APP).elf

$(APP).elf: $(APP).o
	$(LD) $(TGT_LDFLAGS) $(LDFLAGS) src/apps/$(APP).o $(LDLIBS) -o $(APP).elf

$(APP).o:
	$(CXX) $(TGT_CXXFLAGS) $(TGT_CPPFLAGS) -o src/apps/$(APP).o -c src/apps/$(APP).cpp

clean:
	rm src/apps/$(APP).o
	rm src/apps/$(APP).d
	rm $(APP).elf
	rm .map
