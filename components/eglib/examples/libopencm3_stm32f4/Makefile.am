DEVICE = stm32f411ceu6
MAIN_MEMORY_BASE=0x08000000
STM_DFU_USB_VID=0x0483
STM_DFU_USB_PID=0xdf11
OOCD_INTERFACE = stlink-v2
OOCD_TARGET= stm32f4x

all: $(BINFILES)

##
## configure.ac dep
##

EXTRA_DIST = build-aux/libopencm3.mk

##
## distcheck
##

DISTCHECK_CONFIGURE_FLAGS =
DISTCHECK_CONFIGURE_FLAGS += --host=arm-none-eabi
DISTCHECK_CONFIGURE_FLAGS += --with-libopencm3=$(OPENCM3_DIR)
DISTCHECK_CONFIGURE_FLAGS += --with-eglib-libdir=$(EGLIB_LIBDIR)
DISTCHECK_CONFIGURE_FLAGS += --with-eglib-includedir=$(EGLIB_INCLUDEDIR)

##
## opencm3 variables & rules
##

include $(OPENCM3_DIR)/mk/genlink-config.mk
# automake hides replaced variables, so we must redefine it here
LDSCRIPT = generated.$(DEVICE).ld
Q = $(AM_V_GEN)
include $(OPENCM3_DIR)/mk/genlink-rules.mk
CLEANFILES = $(LDSCRIPT)

##
## .bin rules
##

objcopy_verbose = $(objcopy_verbose_@AM_V@)
objcopy_verbose_ = $(objcopy_verbose_@AM_DEFAULT_V@)
objcopy_verbose_0 = @echo "  OBJCOPY " $@;

SUFFIXES = .elf.bin .bin

.elf.bin:
	$(objcopy_verbose)$(OBJCOPY) -O binary $< $@

##
## Common parameters
##

CFLAGS_COMMON =
CFLAGS_COMMON += $(CFLAGS_OPENCM3)
CFLAGS_COMMON += -fno-common
CFLAGS_COMMON += -ffunction-sections -fdata-sections

LDFLAGS_COMMON =
LDFLAGS_COMMON += -T$(LDSCRIPT)
LDFLAGS_COMMON += $(LDFLAGS_OPENCM3)
LDFLAGS_COMMON += -nostartfiles
LDFLAGS_COMMON += $(CFLAGS_OPENCM3)
LDFLAGS_COMMON += -specs=nano.specs
LDFLAGS_COMMON += -Wl,--gc-sections

##
## Examples
##

noinst_PROGRAMS =
BINFILES =

noinst_PROGRAMS += sh1106-i2c.elf
sh1106_i2c_elf_CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_WARNINGS)
sh1106_i2c_elf_LDFLAGS = $(LDFLAGS_EGLIB) $(LDFLAGS_COMMON)
sh1106_i2c_elf_LDADD = $(LIBS_EGLIB) $(LIBS_OPENCM3)
sh1106_i2c_elf_CPPFLAGS = $(CPPFLAGS_EGLIB)
sh1106_i2c_elf_SOURCES = sh1106-i2c.c
EXTRA_sh1106_i2c_elf_DEPENDENCIES = $(LDSCRIPT)
BINFILES += sh1106-i2c.bin

noinst_PROGRAMS += sh1106-four_wire_spi.elf
sh1106_four_wire_spi_elf_CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_WARNINGS)
sh1106_four_wire_spi_elf_LDFLAGS = $(LDFLAGS_EGLIB) $(LDFLAGS_COMMON)
sh1106_four_wire_spi_elf_LDADD = $(LIBS_EGLIB) $(LIBS_OPENCM3)
sh1106_four_wire_spi_elf_CPPFLAGS = $(CPPFLAGS_EGLIB)
sh1106_four_wire_spi_elf_SOURCES = sh1106-four_wire_spi.c
EXTRA_sh1106_four_wire_spi_elf_DEPENDENCIES = $(LDSCRIPT)
BINFILES += sh1106-four_wire_spi.bin

noinst_PROGRAMS += ssd1331-four_wire_spi.elf
ssd1331_four_wire_spi_elf_CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_WARNINGS)
ssd1331_four_wire_spi_elf_LDFLAGS = $(LDFLAGS_EGLIB) $(LDFLAGS_COMMON)
ssd1331_four_wire_spi_elf_LDADD = $(LIBS_EGLIB) $(LIBS_OPENCM3)
ssd1331_four_wire_spi_elf_CPPFLAGS = $(CPPFLAGS_EGLIB)
ssd1331_four_wire_spi_elf_SOURCES = ssd1331-four_wire_spi.c
EXTRA_ssd1331_four_wire_spi_elf_DEPENDENCIES = $(LDSCRIPT)
BINFILES += ssd1331-four_wire_spi.bin

