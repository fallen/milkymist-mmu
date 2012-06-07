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

unsigned int mmu_map(unsigned int vaddr, unsigned int paddr) {
	int i;
	int empty_slot = NO_EMPTY_SLOT;
	vaddr = get_pfn(vaddr);
	paddr = get_pfn(paddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
	{
		if (!mappings[i].valid)
			empty_slot = i;
		if ((vaddr == mappings[i].vaddr) && (paddr == mappings[i].paddr) && mappings[i].valid)
		{
			puts("Already mapped !");
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
	mappings[empty_slot].valid = 1;
	mmu_dtlb_map(vaddr, paddr);
	printf("mapping 0x%08X->0x%08X in slot %d [0x%p]\n", vaddr, paddr, empty_slot, &mappings[empty_slot]);

	return 1;
}

unsigned int get_mmu_mapping_for(unsigned int vaddr) {
	int i;
	vaddr = get_pfn(vaddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
		if (mappings[i].valid && (vaddr == mappings[i].vaddr))
			return mappings[i].paddr;

	return A_BAD_ADDR;
}

unsigned char remove_mmu_mapping_for(unsigned int vaddr) {
	int i;
	vaddr = get_pfn(vaddr);

	for (i = 0 ; i < MAX_MMU_SLOTS ; ++i)
	{
		if (mappings[i].valid && (vaddr == mappings[i].vaddr))
		{
			mmu_dtlb_invalidate(vaddr);
			mappings[i].valid = 0;
			return 1;
		}
	}
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