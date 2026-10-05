#define KPRINTF_SERIAL

#include <printf_k.h>
#include <stdarg.h>

static void kput_uint(unsigned long v, unsigned base, int upper) {
	char buf[32];
	const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
	int i = 0;
	if(v == 0) { buf[i++] = '0'; }
	while(v) { buf[i++] = digits[v % base]; v /= base; }
	while(i--) { PUTC(buf[i]); }
}

int kprintf(const char *restrict format, ...) {
	va_list ap;
	va_start(ap, format);

	for(; *format; format++) {
		if(*format != '%') { PUTC(*format); continue; }
		format++;

		int is_long = 0;
		if(*format == 'l') { is_long = 1; format++; }

		switch(*format) {
			case '%':
				PUTC(*format);
				break;
			case 'c':
				PUTC((char)va_arg(ap, int));
				break;
			case 's': {
				const char *s = va_arg(ap, const char *);
				PUTS(s ? s : "(null)");
				break;
			}
			case 'd': {
				long v = is_long ? va_arg(ap, long) : va_arg(ap, int);
				unsigned long u;
				if(v < 0) { PUTC('-'); u = 0UL - (unsigned long)v; }
				else { u = (unsigned long)v; }
				kput_uint(u, 10, 0);
				break;
			}
			case 'u': kput_uint(is_long ? va_arg(ap, unsigned long) : va_arg(ap, unsigned), 10, 0); break;
			case 'x': kput_uint(is_long ? va_arg(ap, unsigned long) : va_arg(ap, unsigned), 16, 0); break;
			case 'X': kput_uint(is_long ? va_arg(ap, unsigned long) : va_arg(ap, unsigned), 16, 1); break;
			case 'p': PUTS("0x"); kput_uint((unsigned long)va_arg(ap, void *), 16, 0); break;
			case '\0': va_end(ap); return 0;
			default: PUTC('%'); PUTC(*format); break;
		}
	}

	va_end(ap);
	return 0;
}
