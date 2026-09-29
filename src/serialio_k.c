#include <serialio_k.h>

void outb(uint16_t port, uint8_t value) {
	__asm__ volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port) {
	uint8_t result;
	__asm__ volatile("inb %w1, %b0" : "=a"(result) : "Nd"(port) : "memory");

	return result;
}

int8_t serial_init(void) {
	outb(PORT + 1, 0x00); /* disable interrupts */
	outb(PORT + 3, 0x80); /* enable DLAB */
	outb(PORT + 0, 0x03); /* low byte of baud rate divisor (to get 38400 baud) */
	outb(PORT + 1, 0x00); /* high byte of baud rate divisor */
	outb(PORT + 3, 0x03); /* line control register 8N1 */
	outb(PORT + 2, 0xC7); /* FIFO enable, clear, 14-byte treshold */
	outb(PORT + 4, 0x1E); /* MCR set RTS, OUT1, OUT2, Loop (for test) */

	outb(PORT + 0, 0xCD); /* set test byte */

	if(inb(PORT + 0) != 0xCD) {
		return 1;
	}

	outb(PORT + 4, 0x0F); /* MCR normal mode, disable loop*/

	return 0;
}

void serial_putc(const char c) {
	while((inb(PORT + 5) & 0x20) == 0) {
		/* 0 = TX reg is busy -> don't send! */
	}

	outb(PORT + 0, c);
}

void serial_puts(const char *s) {
	for (const char *sP = s; *sP != '\0'; ++sP) {
		if(*sP == '\n') {
			serial_putc('\r');
		}
		serial_putc(*sP);
	}
}

void serial_put_hex64(uint64_t value) {
	static const char hex_chars[] = "0123456789ABCDEF";

	serial_putc('0');
	serial_putc('x');

	for (int shift = 60; shift >= 0; shift -= 4) {
		uint8_t nibble = (value >> shift) & 0xF;
		serial_putc(hex_chars[nibble]);
	}
}
