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

#ifndef __BASE_MMU_H__
#define __BASE_MMU_H__

#include <hal/mmu.h>

unsigned int mmu_map(unsigned int vaddr, unsigned int paddr, char metadata);
unsigned int get_mmu_mapping_for(unsigned int vaddr);
unsigned char remove_mmu_mapping_for(unsigned int vaddr);
unsigned char is_dtlb_mapping(unsigned int vaddr);
unsigned char is_itlb_mapping(unsigned int vaddr);
void panic(void);
void check_for_error(int ret);

#endif
