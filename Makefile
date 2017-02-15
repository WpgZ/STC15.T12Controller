
include build/mkfiles/rules.mk

OUTPUT = DIST

TARGET = t12ctrl.hex

SRCS = src/ADC.c src/beeper.c src/config.c src/delay.c src/display.c src/eeprom.c src/pid.c src/rotaryencoder.c

STC_HEADER = include/GENERATED/XSTC15F204EA.h

CFLAGS = -DTARGET_CHIP=STC15F204 -I$(abspath include)


$(OUTPUT)/$(TARGET): $(patsubst %.c,%.rel,$(SRCS))
	mkdir -p $(OUTPUT)
	sdcc $(CFLAGS) src/main.c $(patsubst %.c,%.rel,$(SRCS)) -o $@


$(patsubst %.c,%.rel,$(SRCS)): $(STC_HEADER)


$(STC_HEADER):
	mkdir -p $(dir $(STC_HEADER))
	./build/utils/keil2sdc.py -f build/keil/include/STC15F204EA.h > $@


clean:
	rm -rf $(dir $(STC_HEADER)) $(OUTPUT)
	rm -rf src/*.rel src/*.asm src/*.sym src/*.lst src/*.rst


.PHONY: clean 
