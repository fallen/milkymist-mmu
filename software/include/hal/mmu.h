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
#define MAX_MMU_SLOTS	10
#define NO_EMPTY_SLOT	(MAX_MMU_SLOTS + 1)
#define A_BAD_ADDR	0xffffffff
#define NULL 		(0)
#define get_pfn(x)	(x & ~(PAGE_SIZE - 1))

struct mmu_mapping {

	unsigned int vaddr;
	unsigned int paddr;
	char valid;

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

void mmu_dtlb_map(unsigned int vpfn, unsigned int pfn);

#endif
