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

#define DTLB_CTRL_FLUSH_CMD	(0x3)
#define DTLB_CTRL_DISABLE_CMD	(0x9)
#define DTLB_CTRL_ENABLE_CMD	(0x11)
#define ITLB_CTRL_FLUSH_CMD	(0x2)
#define ITLB_CTRL_DISABLE_CMD	(0x8)
#define ITLB_CTRL_ENABLE_CMD	(0x10)

/* @vpfn : virtual page frame number
 * @pfn  : physical page frame number
 */
inline void mmu_dtlb_map(unsigned int vpfn, unsigned int pfn)
{

	asm volatile	("ori %0, %0, 1\n\t"
 			 "wcsr tlbvaddr, %0" :: "r"(vpfn) : );

	asm volatile	("ori %0, %0, 1\n\t"
			 "wcsr tlbpaddr, %0" :: "r"(pfn) : );
/*
	asm volatile	("xor r11, r11, r11\n\t"
			 "ori r11, r11, 0x5\n\t"
			 "wcsr tlbctrl, r11" ::: "r11"); */
}

inline void mmu_itlb_map(unsigned int vpfn, unsigned int pfn)
{

	asm volatile	("wcsr tlbvaddr, %0" :: "r"(get_pfn(vpfn)) : );

	asm volatile	("wcsr tlbpaddr, %0" :: "r"(get_pfn(pfn)) : );

/*	asm volatile	("xor r11, r11, r11\n\t"
			 "ori r11, r11, 0x4\n\t"
			 "wcsr tlbctrl, r11" ::: "r11"); */
}

inline void mmu_dtlb_invalidate_line(unsigned int vaddr)
{
	vaddr |= 0x21;
	asm volatile ("wcsr tlbvaddr, %0" :: "r"(vaddr) : );
}

inline void mmu_itlb_invalidate_line(unsigned int vaddr)
{
	vaddr |= 0x20;
	asm volatile ("wcsr tlbvaddr, %0" :: "r"(vaddr) : );
}

inline void mmu_dtlb_invalidate(void)
{
//	register unsigned int cmd = DTLB_CTRL_FLUSH_CMD;
//	asm volatile("wcsr tlbctrl, %0" :: "r"(cmd) : );
	asm volatile("xor r11, r11, r11\n\t"
		     "ori r11, r11, 0x3\n\t"
		     "wcsr tlbvaddr, r11" ::: "r11");

}

inline void mmu_itlb_invalidate(void)
{
//	register unsigned int cmd = ITLB_CTRL_FLUSH_CMD;
//	asm volatile("wcsr tlbctrl, %0" :: "r"(cmd) : );
	asm volatile("xor r11, r11, r11\n\t"
		     "ori r11, r11, 0x2\n\t"
		     "wcsr tlbvaddr, r11" ::: "r11");

}

/* This function activates the MMU
 * then reads from virtual address "vaddr"
 * and store the result in temporary variable "data".
 * Then MMU is disactivated and the content of "data"
 * is returned.
 */

unsigned int read_word_with_mmu_enabled(unsigned int vaddr)
{
	register unsigned int data, cmd1, cmd2;

	cmd1  = DTLB_CTRL_ENABLE_CMD;
	cmd2 = DTLB_CTRL_DISABLE_CMD;

	asm volatile("rcsr r11, PSW\n\t"
		     "ori r11, r11, 64\n\t"
		     "wcsr PSW, r11\n\t" // Activates the MMU
		     //"wcsr tlbctrl, %2\n\t" // Activates the MMU
		     "xor r0, r0, r0\n\t"
		     "lw  %0, (%1+0)\n\t" // Reads from virtual address "addr"
		     "mvi r10, ~(64)\n\t"
		     "and r11, r11, r10\n\t"
		     "wcsr PSW, r11\n\t" // Disactivates the MMU
		     //"wcsr tlbctrl, %3\n\t" // Disactivates the MMU
		     "xor r0, r0, r0\n\t" : "=&r"(data) : "r"(vaddr)/*, "r"(cmd1), "r"(cmd2)*/ : "r11", "r10"
	);

	return data;
}

unsigned int write_word_with_mmu_enabled(register unsigned int vaddr, register unsigned int data)
{
	asm volatile(
		"rcsr r11, PSW\n\t"
		"ori r11, r11, 64\n\t"
		"wcsr PSW, r11\n\t" // Activates the MMU
		"xor r0, r0, r0\n\t"
		"sw  (%0 + 0), %1\n\t" // Reads from virtual address "addr"
		"mvi r10, ~(64)\n\t"
		"and r11, r11, r10\n\t"
		"wcsr PSW, r11\n\t" // Disactivates the MMU
		"xor r0, r0, r0\n\t" :: "r"(vaddr), "r"(data) : "r11", "r10"
	);
}
/*
inline void call_function_with_itlb_enabled(unsigned int f)
{
	asm volatile(
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x10\n\t"
		"wcsr tlbctrl, r11\n\t" // Activates ITLB
		"call %0\n\t"
		"xor r11, r11, r11\n\t"
		"ori r11, r11, 0x8\n\t"
		"wcsr tlbctrl, r11" :: "r"(f) : "r11" // Disactivates ITLB
	);
}*/
