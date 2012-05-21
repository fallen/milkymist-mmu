#include <hal/mmu.h>

#define MAX_MMU_SLOTS	10
#define NO_EMPTY_SLOT	(MAX_MMU_SLOTS + 1)

#define NULL (0)

struct mmu_mapping {

	unsigned int vaddr;
	unsigned int paddr;

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

	for (i = 0 ; i < 10 ; ++i)
	{
		if (vaddr == NULL)
			empty_slot = i;
		if (vaddr == mappings[i].vaddr && paddr == mappings[i].paddr)
			return 1;
	}
	
	if (empty_slot == NO_EMPTY_SLOT)
		return empty_slot;

	mappings[empty_slot].vaddr = vaddr;
	mappings[empty_slot].paddr = paddr;
	mmu_dtlb_map(vaddr, paddr);

	return 1;
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

	register unsigned int stack;
	int ret;

	asm volatile("mv %0, sp" : "=r"(stack) :: );

	ret = mmu_map(stack, stack);
	check_for_error(ret);

	ret = mmu_map(stack-0x1000, stack-0x1000);
	check_for_error(ret);

	printf("stack == 0x%08X\n", stack);

}
