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

#include <hal/mmu.h>
#include <base/mmu.h>

void dtlb_miss_handler(void)
{
	unsigned int vaddr, paddr;
	unsigned int EA;

	// retrieve virtual address which caused the page fault
	asm volatile("rcsr %0, dtlbma" : "=r"(vaddr) :: );
	asm volatile("mv %0, ea": "=r"(EA) :: );

	printf("Address 0x%08X caused a DTLB exception\n", EA);
	printf("dtlbma == 0x%08X\n", vaddr);

	/*
	* check if there is an existing mapping for that virtual address
	* if yes: refill the DTLB with it
	* if not: we panic() !
	*/
	paddr = get_mmu_mapping_for(vaddr);
	if (paddr == A_BAD_ADDR || !is_dtlb_mapping(vaddr))
	{
		puts("no such mapping !");
		puts("Unrecoverable DTLB page fault !");
		panic();
	}

	if (!is_dtlb_mapping(vaddr)) {
		puts("Mapping not in DTLB");
		panic();
	}

	printf("Refilling DTLB with mapping 0x%08X->0x%08X\n", vaddr, paddr);
	mmu_dtlb_map(vaddr, paddr);

}

void itlb_miss_handler(void)
{
	unsigned int vaddr, paddr;
//	unsigned int vaddr;
	unsigned int EA;
	unsigned int PSW;
	unsigned int ra;

	asm volatile("mv %0, ea": "=r"(EA) :: );
	asm volatile("rcsr %0, itlbma" : "=r"(vaddr) :: );
	asm volatile("rcsr %0, PSW" : "=r"(PSW) :: );
	asm volatile("mv %0, ra" : "=r"(ra) :: );

	printf("ra == 0x%08X\n", ra);
	printf("ea == 0x%08X\n", EA);
	printf("Address 0x%08X caused an ITLB page fault\n", vaddr);
	printf("PSW == 0x%08X\n", PSW);
//	asm volatile("mv EA, %0" :: "r"(vaddr) : );

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
