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

#ifndef __MMU_H__
#define __MMU_H__

#define PAGE_SIZE	(4096)
#define MAX_MMU_SLOTS	30
#define NO_EMPTY_SLOT	(MAX_MMU_SLOTS + 1)
#define A_BAD_ADDR	0xffffffff
#define get_pfn(x)	(x & ~(PAGE_SIZE - 1))

#define ITLB_MAPPING		(1)
#define DTLB_MAPPING		(1 << 1)
#define MAPPING_CAN_READ	(1 << 2)
#define MAPPING_CAN_WRITE	(1 << 3)
#define MAPPING_COPY_ON_WRITE	(1 << 4)
#define MAPPING_IS_VALID	(1 << 5)

struct mmu_mapping {

	unsigned int vaddr;
	unsigned int paddr;
	char metadata;
	// x  x  x  x    x  x  x  x
	//       |  |    |  |  |  |-> ITLB mapping
	//       |  |    |  |  |-> DTLB mapping
	//       |  |    |  |->  CAN_READ
	//       |  |    |-> CAN_WRITE
	//       |  |-> COPY_ON_WRITE: Not Implemented Yet
	//       |-> MAPPING_IS_VALID
};

#define enable_dtlb() do { \
	asm volatile	("xor r11, r11, r11\n\t" \
			 "ori r11, r11, 0x11\n\t" \
			 "wcsr tlbctrl, r11\n\t" \
			 "xor r0, r0, r0":::"r11"); \
} while(0);

#define disable_dtlb() do { \
	asm volatile	("xor r11, r11, r11\n\t" \
			 "ori r11, r11, 0x9\n\t" \
			 "wcsr tlbctrl, r11\n\t" \
			 "xor r0, r0, r0\n\t" \
			 "xor r0, r0, r0\n\t" \
			 "xor r0, r0, r0":::"r11"); \
} while(0);

#define enable_itlb() do { \
	asm volatile	("xor r11, r11, r11\n\t" \
			 "ori r11, r11, 0x10\n\t" \
			 "wcsr tlbctrl, r11\n\t" \
			 "xor r0, r0, r0\n\t" \
			 "xor r0, r0, r0\n\t" \
			 "xor r0, r0, r0\n\t" \
			 "xor r0, r0, r0\n\t":::"r11"); \
} while(0);

#define LM32_CSR_PSW_ITLBE	"(0x8)"

#define disable_itlb() do { \
	asm volatile ("rcsr r11, PSW\n\t" \
		      "mvi r10, ~(0x8)\n\t" \
		      "and r11, r11, r10\n\t" \
		      "wcsr PSW, r11\n\t" ::: "r10", "r11"); \
} while (0);

// FIXME : We MUST replace the following macro with a function which
// enables ITLB using two different methods depending on whether
// branch will be predicted as taken or non-taken

#define call_function_with_itlb_enabled(function) do { \
	asm volatile	("rcsr r11, PSW\n\t" \
			 "ori r11, r11, 0x8\n\t" \
			 "wcsr psw, r11\n\t" \
			 "call %0\n\t" :: "r"(function) : "r11"); \
} while (0);

#define call_function_with_itlb_disabled(function) do { \
	asm volatile	("rcsr r11, PSW\n\t" \
			 "mvi r10, ~(0x8)\n\t" \
			 "and r11, r11, r10\n\t" \
			 "wcsr psw, r11\n\t" \
			 "call %0\n\t" :: "r"(function) : "r10", "r11"); \
} while (0);

void mmu_dtlb_map(unsigned int vpfn, unsigned int pfn);
unsigned int read_word_with_mmu_enabled(unsigned int vaddr);
void mmu_dtlb_invalidate_line(unsigned int vaddr);
void mmu_itlb_invalidate_line(unsigned int vaddr);
void mmu_dtlb_invalidate(void);

#endif
