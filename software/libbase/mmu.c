/*
 * Milkymist SoC (Software)
 * Copyright (C) 2012 Yann Sionneau <yann.sionneau@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* @vpfn : virtual page frame number
 * @pfn  : physical page frame number
 */

#include <hal/mmu.h>
#include <base/mmu.h>

struct mmu_mapping mappings[MAX_MMU_SLOTS];

/*
 * This records in a global structure all MMU mappings
 * If such a mapping already exists the function returns immediately.
 * If such a mapping does not exist yet, vaddr is mapped to paddr and
 * the mapping is recorded in the mappings[] global structure array in
 * an empty slot.
 * If there is no empty slot anymore then we fail
 */

unsigned int mmu_map(unsigned int vaddr, unsigned int paddr, char metadata) {
	int i;
	int empty_slot = NO_EMPTY_SLOT;
	vaddr = get_pfn(vaddr);
	paddr = get_pfn(paddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
	{
		if (!(mappings[i].metadata & (MAPPING_IS_VALID)))
			empty_slot = i;
		if ((vaddr == mappings[i].vaddr) && (paddr == mappings[i].paddr) && (mappings[i].metadata & MAPPING_IS_VALID))
		{
			puts("Already mapped, updating metadata !");
			mappings[i].metadata |= metadata;
			// set the permission bits in lower bits of paddr to be written in the TLB
			paddr |= (metadata & MAPPING_CAN_RW);

			if (mappings[i].metadata & ITLB_MAPPING)
				mmu_itlb_map(vaddr, paddr);
			if (mappings[i].metadata & DTLB_MAPPING)
				mmu_dtlb_map(vaddr, paddr);
			return 1;
		} else if ((vaddr == mappings[i].vaddr) && (paddr != mappings[i].paddr) && (mappings[i].metadata & MAPPING_IS_VALID))
		{
			printf("Vaddr already mapped to another Paddr (0x%08X), overwritting...\n", mappings[i].paddr);
			mappings[i].paddr = paddr;
			mappings[i].metadata = (metadata | MAPPING_IS_VALID);
			// set the permission bits in lower bits of paddr to be written in the TLB
			paddr |= (metadata & MAPPING_CAN_RW);

			if (mappings[i].metadata & ITLB_MAPPING)
				mmu_itlb_map(vaddr, paddr);
			if (mappings[i].metadata & DTLB_MAPPING)
				mmu_dtlb_map(vaddr, paddr);
			return 1;
		}
	}

	if (empty_slot == NO_EMPTY_SLOT)
	{
		puts("No more slots !");
		return empty_slot;
	}

	mappings[empty_slot].vaddr = vaddr;
	mappings[empty_slot].paddr = paddr;
	mappings[empty_slot].metadata = (metadata | MAPPING_IS_VALID);
	// set the permission bits in lower bits of paddr to be written in the TLB
	paddr |= (metadata & MAPPING_CAN_RW);

	if (metadata & ITLB_MAPPING)
		mmu_itlb_map(vaddr, paddr);

	if (metadata & DTLB_MAPPING)
		mmu_dtlb_map(vaddr, paddr);

	printf("mapping 0x%08X->0x%08X in slot %d [0x%p]\n", vaddr, paddr, empty_slot, &mappings[empty_slot]);

	return 1;
}

unsigned int get_mmu_mapping_for(unsigned int vaddr) {
	int i;
	vaddr = get_pfn(vaddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
		if ((mappings[i].metadata & MAPPING_IS_VALID) && (vaddr == mappings[i].vaddr))
			return get_pfn(mappings[i].paddr);

	return A_BAD_ADDR;
}

unsigned char remove_mmu_mapping_for(unsigned int vaddr) {
	int i;
	vaddr = get_pfn(vaddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
	{
		if ((mappings[i].metadata & MAPPING_IS_VALID) && (vaddr == mappings[i].vaddr))
		{
			mmu_dtlb_invalidate_line(vaddr);
			mappings[i].metadata &= ~MAPPING_IS_VALID;
			return 1;
		}
	}
	return 0;
}

unsigned char is_dtlb_mapping(unsigned int vaddr) {
	unsigned int i;
	vaddr = get_pfn(vaddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
		if ((mappings[i].vaddr == vaddr) && (mappings[i].metadata & (MAPPING_IS_VALID)) && (mappings[i].metadata & (DTLB_MAPPING)))
			return 1;

	return 0;
}

unsigned char is_itlb_mapping(unsigned int vaddr) {
	unsigned int i;
	vaddr = get_pfn(vaddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
		if ((mappings[i].vaddr == vaddr) && (mappings[i].metadata & (MAPPING_IS_VALID)) && (mappings[i].metadata & (ITLB_MAPPING)))
			return 1;

	return 0;
}

void panic(void) {
	puts("PANIC !");
	while(1)
		asm volatile("nop");
}

void check_for_error(int ret) {
	if (ret == 1)
		return;

	if (ret == NO_EMPTY_SLOT)
	{
		puts("No empty slot in MMU mappings structure anymore");
		panic();
	}

	if ( !ret )
	{
		puts("Unknown issue");
		panic();
	}
}

void mmu_map_print(void)
{
	unsigned int i;

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
	{
		if (mappings[i].metadata & MAPPING_IS_VALID)
		{
			printf("[%d] 0x%08X -> 0x%08X:", i, mappings[i].vaddr, mappings[i].paddr);
			if (mappings[i].metadata & ITLB_MAPPING)
				printf(" ITLB ");
			if (mappings[i].metadata & DTLB_MAPPING)
				printf(" DTLB ");
			puts("");
		}
	}

}
