#include <hal/mmu.h>

#define PAGE_SIZE	(4096)

#define MAX_MMU_SLOTS	10
#define NO_EMPTY_SLOT	(MAX_MMU_SLOTS + 1)

#define A_BAD_ADDR	(0)

#define NULL (0)

#define get_pfn(x)	(x & ~(PAGE_SIZE - 1))

struct mmu_mapping {

	unsigned int vaddr;
	unsigned int paddr;
	char valid;

} mappings[10];

/*
 * This records in a global structure all MMU mappings
 * If such a mapping already exists the function returns immediately.
 * If such a mapping does not exist yet, vaddr is mapped to paddr and 
 * the mapping is recorded in the mappings[] global structure array in
 * an empty slot.
 * If there is no empty slot anymore then we fail
 */

unsigned int mmu_map(unsigned int vaddr, unsigned int paddr) {
	int i;
	int empty_slot = NO_EMPTY_SLOT;
	vaddr = get_pfn(vaddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
	{
		if (!mappings[i].valid)
			empty_slot = i;
		if (vaddr == mappings[i].vaddr && paddr == mappings[i].paddr)
			return 1;
	}
	
	if (empty_slot == NO_EMPTY_SLOT)
		return empty_slot;

	mappings[empty_slot].vaddr = vaddr;
	mappings[empty_slot].paddr = paddr;
	mappings[empty_slot].valid = 1;
	mmu_dtlb_map(vaddr, paddr);

	return 1;
}

unsigned int get_mmu_mapping_for(unsigned int vaddr) {
	int i;
	vaddr = get_pfn(vaddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
		if (mappings[i].valid && vaddr == mappings[i].vaddr)
			return mappings[i].paddr;

	return A_BAD_ADDR;
}

unsigned int invalidate_mmu_mapping(unsigned int vaddr) {
	int i;
	vaddr = get_pfn(vaddr);
	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
	{
		if (mappings[i].valid && vaddr == mappings[i].vaddr) {
			mmu_dtlb_invalidate(vaddr);
			mappings[i].valid = 0;
			return 1;
		}
	}
	return 0;
}

static void panic(void) {
	puts("PANIC !");
	while(1)
		asm volatile("nop");
}

static void check_for_error(int ret) {
	if (ret)
		return;

	if (ret == NO_EMPTY_SLOT) {
		puts("No empty slot in MMU mappings structure anymore");
		panic();
	}

	if ( !ret ) {
		puts("Unknown issue");
		panic();
	}
}

void dtlb_exception_handling_tests() {

	register unsigned int stack, addr, data;
	int ret;

	asm volatile("mv %0, sp" : "=r"(stack) :: );

	ret = mmu_map(stack, stack);
	check_for_error(ret);

	ret = mmu_map(stack-0x1000, stack-0x1000);
	check_for_error(ret);

	printf("stack == 0x%08X\n", stack);

	addr = 0x44002342; // Random address
	*(unsigned int *)addr = 42;
//	mmu_map(addr, addr);

	printf("Address 0x%08X mapped to itself, value : ", addr);

	asm volatile(
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x11\n\t"
		"wcsr tlbctrl, r11\n\t" // this activates the mmu
		"xor r0, r0, r0\n\t"
		"xor r11, r11, r11\n\t"
		"or r11, r11, %1\n\t"
		"lw  %0, (r11+0)\n\t"
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x9\n\t"
		"wcsr tlbctrl, r11\n\t" // this disactivates the mmu
		"xor r0, r0, r0" : "=&r"(data) : "r"(addr) : "r11"
	);

	printf("%d\n", data);

	invalidate_mmu_mapping(addr);

	printf("DTLB has just been invalidated, next access to 0x%08X should trigger a DTLB exception\n", addr);

	printf("Address 0x%08X not mapped, value : ", addr);

	asm volatile(
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x11\n\t"
		"wcsr tlbctrl, r11\n\t" // this activates the mmu
		"xor r0, r0, r0\n\t"
		"xor r11, r11, r11\n\t"
		"or r11, r11, %1\n\t"
		"lw  %0, (r11+0)\n\t"
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x9\n\t"
		"wcsr tlbctrl, r11\n\t" // this disactivates the mmu
		"xor r0, r0, r0" : "=&r"(data) : "r"(addr) : "r11"
	);

	printf("%d\n", data);

}
