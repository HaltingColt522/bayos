#ifndef _MEMORY_K_H
#define _MEMORY_K_H

#include <stddef.h>
#include <stdint.h>

void *memset(void *dest, int ch, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
void *memmove(void *dest, const void *src, size_t count);
int memcmp(const void *lhs, const void *rhs, size_t count);

#endif
