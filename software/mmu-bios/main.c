#include "main.h"
#include <base/mmu.h>

extern void call_function_with_itlb_enabled(void (*f)(void));

void uart_write(char c)
{
	unsigned int oldmask;
	
	oldmask = irq_getmask();
	irq_setmask(0);

	CSR_UART_RXTX = c;
	irq_setmask(oldmask);
}

static void writechar(char c)
{
	uart_write(c);
}

int puts(const char *s)
{
	unsigned int oldmask;

	oldmask = irq_getmask();
	irq_setmask(IRQ_UART); // HACK: prevent UART data loss

	while(*s) {
		writechar(*s);
		s++;
	}
	writechar('\n');
	
	irq_setmask(oldmask);
	return 1;
}

void putsnonl(const char *s)
{
	unsigned int oldmask;

	oldmask = irq_getmask();
	irq_setmask(IRQ_UART); // HACK: prevent UART data loss
	
	while(*s) {
		writechar(*s);
		s++;
	}
	
	irq_setmask(oldmask);
}

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int i;

	i=vsnprintf(buf,size,fmt,args);
	return (i >= size) ? (size - 1) : i;
}

int printf(const char *fmt, ...)
{
	va_list args;
	int len;
	char outbuf[256];

	va_start(args, fmt);
	len = vscnprintf(outbuf, sizeof(outbuf), fmt, args);
	va_end(args);
	outbuf[len] = 0;
	putsnonl(outbuf);

	return len;
}

void f(void) {
	char c = '@';
	CSR_UART_RXTX = c;
//	asm volatile("bi f" ::: ); // We intinitely loop to f()
	asm volatile("xor r0, r0, r0\n\t"
		     "xor r0, r0, r0" ::: );
}

void itlbtest(void) {
	register unsigned int stack, f_addr;
	unsigned int *p;
	unsigned int *pdest;

	asm volatile("mv %0, sp" : "=r"(stack) :: );
	printf("stack == 0x%08X\n", stack);

	printf("f() is located at 0x%p\n", f);

	f_addr = 0x44004000;
	printf("Mapping f() into virtual memory at 0x%08X [physical == 0x%08X]\n", f_addr, f_addr+0x1000);

	mmu_map(0x44003000, 0x44003000, DTLB_MAPPING | MAPPING_CAN_READ | MAPPING_CAN_WRITE);
	mmu_map(stack, stack, DTLB_MAPPING | MAPPING_CAN_READ | MAPPING_CAN_WRITE);
	mmu_map(f_addr, f_addr + 0x1000, ITLB_MAPPING | MAPPING_CAN_READ);
	mmu_map(itlbtest, itlbtest, ITLB_MAPPING | MAPPING_CAN_READ);
	mmu_map(call_function_with_itlb_enabled, call_function_with_itlb_enabled, ITLB_MAPPING | MAPPING_CAN_READ);
	puts("Mapping DONE");

	mmu_itlb_invalidate(f_addr);

	// We copy f's code to 0x44005000
	for (p = f, pdest = 0x44005000 ; p < f + 0x1000 ; p++, pdest++)
		*pdest = *p;
	puts("Copy DONE");

	asm volatile("wcsr DCC, r0\n\t"
		     "xor r0, r0, r0\n\t"
		     "xor r0, r0, r0\n\t"
		     "xor r0, r0, r0\n\t"
		     "xor r0, r0, r0");
	asm volatile("wcsr ICC, r0\n\t"
		     "xor r0, r0, r0");
	puts("Instruction and Data caches have been invalidated");

	call_function_with_itlb_enabled(f_addr);
	disable_dtlb();
	disable_itlb();
	puts("Call DONE");
	while(1)
		asm volatile("xor r0, r0, r0");
}

int main(int argc, char **argv)
{
	asm volatile("wcsr IE, r0");
//	dtlb_load_test();
//	dtlb_exception_handling_tests();
	itlbtest();

	while (1)
	{
		asm volatile("xor r0, r0, r0");
	}

	return 0;
}
