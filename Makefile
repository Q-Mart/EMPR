PKG=/usr/local/pkg

# source directories
SRC = ./src
BIN = ./bin
INCLUDE = ./include
INC_BIN = $(BIN)/include
INC_BIN_DBG = $(BIN)/dbg_include

INCLUDE_LIVE=$(INCLUDE)/live/
INCLUDE_DBG=$(INCLUDE)/dbg/

# Safe mkdir
MKDIR_P = mkdir -p

ARCH=arm-none-eabi
CC=$(ARCH)-gcc
HCC= /usr/bin/gcc
OBJCOPY=$(ARCH)-objcopy

USER:=$(shell whoami)

SOURCERY=$(PKG)/sourcery-g++-lite-arm-eabi-2010.09.51-i686-1
GNU_VERSION=4.5.0
THUMB2GNULIB=$(SOURCERY)/$(ARCH)/lib/$(GNU_VERSION)/thumb2
THUMB2GNULIB2=$(SOURCERY)/$(ARCH)/lib/thumb2

# "Cortex Microcontroller Software Interface Standard" Startup files
CMSIS=$(PKG)/lpc1700-cmsis-lite-2011.01.26-i686-1
CMSISINCLUDES=-I$(CMSIS)/include
CMSISFL=$(CMSIS)/lib/core_cm3.o \
	$(CMSIS)/lib/system_LPC17xx.o \
	$(CMSIS)/lib/startup_LPC17xx.o
LDSCRIPT = $(CMSIS)/lib/ldscript_rom_gnu.ld

ARCH_FLAGS=-mcpu=cortex-m3  -mthumb  -Wall  -O0  -mapcs-frame  -D__thumb2__=1 \
  -msoft-float  -gdwarf-2  -mno-sched-prolog  -fno-hosted  -mtune=cortex-m3 \
  -march=armv7-m  -mfix-cortex-m3-ldrd   -ffunction-sections  -fdata-sections \
          -D__RAM_MODE__=0 $(CMSISINCLUDES)

CFLAGS=$(ARCH_FLAGS) -I$(SRC) -I$(INCLUDE)/

LDFLAGS=$(CMSISFL) -static -mcpu=cortex-m3 -mthumb -mthumb-interwork \
	-Wl,--start-group -L$(THUMB2GNULIB) -L$(THUMB2GNULIB2) \
	-lc -lg -lstdc++ -lsupc++  -lgcc -lm  -Wl,--end-group \
	-Xlinker -Map -Xlinker bin/lpc1700.map -Xlinker -T $(LDSCRIPT)

LDFLAGS+=-L$(CMSIS)/lib -lDriversLPC17xxgnu

EXECNAME= $(BIN)/main

LIBS_C_FILES = $(wildcard $(INCLUDE_LIVE)/*.c)
LIBS_O_FILES = $(patsubst $(INCLUDE_LIVE)/%.c, $(INC_BIN)/%.o, $(LIBS_C_FILES))

LIBS_C_FILES_DBG = $(wildcard $(INCLUDE_DBG)/*.c)
LIBS_O_FILES_DBG = $(patsubst $(INCLUDE_DBG)/%.c, $(INC_BIN_DBG)/%.o, $(LIBS_C_FILES_DBG))

C_FILES = $(wildcard $(SRC)/*.c)
O_FILES = $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(C_FILES))

LIVE_OBJS = $(O_FILES) $(LIBS_O_FILES)
DBG_OBJS = $(O_FILES) $(LIBS_O_FILES_DBG)

$(BIN)/%.o: $(SRC)/%.c
	$(MKDIR_P) $(BIN)
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(INC_BIN)/%.o: $(INCLUDE_LIVE)/%.c
	$(MKDIR_P) $(INC_BIN)
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(INC_BIN_DBG)/%.o: $(INCLUDE_DBG)/%.c
	$(MKDIR_P) $(INC_BIN_DBG)
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
.phony: all
all:	program
	@echo "Build finished"

.phony: debug
debug:	INCLUDE_C=$(INCLUDE)/dbg/
	CC=gcc	
	CFLAGS=-I$(INCLUDE) -I$(INCLUDE_DBG)
	LDFLAGS=
debug:	programDebug
	@echo "Debug build finished"

.phony: program
program: $(LIVE_OBJS)
	$(CC) $(CFLAGS) -o $(EXECNAME) $(LIVE_OBJS) $(LDFLAGS)
	$(OBJCOPY) -I elf32-little -O binary $(EXECNAME) $(EXECNAME).bin

.phony: programDebug
programDebug: $(DBG_OBJS)
	$(CC) $(CFLAGS) -o $(EXECNAME) $(DBG_OBJS) $(LDFLAGS)

# clean out the source tree ready to re-build
.phony: clean
clean:
	rm -rf bin/
	rm -f `find . | grep \~`
	rm -f *.swp *.o */*.o */*/*.o  *.log
	rm -f *.d */*.d *.srec */*.a bin/*.map
	rm -f *.elf *.wrn bin/*.bin log *.hex
	rm -f $(INC_BIN)
	rm -f $(BIN)

# install software to board, remember to sync the file systems
.phony: install
install:
	@echo "Copying " $(EXECNAME) "to the MBED file system"
	cp $(EXECNAME).bin /media/$(USER)/MBED &
	sync
	@echo "Now press the reset button on all MBED file systems"
