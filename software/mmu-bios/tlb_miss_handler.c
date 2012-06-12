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
	if (paddr == A_BAD_ADDR || !is_dtlb_mapping(vaddr))
	{
		puts("Unrecoverable DTLB page fault !");
		panic();
	}

	printf("Refilling DTLB with mapping 0x%08X->0x%08X\n", vaddr, paddr);
	mmu_dtlb_map(vaddr, paddr);
}

void itlb_miss_handler(void)
{
	unsigned int vaddr, paddr;

	asm volatile("rcsr %0, itlbma" : "=r"(vaddr) :: );
	printf("Address 0x%08X caused an ITLB page fault\n", vaddr);

	paddr = get_mmu_mapping_for(vaddr);
	if (paddr == A_BAD_ADDR)
	{
		puts("No such mapping !");
		puts("Unrecoverable ITLB page fault !");
		panic();
	}

	if (!is_itlb_mapping(vaddr))
	{
		puts("Mapping not in ITLB");
		panic();
	}

	printf("Refilling ITLB with mapping 0x%08X->0x%08X\n", vaddr, paddr);
	mmu_itlb_map(vaddr, paddr);
}