noinst_PROGRAMS += ssd1675a-four_wire_spi.elf
ssd1675a_four_wire_spi_elf_CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_WARNINGS)
ssd1675a_four_wire_spi_elf_LDFLAGS = $(LDFLAGS_EGLIB) $(LDFLAGS_COMMON)
ssd1675a_four_wire_spi_elf_LDADD = $(LIBS_EGLIB) $(LIBS_OPENCM3)
ssd1675a_four_wire_spi_elf_CPPFLAGS = $(CPPFLAGS_EGLIB)
ssd1675a_four_wire_spi_elf_SOURCES = ssd1675a-four_wire_spi.c
EXTRA_ssd1675a_four_wire_spi_elf_DEPENDENCIES = $(LDSCRIPT)
BINFILES += ssd1675a-four_wire_spi.bin

noinst_PROGRAMS += st7789-four_wire_spi.elf
st7789_four_wire_spi_elf_CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_WARNINGS)
st7789_four_wire_spi_elf_LDFLAGS = $(LDFLAGS_EGLIB) $(LDFLAGS_COMMON)
st7789_four_wire_spi_elf_LDADD = $(LIBS_EGLIB) $(LIBS_OPENCM3)
st7789_four_wire_spi_elf_CPPFLAGS = $(CPPFLAGS_EGLIB)
st7789_four_wire_spi_elf_SOURCES = st7789-four_wire_spi.c
EXTRA_st7789_four_wire_spi_elf_DEPENDENCIES = $(LDSCRIPT)
BINFILES += st7789-four_wire_spi.bin

noinst_PROGRAMS += st7789-parallel_8_bit_8080.elf
st7789_parallel_8_bit_8080_elf_CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_WARNINGS)
st7789_parallel_8_bit_8080_elf_LDFLAGS = $(LDFLAGS_EGLIB) $(LDFLAGS_COMMON)
st7789_parallel_8_bit_8080_elf_LDADD = $(LIBS_EGLIB) $(LIBS_OPENCM3)
st7789_parallel_8_bit_8080_elf_CPPFLAGS = $(CPPFLAGS_EGLIB)
st7789_parallel_8_bit_8080_elf_SOURCES = st7789-parallel_8_bit_8080.c
EXTRA_st7789_parallel_8_bit_8080_elf_DEPENDENCIES = $(LDSCRIPT)
BINFILES += st7789-parallel_8_bit_8080.bin

CLEANFILES += $(BINFILES)

##
## Common
##

EXAMPLE_CHECK = @if test "x$(EXAMPLE)" = x ; then echo "Please supply an example, eg: \`$(MAKE) $(AM_MAKEFLAGS) $@ EXAMPLE=ssd1331-four_wire_spi'" ; exit 1  ; fi

##
## dfu
##

dfu_util_verbose = $(dfu_util_verbose_@AM_V@)
dfu_util_verbose_ = $(dfu_util_verbose_@AM_DEFAULT_V@)
dfu_util_verbose_0 = @echo "  DFU_UTIL " $@;

.PHONY: dfu
dfu: $(BINFILES)
	$(EXAMPLE_CHECK)
	$(dfu_util_verbose)dfu-util --alt 0 --device $(STM_DFU_USB_VID):$(STM_DFU_USB_PID) --dfuse-address $(MAIN_MEMORY_BASE) --download $(EXAMPLE).bin --reset

##
## openocd
##

openocd_verbose = $(openocd_verbose_@AM_V@)
openocd_verbose_ = $(openocd_verbose_@AM_DEFAULT_V@)
openocd_verbose_0 = @echo "  OPENOCD " $@;

.PHONY: openocd
openocd:
	$(openocd_verbose)openocd --file interface/$(OOCD_INTERFACE).cfg --file target/$(OOCD_TARGET).cfg

gdb_verbose = $(gdb_verbose_@AM_V@)
gdb_verbose_ = $(gdb_verbose_@AM_DEFAULT_V@)
gdb_verbose_0 = @echo "  GDB " $@;

.PHONY: openocd_program
openocd_program: $(BINFILES)
	$(EXAMPLE_CHECK)
	$(openocd_verbose)openocd --file interface/$(OOCD_INTERFACE).cfg --file target/$(OOCD_TARGET).cfg --command "program $(EXAMPLE).bin verify reset exit $(MAIN_MEMORY_BASE)"

##
## gdb
##

EXTRA_DIST += gdb.init

.PHONY: gdb
if HAS_GDB
gdb: $(noinst_PROGRAMS)
	$(EXAMPLE_CHECK)
	$(gdb_verbose)$(GDB) --command=$(top_srcdir)/gdb.init $(EXAMPLE).elf
endif