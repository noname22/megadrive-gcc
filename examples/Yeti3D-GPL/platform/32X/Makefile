MDLD = m68k-elf-ld
MDAS = m68k-elf-as
SHLD = sh-elf-ld
SHCC = sh-elf-gcc
SHAS = sh-elf-as
RM = rm -f

export VPATH  := $(CURDIR) $(CURDIR)/../../src

FLAGS = -m2 -mb -O6 -Wall -g -fomit-frame-pointer -D__MARS__ -DDBL_PIXEL
HWFLAGS = -m2 -mb -O1 -Wall -g -fomit-frame-pointer -D__MARS__ -DDBL_PIXEL
LDFLAGS = -T $(GENDEV)/ldscripts/mars.ld -Map output.map -relax -small -e _start --oformat binary

INCS = -I. -I./../../src -I$(GENDEV)/sh-elf/include -I$(GENDEV)/sh-elf/sh-elf/include

LIBS = -L$(GENDEV)/sh-elf/sh-elf/lib -L$(GENDEV)/sh-elf/lib/gcc/sh-elf/4.5.2 -lm -lc -lgcc -lgcc-Os-4-200

OBJS = sh2_crt0.o main.o data.o draw.o game.o maps.o sprites.o yeti.o module.o sound.o hw_32x.o font.o files-psyg.o

all: m68k_crt0.bin m68k_crt1.bin hw_32x.o YETI3D-32X.bin

YETI3D-32X.bin: $(OBJS)
	$(SHLD) $(LDFLAGS) -o temp.bin $(OBJS) $(LIBS)
	dd if=temp.bin of=YETI3D-32X.bin bs=1024K conv=sync

m68k_crt0.bin: m68k_crt0.s
	$(MDAS) -m68000 --register-prefix-optional -o m68k_crt0.o m68k_crt0.s
	$(MDLD) -T $(GENDEV)/ldscripts/md.ld --oformat binary -o m68k_crt0.bin m68k_crt0.o

m68k_crt1.bin: m68k_crt1.s
	$(MDAS) -m68000 --register-prefix-optional -o m68k_crt1.o m68k_crt1.s
	$(MDLD) -T $(GENDEV)/ldscripts/md.ld --oformat binary -o m68k_crt1.bin m68k_crt1.o

hw_32x.o: hw_32x.c
	$(SHCC) $(HWFLAGS) -c $< -o $@

%.o: %.c
	$(SHCC) $(FLAGS) -c $< -o $@

%.o: %.s
	$(SHAS) -I. --small -o $@ $<

clean:
	$(RM) -f *.o *.out *.bin *.map
