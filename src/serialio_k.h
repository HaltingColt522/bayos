#ifndef _SERIALIO_K_H
#define _SERIALIO_K_H

#include <stdint.h>

#define PORT 0x3F8 /* COM1 Port */

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
int8_t serial_init(void);
void serial_putc(const char c);
void serial_puts(const char *s);

#endif
