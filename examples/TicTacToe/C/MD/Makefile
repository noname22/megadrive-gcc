CC = m68k-elf-gcc
AS = m68k-elf-as
OBJC = m68k-elf-objcopy
RM = rm -f
ASMZ80 = zasm
BINTOC = bin2c

OPTION =
INCS = -I. -I$(GENDEV)/m68k-elf/include -I$(GENDEV)/m68k-elf/m68k-elf/include
CCFLAGS = $(OPTION) -m68000 -Wall -O2 -c -fomit-frame-pointer
HWCCFLAGS = $(OPTION) -m68000 -Wall -O1 -c -fomit-frame-pointer
Z80FLAGS = -vb2
ASFLAGS = -m68000 --register-prefix-optional
LIBS = -L$(GENDEV)/m68k-elf/lib -L$(GENDEV)/m68k-elf/m68k-elf/lib -lc -lgcc -lnosys
LINKFLAGS = -T $(GENDEV)/ldscripts/md.ld -Wl,-Map=output.map -nostdlib

OBJS = crt0.o main.o hw_md.o font.o

all: TicTacToe.bin

TicTacToe.bin: TicTacToe.elf
	$(OBJC) -O binary $< temp.bin
	dd if=temp.bin of=$@ bs=8K conv=sync

TicTacToe.elf: $(OBJS)
	$(CC) $(LINKFLAGS) $(OBJS) $(LIBS) -o TicTacToe.elf

%.o80: %.s80
	$(ASMZ80) $(Z80FLAGS) -o $@ $<

%.c: %.o80
	$(BINTOC) $<

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	$(RM) *.o *.bin *.elf *.map
