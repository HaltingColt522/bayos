#ifndef _PRINTF_K_H
#define _PRINTF_K_H

#include <serialio_k.h>

/* serial_putc does not convert '\n' to '\r\n' but only for PUTC()! serial_puts
 * convert '\n' itself */
static inline void serial_kputc(char c) {
	if(c == '\n') { serial_putc('\r'); }
	serial_putc(c);
}

#if defined(KPRINTF_SERIAL) && defined(KPRINTF_FB)
	#define PUTC(c) do { serial_kputc(c); } while(0)
	#define PUTS(s) do { serial_puts(s); } while(0)
#elif defined(KPRINTF_SERIAL)
	#define PUTC(c) serial_kputc(c)
	#define PUTS(s) serial_puts(s)
#elif defined(KPRINTF_FB)
	#define PUTC(c)
	#define PUTS(s)
#endif

int kprintf(const char *restrict format, ...);

#endif
