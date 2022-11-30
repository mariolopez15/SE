#TOOLCHAIN=~/toolchain/gcc-arm-none-eabi-4_9-2014q4/bin
#PREFIX=$(TOOLCHAIN)/arm-none-eabi-
PREFIX=arm-none-eabi-

ARCHFLAGS=-mthumb -mcpu=cortex-m0plus
COMMONFLAGS=-g3 -Og -Wall -Werror $(ARCHFLAGS)

CFLAGS=-I./includes -I./drivers $(COMMONFLAGS)
LDFLAGS=$(COMMONFLAGS) --specs=nano.specs -Wl,--gc-sections,-Map,$(theTARGET).map,-Tlink.ld
LDLIBS=

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
SIZE=$(PREFIX)size
RM=rm -f

SRC=$(wildcard *.c drivers/*.c)
OBJ=$(patsubst %.c, %.o, $(SRC))
OBJ.tpm=$(patsubst %.c, %.o, $(filter-out lptmr.c ,$(SRC)))
OBJ.lptmr=$(patsubst %.c, %.o, $(filter-out tpm.c ,$(SRC)))

TARGET = lptmr tpm
all: $(TARGET:=.elf)

clean:
	$(RM) lptmr.map tpm.map lptmr.elf tpm.elf $(OBJ)

flash_lptmr: lptmr.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

lptmr.elf: theTARGET = lptmr
lptmr.elf: $(OBJ.lptmr)
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(SIZE) $@



flash_tpm: tpm.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

tpm.elf: theTARGET = tpm
tpm.elf: $(OBJ.tpm)
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(SIZE) $@


