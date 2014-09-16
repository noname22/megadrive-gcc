MDLD = m68k-elf-ld
MDAS = m68k-elf-as
SHLD = sh-elf-ld
SHCC = sh-elf-gcc
SHAS = sh-elf-as
SHOBJC = sh-elf-objcopy
RM = rm -f

CCFLAGS = -m2 -mb -O3 -Wall -c -fomit-frame-pointer
HWCCFLAGS = -m2 -mb -O1 -Wall -c -fomit-frame-pointer
LINKFLAGS = -T $(GENDEV)/ldscripts/mars.ld -Wl,-Map=output.map -nostdlib

INCS = -I. -I$(GENDEV)/sh-elf/include -I$(GENDEV)/sh-elf/sh-elf/include

LIBS = -L$(GENDEV)/sh-elf/sh-elf/lib -L$(GENDEV)/sh-elf/lib/gcc/sh-elf/$(GENDEV_GCC_VERSION) -lc -lgcc -lgcc-Os-4-200 -lnosys

OBJS = sh2_crt0.o main.o hw_32x.o font.o

all: m68k_crt0.bin m68k_crt1.bin TicTacToe32X.bin

TicTacToe32X.bin: TicTacToe.elf
	$(SHOBJC) -O binary $< temp.bin
	dd if=temp.bin of=$@ bs=64K conv=sync

TicTacToe.elf: $(OBJS)
	$(SHCC) $(LINKFLAGS) $(OBJS) $(LIBS) -o TicTacToe.elf

m68k_crt0.bin: m68k_crt0.s
	$(MDAS) -m68000 --register-prefix-optional -o m68k_crt0.o m68k_crt0.s
	$(MDLD) -T $(GENDEV)/ldscripts/md.ld --oformat binary -o m68k_crt0.bin m68k_crt0.o

m68k_crt1.bin: m68k_crt1.s
	$(MDAS) -m68000 --register-prefix-optional -o m68k_crt1.o m68k_crt1.s
	$(MDLD) -T $(GENDEV)/ldscripts/md.ld --oformat binary -o m68k_crt1.bin m68k_crt1.o

hw_32x.o: hw_32x.c
	$(SHCC) $(HWCCFLAGS) $< -o $@

%.o: %.c
	$(SHCC) $(CCFLAGS) $< -o $@

%.o: %.s
	$(SHAS) --small -o $@ $<

clean:
	$(RM) *.o *.bin *.elf *.map
