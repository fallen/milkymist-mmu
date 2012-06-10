#ifndef __BASE_MMU_H__
#define __BASE_MMU_H__

#include <hal/mmu.h>

unsigned int mmu_map(unsigned int vaddr, unsigned int paddr, char metadata);
unsigned int get_mmu_mapping_for(unsigned int vaddr);
unsigned char remove_mmu_mapping_for(unsigned int vaddr);
void panic(void);
void check_for_error(int ret);
unsigned int read_word_with_mmu_enabled(unsigned int vaddr);

#endif
