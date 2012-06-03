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
#include <base/stdio.h>

void dtlb_exception_handling_tests() {

	register unsigned int stack, addr;
	volatile unsigned int data;
	int ret;

	asm volatile("mv %0, sp" : "=r"(stack) :: );

	ret = mmu_map(stack, stack);
	check_for_error(ret);

	ret = mmu_map(stack-0x1000, stack-0x1000);
	check_for_error(ret);

	printf("stack == 0x%08X\n", stack);

	addr = 0x44004004;

	printf("\n=> Mapping 0x%08X to 0x%08X\n", addr, addr);
	ret = mmu_map(addr, addr);
	check_for_error(ret);

	data = 42;
	printf("=> Writing %d to physical address 0x%08X\n", data, addr);
	*(unsigned int *)addr = data;

	data = 0; // clears data to make sure we are not reading back previous value cached in a register or so
	printf("=> Activating the MMU and reading form virtual address 0x%08X\n", addr);
	data = read_word_with_mmu_enabled(addr);
	printf("\n<= Reading %d from virtual address 0x%08X\n\n", data, addr);

	printf("=> Invalidating the mapping of virtual address 0x%08X in the TLB\n", addr);
	mmu_dtlb_invalidate(addr);

	data = 43;
	printf("=> Writing %d to physical address 0x%08X\n", data, addr);
	*(unsigned int *)addr = data;

	data = 0; // clears data to make sure we are not reading back previous value cached in a register or so
	printf("=> Activating the MMU and reading form virtual address 0x%08X\n", addr);
	data = read_word_with_mmu_enabled(addr);
	printf("\n<= Reading %d from virtual address 0x%08X\n\n", data, addr);

	printf("=> Mapping 0x%08X to 0%08X\n", addr, addr+0x1000);
	ret = mmu_map(addr, addr+0x1000); // Map to something else
	check_for_error(ret);

	printf("=> Invalidating the mapping of virtual address 0x%08X in the TLB\n", addr);
	mmu_dtlb_invalidate(addr); // AND invalidate the mapping

	data = 44;
	printf("=> Writting %d to physical address 0x%08X\n", data, addr+0x1000);
	*(unsigned int *)(addr + 0x1000) = data;

	data = 0; // clears data to make sure we are not reading back previous value cached in a register or so
	printf("=> Activating the MMU and reading form virtual address 0x%08X\n", addr);
	data = read_word_with_mmu_enabled(addr);
	printf("\n<= Reading %d from virtual address 0x%08X\n\n", data, addr);

}
