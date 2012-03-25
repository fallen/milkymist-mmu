typedef int size_t;
#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 4))
#define va_start(v,l) __builtin_va_start((v),l)
#else
#define va_start(v,l) __builtin_stdarg_start((v),l)
#endif

#define va_arg(ap, type) \
	__builtin_va_arg((ap), type)

#define va_end(ap) \
	__builtin_va_end(ap)

#define va_list \
	__builtin_va_list

#define unlikely(x) x

#define IRQ_UART	(0x00000001)
#define CSR_UART_RXTX *((volatile unsigned int *)0x44003000)

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
