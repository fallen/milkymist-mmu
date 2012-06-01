#include <hal/mmu.h>
#include <base/mmu.h>

void dtlb_miss_handler(void)
{
	unsigned int vaddr, paddr;

	// retrieve virtual address which caused the page fault
	asm volatile("rcsr %0, dtlbma" : "=r"(vaddr) :: );

	/*
	* check if there is an existing mapping for that virtual address
	* if yes: refill the DTLB with it
	* if not: we panic() !
	*/
	paddr = get_mmu_mapping_for(vaddr);
	if (paddr == A_BAD_ADDR)
	{
		puts("Unrecoverable page fault !");
		panic();
	}

	printf("Refilling DTLB with mapping 0x%08X->0x%08X\n", vaddr, paddr);
	mmu_dtlb_map(vaddr, paddr);

}
