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

inline void mmu_dtlb_map(unsigned int vpfn, unsigned int pfn)
{

	asm volatile	("ori %0, %0, 1\n\t"
 			 "wcsr tlbvaddr, %0"::"r"(vpfn):);

	asm volatile	("ori %0, %0, 1\n\t"
			 "wcsr tlbpaddr, %0"::"r"(pfn):);

	asm volatile	("xor r11, r11, r11\n\t"
			 "ori r11, r11, 0x5\n\t"
			 "wcsr tlbctrl, r11":::"r11");

}

inline void mmu_dtlb_invalidate(unsigned int vaddr)
{
	asm volatile ("ori %0, %0, 1\n\t"
		      "wcsr tlbvaddr, %0"::"r"(vaddr):);

	asm volatile ("xor r11, r11, r11\n\t"
		      "ori r11, r11, 0x21\n\t"
		      "wcsr tlbctrl, r11":::"r11");
}

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
	register unsigned int stack;
	int empty_slot = NO_EMPTY_SLOT;
	vaddr = get_pfn(vaddr);
	paddr = get_pfn(paddr);

	asm volatile("mv %0, sp" : "=r"(stack) :: );
	printf("stack == 0x%08X\n", stack);

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

/* This function activates the MMU
 * then reads from virtual address "vaddr"
 * and store the result in temporary variable "data".
 * Then MMU is disactivated and the content of "data"
 * is returned.
 */

unsigned int read_word_with_mmu_enabled(unsigned int vaddr)
{
	register unsigned int data;
	asm volatile(
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x11\n\t"
		"wcsr tlbctrl, r11\n\t" // Activates the MMU
		"xor r0, r0, r0\n\t"
		"xor r11, r11, r11\n\t"
		"or r11, r11, %1\n\t"
		"lw  %0, (r11+0)\n\t" // Reads from virtual address "vaddr"
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x9\n\t"
		"wcsr tlbctrl, r11\n\t" // Disactivates the MMU
		"xor r0, r0, r0\n\t" : "=&r"(data) : "r"(vaddr) : "r11"
	);

	return data;
}

unsigned int write_word_with_mmu_enabled(register unsigned int vaddr, register unsigned int data)
{
	asm volatile(
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x11\n\t"
		"wcsr tlbctrl, r11\n\t" // Activates the MMU
		"xor r0, r0, r0\n\t"
		"sw  (%0 + 0), %1\n\t" // Writes "data" to virtual address "vaddr"
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x9\n\t"
		"wcsr tlbctrl, r11\n\t" // Disactivates the MMU
		"xor r0, r0, r0\n\t" :: "r"(vaddr), "r"(data) : "r11"
	);
}
