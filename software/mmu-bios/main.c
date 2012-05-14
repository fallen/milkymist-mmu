#include "main.h"


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

int main(int argc, char **argv)
{
	asm volatile("wcsr IE, r0");
//	asm volatile("scall");
	dtlb_load_test();

	while (1)
	{
		asm volatile("xor r0, r0, r0");
	}

	return 0;
}
