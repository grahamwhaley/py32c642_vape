
PROJECT = pressure_sensors

TOP=../..
HALFILEPATH = $(TOP)/Libraries/PY32F002B_HAL_Driver/Src

# Potentially clean up the libfile list - we don't have half these periphs on the 642, and don't
# use most of the rest in this example

SRCFILES = \
	py32f002b_it.c \
	main.c \
	charlie.c \
	digit.c \
	py32f002b_hal_msp.c

HALSRCFILES = \
	$(HALFILEPATH)/py32f002b_hal_crc.c \
	$(HALFILEPATH)/py32f002b_hal_usart.c \
	$(HALFILEPATH)/py32f002b_hal_i2c.c \
	$(HALFILEPATH)/py32f002b_hal_adc.c \
	$(HALFILEPATH)/py32f002b_hal_exti.c \
	$(HALFILEPATH)/py32f002b_hal_uart.c \
	$(HALFILEPATH)/py32f002b_hal_pwr.c \
	$(HALFILEPATH)/py32f002b_hal.c \
	$(HALFILEPATH)/py32f002b_hal_flash.c \
	$(HALFILEPATH)/py32f002b_hal_iwdg.c \
	$(HALFILEPATH)/py32f002b_hal_spi.c \
	$(HALFILEPATH)/py32f002b_hal_comp.c \
	$(HALFILEPATH)/py32f002b_hal_lptim.c \
	$(HALFILEPATH)/py32f002b_hal_tim.c \
	$(HALFILEPATH)/py32f002b_hal_tim_ex.c \
	$(HALFILEPATH)/py32f002b_hal_rcc_ex.c \
	$(HALFILEPATH)/py32f002b_hal_rcc.c \
	$(HALFILEPATH)/py32f002b_hal_cortex.c \
	$(HALFILEPATH)/py32f002b_hal_gpio.c \

BSPFILEPATH = $(TOP)/Libraries/PY32F002B_HAL_BSP/Src
BSPSRCFILES = \
	$(BSPFILEPATH)/py32f002b_bsp_printf.c

CMSISFILEPATH = $(TOP)/Libraries/CMSIS/Device/PY32F0xx/Source
CMSISSRCFILES = $(CMSISFILEPATH)/system_py32f002b.c

STARTUPFILEPATH = $(TOP)/Libraries/CMSIS/Device/PY32F0xx/Source/gcc
STARTUPSRCFILES = $(STARTUPFILEPATH)/startup_py32f002b.s

LDFILE = $(TOP)/Libraries/LDScripts/py32f002bx5.ld

INCLUDE := . \
			$(TOP)/Libraries/CMSIS/Core/Include \
			$(TOP)/Libraries/CMSIS/Device/PY32F0xx/Include \
			$(TOP)/Libraries/PY32F002B_HAL_Driver/Inc \
			$(TOP)/Libraries/PY32F002B_HAL_BSP/Inc

INCLUDE_CMD := $(addprefix -I, $(INCLUDE))

ARM_TOOLCHAIN ?= /home/gwhaley/.arduino15/packages/STMicroelectronics/tools/xpack-arm-none-eabi-gcc/12.2.1-1.2/bin
TOOL_PREFIX = arm-none-eabi-

CC = $(ARM_TOOLCHAIN)/$(TOOL_PREFIX)gcc
OBJCOPY = $(ARM_TOOLCHAIN)/$(TOOL_PREFIX)objcopy

CFLAGS += -D PY32F002Bx5 -mthumb -mcpu=cortex-m0plus -std=c99 -Og -Wall -ffunction-sections -fdata-sections

# note the no-warn-rwx is gcc >=12 specific or similar. See IOsetting repo rules.mk
LDFLAGS		= -mthumb -mcpu=cortex-m0plus -specs=nano.specs -specs=nosys.specs -static -lc -lm \
			-Wl,-Map=$(BUILDDIR)/$(PROJECT).map \
			-Wl,--gc-sections \
			-Wl,--print-memory-usage \
			-Wl,--no-warn-rwx-segments

BUILDDIR = build
OBJS = $(SRCFILES:%.c=$(BUILDDIR)/%.o)
OBJS += $(HALSRCFILES:$(HALFILEPATH)/%.c=$(BUILDDIR)/%.o)
OBJS += $(BSPSRCFILES:$(BSPFILEPATH)/%.c=$(BUILDDIR)/%.o)
OBJS += $(CMSISSRCFILES:$(CMSISFILEPATH)/%.c=$(BUILDDIR)/%.o)
OBJS += $(STARTUPSRCFILES:$(STARTUPFILEPATH)/%.s=$(BUILDDIR)/%.o)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	echo Make $@
	$(CC) $(CFLAGS) $(INCLUDE_CMD) -o $@ -c $<

$(BUILDDIR)/%.o: $(HALFILEPATH)/%.c
	@mkdir -p $(dir $@)
	echo Make $@
	$(CC) $(CFLAGS) $(INCLUDE_CMD) -o $@ -c $<

$(BUILDDIR)/%.o: $(BSPFILEPATH)/%.c
	@mkdir -p $(dir $@)
	echo Make $@
	$(CC) $(CFLAGS) $(INCLUDE_CMD) -o $@ -c $<

$(BUILDDIR)/%.o: $(CMSISFILEPATH)/%.c
	@mkdir -p $(dir $@)
	echo Make $@
	$(CC) $(CFLAGS) $(INCLUDE_CMD) -o $@ -c $<

$(BUILDDIR)/%.o: $(STARTUPFILEPATH)/%.s
	@mkdir -p $(dir $@)
	echo Make $@
	$(CC) $(CFLAGS) $(INCLUDE_CMD) -o $@ -c $<


.PHONY: all clean

all:	$(BUILDDIR)/$(PROJECT).bin
	echo Finished

clean:
	rm -rf $(BUILDDIR)/*

$(BUILDDIR)/$(PROJECT).bin: $(BUILDDIR)/$(PROJECT).elf
	echo Make $@
	$(OBJCOPY) -I elf32-littlearm -O binary  $< $@

# Link object files to elf
$(BUILDDIR)/$(PROJECT).elf: $(OBJS) $(LDFILE)
	$(CC) $(LDFLAGS) -T$(LDFILE) $(OBJS) -o $@

