PREFIX = /usr/local/arm-2012.03/bin/arm-none-eabi-
CC = $(PREFIX)gcc
BUILDID=`git describe --dirty`

INCLUDES = -I. -Iinc -Iarch/stm32f4
CFLAGS += -fno-common -mcpu=cortex-m3 -mthumb -O3 -g -fno-unroll-loops -nostdlib -nostdinc -Dassert_param=
CFLAGS += -Wall -fdata-sections -fshort-enums -Wno-unused -Wno-address -fno-strict-aliasing -funsigned-char
LDFLAGS = -Tarch/stm32f4/stm32f4.lds -mcpu=cortex-m3 -mthumb -fwhole-program -Wl,-Map=2ci.map -nostdlib -nostdinc

INCLUDES += -I../ext/CMSIS/Device/ST/STM32F4xx/Include/
INCLUDES += -I../ext/CMSIS/Include
INCLUDES += -I../ext/minilib/inc
INCLUDES += -Iperiph/inc

SRCS += usb_bsp.c usb_core.c usb_dcd.c usb_dcd_int.c
SRCS += usbd_core.c usbd_ioreq.c usbd_req.c usbd_audio_core.c usbd_audio_out_if.c usbd_desc.c usbd_usr.c

SRCS += stm324xg_usb_audio_codec.c stm324xg_eval_ioe.c stm324xg_eval.c stm32fxxx_it.c

SRCS += periph/src/stm32f4xx_dma.c periph/src/stm32f4xx_spi.c periph/src/stm32f4xx_i2c.c periph/src/stm32f4xx_rcc.c periph/src/stm32f4xx_gpio.c \
	periph/src/stm32f4xx_exti.c periph/src/misc.c periph/src/stm32f4xx_syscfg.c periph/src/stm32f4xx_usart.c periph/src/stm32f4xx_sdio.c


CFLAGS += $(INCLUDES)

SRCS += arch/stm32f4/startup_stm32f4.S system_stm32f4xx.c main.c

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

.PHONY: clean flash bl size term link

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

link:
	arm-none-eabi-gdb 2ci.elf -ex "tar extended-remote :4242"
