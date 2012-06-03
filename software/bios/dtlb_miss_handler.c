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
		puts("[TLB miss handler] Unrecoverable page fault !");
		panic();
	}

	printf("[TLB miss handler] Refilling DTLB with mapping 0x%08X->0x%08X\n", vaddr, paddr);
	mmu_dtlb_map(vaddr, paddr);

}
