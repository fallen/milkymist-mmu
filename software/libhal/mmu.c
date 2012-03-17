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
