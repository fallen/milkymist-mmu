#include "main.h"

#define PRINTF_ZEROPAD	1		/* pad with zero */
#define PRINTF_SIGN	2		/* unsigned/signed long */
#define PRINTF_PLUS	4		/* show plus */
#define PRINTF_SPACE	8		/* space if plus */
#define PRINTF_LEFT	16		/* left justified */
#define PRINTF_SPECIAL	32		/* 0x */
#define PRINTF_LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

typedef int ptrdiff_t;

#define NULL ((void *)0)

#define likely(x) x
#define unlikely(x) x

#define abs(x) ((x) > 0 ? (x) : -(x))

static inline int isdigit(char c)
{
	return (c >= '0') && (c <= '9');
}

static inline int isxdigit(char c)
{
	return isdigit(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}

static inline int isupper(char c)
{
	return (c >= 'A') && (c <= 'Z');
}

static inline int islower(char c)
{
	return (c >= 'a') && (c <= 'z');
}

static inline unsigned char tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A'-'a';
	return c;
}

static inline unsigned char toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}

static inline char isspace(unsigned char c)
{
	if(c == ' '
		|| c == '\f'
		|| c == '\n'
		|| c == '\r'
		|| c == '\t'
		|| c == '\v')
		return 1;

	return 0;
}

int skip_atoi(const char **s)
{
	int i=0;

	while (isdigit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

size_t strnlen(const char *s, size_t count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

char *number(char *buf, char *end, unsigned long num, int base, int size, int precision, int type)
{
	char c,sign,tmp[66];
	const char *digits;
	static const char small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	static const char large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	digits = (type & PRINTF_LARGE) ? large_digits : small_digits;
	if (type & PRINTF_LEFT)
		type &= ~PRINTF_ZEROPAD;
	if (base < 2 || base > 36)
		return NULL;
	c = (type & PRINTF_ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & PRINTF_SIGN) {
		if ((signed long) num < 0) {
			sign = '-';
			num = - (signed long) num;
			size--;
		} else if (type & PRINTF_PLUS) {
			sign = '+';
			size--;
		} else if (type & PRINTF_SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & PRINTF_SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0) {
		tmp[i++] = digits[num % base];
		num = num / base;
	}
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(PRINTF_ZEROPAD+PRINTF_LEFT))) {
		while(size-->0) {
			if (buf < end)
				*buf = ' ';
			++buf;
		}
	}
	if (sign) {
		if (buf < end)
			*buf = sign;
		++buf;
	}
	if (type & PRINTF_SPECIAL) {
		if (base==8) {
			if (buf < end)
				*buf = '0';
			++buf;
		} else if (base==16) {
			if (buf < end)
				*buf = '0';
			++buf;
			if (buf < end)
				*buf = digits[33];
			++buf;
		}
	}
	if (!(type & PRINTF_LEFT)) {
		while (size-- > 0) {
			if (buf < end)
				*buf = c;
			++buf;
		}
	}
	while (i < precision--) {
		if (buf < end)
			*buf = '0';
		++buf;
	}
	while (i-- > 0) {
		if (buf < end)
			*buf = tmp[i];
		++buf;
	}
	while (size-- > 0) {
		if (buf < end)
			*buf = ' ';
		++buf;
	}
	return buf;
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int len;
	unsigned long long num;
	int i, base;
	char *str, *end, c;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */
				/* 'z' support added 23/7/1999 S.H.    */
				/* 'z' changed to 'Z' --davidm 1/25/99 */
				/* 't' added for ptrdiff_t */

	/* Reject out-of-range values early.  Large positive sizes are
	   used for unknown buffer sizes. */
	if (unlikely((int) size < 0))
		return 0;

	str = buf;
	end = buf + size;

	/* Make sure end is always >= buf */
	if (end < buf) {
		end = ((void *)-1);
		size = end - buf;
	}

	for (; *fmt ; ++fmt) {
		if (*fmt != '%') {
			if (str < end)
				*str = *fmt;
			++str;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= PRINTF_LEFT; goto repeat;
				case '+': flags |= PRINTF_PLUS; goto repeat;
				case ' ': flags |= PRINTF_SPACE; goto repeat;
				case '#': flags |= PRINTF_SPECIAL; goto repeat;
				case '0': flags |= PRINTF_ZEROPAD; goto repeat;
			}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= PRINTF_LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;	
			if (isdigit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' ||
		    *fmt =='Z' || *fmt == 'z' || *fmt == 't') {
			qualifier = *fmt;
			++fmt;
			if (qualifier == 'l' && *fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
		}

		/* default base */
		base = 10;

		switch (*fmt) {
			case 'c':
				if (!(flags & PRINTF_LEFT)) {
					while (--field_width > 0) {
						if (str < end)
							*str = ' ';
						++str;
					}
				}
				c = (unsigned char) va_arg(args, int);
				if (str < end)
					*str = c;
				++str;
				while (--field_width > 0) {
					if (str < end)
						*str = ' ';
					++str;
				}
				continue;

			case 's':
				s = va_arg(args, char *);
				if (s == NULL)
					s = "<NULL>";

				len = strnlen(s, precision);

				if (!(flags & PRINTF_LEFT)) {
					while (len < field_width--) {
						if (str < end)
							*str = ' ';
						++str;
					}
				}
				for (i = 0; i < len; ++i) {
					if (str < end)
						*str = *s;
					++str; ++s;
				}
				while (len < field_width--) {
					if (str < end)
						*str = ' ';
					++str;
				}
				continue;

			case 'p':
				if (field_width == -1) {
					field_width = 2*sizeof(void *);
					flags |= PRINTF_ZEROPAD;
				}
				str = number(str, end,
						(unsigned long) va_arg(args, void *),
						16, field_width, precision, flags);
				continue;


			case 'n':
				/* FIXME:
				* What does C99 say about the overflow case here? */
				if (qualifier == 'l') {
					long * ip = va_arg(args, long *);
					*ip = (str - buf);
				} else if (qualifier == 'Z' || qualifier == 'z') {
					size_t * ip = va_arg(args, size_t *);
					*ip = (str - buf);
				} else {
					int * ip = va_arg(args, int *);
					*ip = (str - buf);
				}
				continue;

			case '%':
				if (str < end)
					*str = '%';
				++str;
				continue;

				/* integer number formats - set up the flags and "break" */
			case 'o':
				base = 8;
				break;

			case 'X':
				flags |= PRINTF_LARGE;
			case 'x':
				base = 16;
				break;

			case 'd':
			case 'i':
				flags |= PRINTF_SIGN;
			case 'u':
				break;

			default:
				if (str < end)
					*str = '%';
				++str;
				if (*fmt) {
					if (str < end)
						*str = *fmt;
					++str;
				} else {
					--fmt;
				}
				continue;
		}
		if (qualifier == 'L')
			num = va_arg(args, long long);
		else if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
			if (flags & PRINTF_SIGN)
				num = (signed long) num;
		} else if (qualifier == 'Z' || qualifier == 'z') {
			num = va_arg(args, size_t);
		} else if (qualifier == 't') {
			num = va_arg(args, ptrdiff_t);
		} else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & PRINTF_SIGN)
				num = (signed short) num;
		} else {
			num = va_arg(args, unsigned int);
			if (flags & PRINTF_SIGN)
				num = (signed int) num;
		}
		str = number(str, end, num, base,
				field_width, precision, flags);
	}
	if (size > 0) {
		if (str < end)
			*str = '\0';
		else
			end[-1] = '\0';
	}
	/* the trailing null byte doesn't count towards the total */
	return str-buf;
}
