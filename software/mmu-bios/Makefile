MMDIR=../..
include $(MMDIR)/software/include.mak

OBJECTS=crt0.o 
SEGMENTS=-j .text -j .data -j .rodata -j .lol

all: bios.bin

%.bin: %.elf
	$(MAKE) -C $(MMDIR)/tools
	$(OBJCOPY) --pad-to 0x350 $(SEGMENTS) -O binary $< $@
	chmod -x $@

bios.elf: linker.ld $(OBJECTS) $(LIBS)

%.elf:
	$(LD) $(LDFLAGS) -T $< -N -o $@ $(OBJECTS) --start-group --end-group
	chmod -x $@

.PHONY: clean depend

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	rm -f *.o bios.elf bios.bin .*~ *~ Makefile.bak

