#include <hal/mmu.h>

void dtlb_miss_handler(void)
{
	mmu_dtlb_map(0x44000000, 0x44003000);
}
