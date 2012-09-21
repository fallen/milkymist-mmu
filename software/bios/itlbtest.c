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

#ifdef __ASSEMBLER__
#define MMPTR(x) x
#else
#define MMPTR(x) (*((volatile unsigned int *)(x)))
#endif
#define CSR_UART_RXTX 		MMPTR(0xe0000000)
#define UART_STAT_THRE		(0x1)
#define CSR_UART_STAT		MMPTR(0xe0000008)

#define OPCODE_MASK 	(0xfc000000)
#define OPCODE_BI	(0xe0000000)
#define OPCODE_CALLI	(0xf8000000)

unsigned int relocate(unsigned int *opcode, unsigned int *reloc_addr)
{
	unsigned int reloc_diff = reloc_addr - opcode;
	unsigned int nopcode;
	int imm;

	switch (*opcode & OPCODE_MASK)
	{
		case OPCODE_BI:
			puts("This is a BI");
			return *opcode;
		break;

		case OPCODE_CALLI:
			puts("This is a CALLI");
			printf("reloc_diff == 0x%08X , reloc_addr = 0x%08X\n", reloc_diff, (unsigned int)reloc_addr);
			imm = (*opcode) & ~(OPCODE_MASK);
			nopcode = (~OPCODE_MASK) & (imm - reloc_diff);
			nopcode |= OPCODE_CALLI;
			return nopcode;
		break;

		default:
		return *opcode;
	}
}

void f(void) {
	unsigned int ret;
	char c = '@';
	CSR_UART_RXTX = c;
	while(!(CSR_UART_STAT & UART_STAT_THRE));
	puts("totolol");
	printf("\n\ntest %d %c %s\n\n", 5, (char)186, "abcd");
	puts("@");
//	asm volatile("bi f" ::: ); // We intinitely loop to f()
	asm volatile("lw %0, (sp+4)" : "=r"(ret) :: );
	printf("Return address == 0x%08X\n", ret);
	asm volatile("xor r0, r0, r0\n\t"
		     "xor r0, r0, r0" ::: );
}

void itlbtest(void) {
	register unsigned int stack, f_addr;
	unsigned int *p;
	unsigned int *pdest;
	unsigned int addr;
	int size_of_f = 10;

	asm volatile("mv %0, sp" : "=r"(stack) :: );
	printf("stack == 0x%08X\n", stack);

	printf("f() is located at 0x%p\n", f);

	f_addr = 0x44004000;
	printf("Mapping f() into virtual memory at 0x%08X [physical == 0x%08X]\n", f_addr, f_addr+0x1000);

	for (addr = 0x00860000 ; addr <= 0x00875000 ; addr += 0x1000)
		mmu_map(addr, addr, ITLB_MAPPING | DTLB_MAPPING | MAPPING_CAN_READ);

	mmu_map(stack, stack, DTLB_MAPPING | MAPPING_CAN_READ | MAPPING_CAN_WRITE);
//	mmu_map(f_addr, f_addr + 0x1000, ITLB_MAPPING | MAPPING_CAN_READ);
	mmu_map(f, f, ITLB_MAPPING | MAPPING_CAN_READ);
	mmu_map(itlbtest, itlbtest, ITLB_MAPPING | MAPPING_CAN_READ);
//	mmu_map(call_function_with_itlb_enabled, call_function_with_itlb_enabled, ITLB_MAPPING | MAPPING_CAN_READ);
	puts("Mapping DONE");

	puts("Printing MMU mappings : \n\n");

	mmu_map_print();

//	mmu_itlb_invalidate_line(f_addr);

	// We copy f's code to 0x44005000
//	for (p = (unsigned int *)f, pdest = (unsigned int *)0x44005000 ; p < (unsigned int *)f + size_of_f ; p++, pdest++)
//	{
//		unsigned int *vpdest = (unsigned int *)((unsigned int)pdest - 0x1000); // Virtual destination is physical one - 0x1000
//		printf("Before : 0x%08X\n", *p);
//		*pdest = relocate(p, vpdest);
//		printf("After : 0x%08X\n", *pdest);
//		*pdest = *p;
//	}
//	puts("Copy DONE");

	asm volatile("wcsr ICC, r0\n\t"
		     "xor r0, r0, r0\n\t"
		     "xor r0, r0, r0\n\t"
		     "xor r0, r0, r0\n\t"
		     "xor r0, r0, r0");
	asm volatile("wcsr DCC, r0\n\t"
		     "xor r0, r0, r0");
	puts("Instruction and Data caches have been invalidated");

	call_function_with_itlb_enabled(f);
	disable_itlb();
	disable_dtlb();
	puts("Call DONE");
}
