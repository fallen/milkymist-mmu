#include <hal/mmu.h>

void dtlb_miss_handler(void) {

	disable_dtlb();
	printf("TOTO");

}
