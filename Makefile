TARGET = main
CC = sdcc
MCU = stm8s105k4

CFLAGS = -mstm8 --std-c89 --opt-code-speed

DEFINES = -DSTM8S105

INCLUDES = -I'./lib/STM8S_StdPeriph_Driver/inc/' \
 -I'./inc/'

LIBPATHS = -L'/home/danya/Projects/DUMRON_RC/lib/'

STATICLIBS = -lSTM8S_StdPeriph_Driver_optspeed.lib

SOURCES = src/main.c \
 src/buttons.c \
 src/delay.c \
 src/debug.c \
 src/joystick.c \
 src/display.c \
 src/radio.c \
 src/nokia5110lcd.c \
 src/nrf24l01p.c \
 src/itoa.c

HEADERS = inc/buttons.h \
 inc/delay.h \
 inc/debug.h \
 inc/joystick.h \
 inc/display.h \
 inc/radio.h \
 inc/nokia5110lcd.h \
 inc/nrf24l01p.h \
 inc/robot_interface.h

OUTPUT_DIR = ./build
OBJ_FILES = $(addprefix $(OUTPUT_DIR)/, $(notdir $(SOURCES:.c=.rel)))

all: $(OUTPUT_DIR) $(OUTPUT_DIR)/$(TARGET).hex

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

%.hex: %.ihx
	packihx $< > $@
	size $@

$(OUTPUT_DIR)/$(TARGET).ihx: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LIBPATHS) $(STATICLIBS) -o $@ $^

build/%.rel: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o $@ $<

clean:
	rm -Rrf ./build/*

prog: all
	stm8flash -c stlinkv2 -p $(MCU) -w build/$(TARGET).hex

uart:
	gtkterm --port /dev/ttyUSB0 --speed 9600
