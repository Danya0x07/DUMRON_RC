TARGET = main
CC = sdcc
MCU = stm8s105k4

CFLAGS = -mstm8 --out-fmt-ihx

DEFINES = -DSTM8S105

INCLUDES = -I'./STM8S_StdPeriph_Driver/inc/' \
-I'./inc/'

VPATH := src STM8S_StdPeriph_Driver/src

OUTPUT_DIR = ./build

SOURCES = main.c buttons.c delay.c stm8s_gpio.c stm8s_clk.c

OBJ_FILES = $(addprefix $(OUTPUT_DIR)/, $(notdir $(SOURCES:.c=.rel)))

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
