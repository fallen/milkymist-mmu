#include <hal/mmu.h>
#include <base/mmu.h>

void dtlb_exception_handling_tests() {

	register unsigned int stack, addr;
	unsigned int data;
	int ret;

	asm volatile("mv %0, sp" : "=r"(stack) :: );

	ret = mmu_map(stack, stack, DTLB_MAPPING | MAPPING_CAN_READ | MAPPING_CAN_WRITE);
	check_for_error(ret);

	ret = mmu_map(stack-0x1000, stack-0x1000, DTLB_MAPPING | MAPPING_CAN_READ | MAPPING_CAN_WRITE);
	check_for_error(ret);

	printf("stack == 0x%08X\n", stack);

	addr = 0x44004004;

	printf("\n=> Mapping 0x%08X to 0x%08X\n", addr, addr);
	ret = mmu_map(addr, addr, DTLB_MAPPING | MAPPING_CAN_READ | MAPPING_CAN_WRITE);
	check_for_error(ret);

	data = 42;
	printf("=> Writing %d to physical address 0x%08X\n", data, addr);
	*(unsigned int *)addr = data;

	printf("=> Activating the MMU and reading form virtual address 0x%08X\n", addr);
	data = read_word_with_mmu_enabled(addr);
	printf("\n<= Reading %d from virtual address 0x%08X\n\n", data, addr);

	printf("=> Invalidating the mapping of virtual address 0x%08X in the TLB\n", addr);
	mmu_dtlb_invalidate(addr);

	data = 43;
	printf("=> Writing %d to physical address 0x%08X\n", data, addr);
	*(unsigned int *)addr = data;

	printf("=> Activating the MMU and reading form virtual address 0x%08X\n", addr);
	data = read_word_with_mmu_enabled(addr);
	printf("\n<= Reading %d from virtual address 0x%08X\n\n", data, addr);

	printf("=> Mapping 0x%08X to 0%08X\n", addr, addr+0x1000);
	ret = mmu_map(addr, addr+0x1000, DTLB_MAPPING | MAPPING_CAN_READ | MAPPING_CAN_WRITE); // Map to something else
	check_for_error(ret);

	printf("=> Invalidating the mapping of virtual address 0x%08X in the TLB\n", addr);
	mmu_dtlb_invalidate(addr); // AND invalidate the mapping

	data = 44;
	printf("=> Writting %d to physical address 0x%08X\n", data, addr+0x1000);
	*(unsigned int *)(addr + 0x1000) = data;

	printf("=> Activating the MMU and reading form virtual address 0x%08X\n", addr);
	data = read_word_with_mmu_enabled(addr);
	printf("\n<= Reading %d from virtual address 0x%08X\n\n", data, addr);

}
