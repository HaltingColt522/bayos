#include <pmm_k.h>
#include <memmap_k.h>
#include <hhdm_k.h>
#include <serialio_k.h>
#include <stddef.h>

static uint8_t *bitmap_ptr = NULL;
static uint64_t bitmap_size;
static uint64_t total_frames;

static inline void pmm_setb(uint64_t frame) {
	uint64_t byte_index = frame / 8;
	uint64_t bit_index = frame % 8;
	uint8_t mask = (uint8_t)(1 << bit_index);

	bitmap_ptr[byte_index] |= mask;
}
static inline void pmm_delb(uint64_t frame) {
	uint64_t byte_index = frame / 8;
	uint64_t bit_index = frame % 8;
	uint8_t mask = (uint8_t)(1 << bit_index);

	bitmap_ptr[byte_index]&=~mask;
}
static inline uint8_t pmm_getb(uint64_t frame) {
	uint64_t byte_index = frame / 8;
	uint64_t bit_index = frame % 8;
	uint8_t mask = (uint8_t)(1 << bit_index);

	return (bitmap_ptr[byte_index]&mask) != 0;
}

int8_t pmm_init(void) {
	if((memmap_request.response == NULL) || (hhdm_request.response == NULL)) {
		/* memmap or hhdm faulty */
		return 1;
	}

	uint64_t hhdm_offset = hhdm_request.response->offset;
	struct limine_memmap_response *memmap_response = memmap_request.response;

	/* find highest address for bitmap*/
	uint64_t highest_addr = 0x00;
	for(uint64_t i = 0; i < memmap_response->entry_count; i++) {
		if((memmap_response->entries[i]->type == LIMINE_MEMMAP_USABLE) &&
			(((memmap_response->entries)[i]->base +
			 (memmap_response->entries)[i]->length) > highest_addr))
		{
			highest_addr = (memmap_response->entries)[i]->base +
							(memmap_response->entries)[i]->length;
		}
	}

	total_frames = highest_addr / PMM_FRAME_SIZE;
	bitmap_size = (total_frames + 7) / 8;

	uint64_t bitmap_phys = 0;

	for(uint64_t i = 0; i < memmap_response->entry_count; i++) {
		if((memmap_response->entries[i]->type == LIMINE_MEMMAP_USABLE) &&
			(memmap_response->entries[i]->base >= PMM_SKIP_LOWER) &&
			(memmap_response->entries[i]->length >= bitmap_size))
		{
			bitmap_phys = memmap_response->entries[i]->base;
			bitmap_ptr = (uint8_t *)(hhdm_offset + bitmap_phys);
			break;
		}
	}

	if(bitmap_phys == 0) {
		/* no memory for bitmap found */
		return 2;
	}

	for(uint64_t i = 0; i < bitmap_size; i++) {
		bitmap_ptr[i] = 0xFF;
	}

	for(uint64_t i = 0; i < memmap_response->entry_count; i++) {
		struct limine_memmap_entry *entry = memmap_response->entries[i];

		if((entry->type == LIMINE_MEMMAP_USABLE) && (entry->base >= PMM_SKIP_LOWER)) {
			uint64_t first = entry->base / PMM_FRAME_SIZE;
			uint64_t last = (entry->base + entry->length) / PMM_FRAME_SIZE;
			for(uint64_t frame = first; frame < last; frame++) {
				pmm_delb(frame);
			}
		}
	}

	uint64_t bitmap_frames = (bitmap_size + PMM_FRAME_SIZE - 1) / PMM_FRAME_SIZE;
	uint64_t start = bitmap_phys / PMM_FRAME_SIZE;

	for(uint64_t frame = start; frame < start + bitmap_frames; frame++) {
		pmm_setb(frame);
	}

	return 0;
}
