MMDIR=../..
include $(MMDIR)/software/include.mak

OBJECTS=crt0.o main.o dtlb_load_test.o mmu.o isr.o vsnprintf-nofloat.o tlb_miss_handler.o dtlb_exception_handling_tests.o
SEGMENTS=-j .text -j .data -j .rodata

all: bios.bin

%.bin: %.elf
	$(MAKE) -C $(MMDIR)/tools
	$(OBJCOPY) --pad-to 0x350 $(SEGMENTS) -O binary $< $@
	chmod -x $@

mmu_test_gen: mmu_test_gen.c
	gcc $< -o $@
	chmod +x $@

dtlb_load_test.c: mmu_test_gen
	./$< > $@

bios.elf: linker.ld $(OBJECTS) $(LIBS)

%.elf:
	$(LD) $(LDFLAGS) -T $< -N -o $@ $(OBJECTS) --start-group --end-group
	chmod -x $@

.PHONY: clean depend

depend:
	makedepend -Y -- $(CFLAGS) -- *.c

clean:
	rm -f *.o bios.elf bios.bin .*~ *~ Makefile.bak

