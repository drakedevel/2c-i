PREFIX = /usr/local/arm-2012.03/bin/arm-none-eabi-
CC = $(PREFIX)gcc
BUILDID=`git describe --dirty`

INCLUDES = -Iinc -Iarch/stm32f4
CFLAGS += -fno-common -mcpu=cortex-m3 -mthumb -O3 -g -fno-unroll-loops -nostdlib -nostdinc
CFLAGS += -Wall -Werror -fdata-sections -fshort-enums -Wno-unused -Wno-address
LDFLAGS = -Tarch/stm32f4/stm32f4.lds -mcpu=cortex-m3 -mthumb -fwhole-program -Wl,-Map=2ci.map -nostdlib -nostdinc

CFLAGS += $(INCLUDES)

SRCS = arch/stm32f4/startup_stm32f4.S main.c

OBJS1 = $(SRCS:.c=.o)
OBJS = $(OBJS1:.S=.o)

all: 2ci.hex size

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

2ci.hex: 2ci.elf
	$(PREFIX)objcopy -O ihex -R .eeprom -R .fuse -R .lock $< $@

2ci.elf: $(OBJS)
	$(CC) $(CFLAGS) -c ../common/lib/build_id.c -o build_id.o -DBUILDID=\"$(BUILDID)\"
	$(CC) $(LDFLAGS) $(OBJS) build_id.o -o 2ci.elf

.PHONY: clean flash bl size term

clean:
	rm -f 2ci.elf 2ci.hex 2ci.map 2ci.bin 2ci.binhex 2ci-full.hex build-id.o pc $(OBJS)

flash: 2ci-full.hex
	~/lpc21isp/lpc21isp -debug3 2ci-full.hex /dev/ttyUSB0 230400 14746

bl:
	sudo ../tools/f0ad/f0ad
	sleep 3
	sudo dfu-util -D 2ci.bin -R

size:
	$(PREFIX)size -A 2ci.elf
	$(PREFIX)nm -anS --size-sort -t d 2ci.elf | grep ^2 | tail -20
	@echo
	@echo "->" $(BUILDID)

term:
	../tools/miniterm/miniterm.py -b 230400 -p /dev/ttyUSB0 --rts=0
