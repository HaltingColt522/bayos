#include <memory_k.h>

void *memset(void *dest, int ch, size_t count) {
	uint8_t *destP = (uint8_t *)dest;
	uint8_t *end = destP + count;

	for(; destP < end; destP++) {
		*destP = (uint8_t)ch;
	}

	return dest;
}

void *memcpy(void *dest, const void *src, size_t count) {
	const uint8_t *srcP = (const uint8_t *)src;
	uint8_t *destP = (uint8_t *)dest;
	const uint8_t *end = srcP + count;

	for(; srcP < end; srcP++, destP++) {
		*destP = *srcP;
	}

	return dest;
}

void *memmove(void *dest, const void *src, size_t count) {
	uint8_t *destP = (uint8_t *)dest;
	const uint8_t *srcP = (const uint8_t *)src;
	const uint8_t *end = srcP + count;

	if(destP < srcP) {
		for(; srcP < end; srcP++, destP++) {
			*destP = *srcP;
		}
	}else{
		while(count--) {
			destP[count] = srcP[count];
		}
	}

	return dest;
}

int memcmp(const void *lhs, const void *rhs, size_t count) {
	const uint8_t *lhsP = (const uint8_t *)lhs;
	const uint8_t *rhsP = (const uint8_t *)rhs;

	for(size_t i = 0; i < count; i++) {
		if(lhsP[i] != rhsP[i]) {
			return (lhsP[i] - rhsP[i]);
		}
	}

	return 0;
}
