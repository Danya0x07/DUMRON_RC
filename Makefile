TARGET = main
CC = sdcc
MCU = stm8s105k4

CFLAGS = -mstm8 --out-fmt-ihx --opt-code-speed

DEFINES = -DSTM8S105

INCLUDES = -I'./lib/STM8S_StdPeriph_Driver/inc/' \
-I'./inc/'

VPATH := src lib/STM8S_StdPeriph_Driver/src

OUTPUT_DIR = ./build

SOURCES_FROM_SPL = stm8s_gpio.c stm8s_clk.c stm8s_uart2.c stm8s_adc1.c

ALL_SOURCES = $(SOURCES_FROM_SPL) \
 main.c buttons.c delay.c debug.c joystick.c msgprotocol.c
 

OBJ_FILES = $(addprefix $(OUTPUT_DIR)/, $(notdir $(ALL_SOURCES:.c=.rel)))

all: $(OUTPUT_DIR) $(OUTPUT_DIR)/$(TARGET).hex

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

%.hex: %.ihx
	packihx $< > $@
	size $@

$(OUTPUT_DIR)/$(TARGET).ihx: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

build/%.rel: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c -o $@ $<

clean:
	rm -Rrf ./build/*

prog: all
	stm8flash -c stlinkv2 -p $(MCU) -w build/$(TARGET).hex
