#include <pmm_k.h>
#include <memmap_k.h>
#include <hhdm_k.h>
#include <serialio_k.h>
#include <stddef.h>

static uint8_t *bitmap_ptr = NULL;
static uint64_t bitmap_size;
static uint64_t total_frames;
static uint64_t bitmap_start;
static uint64_t bitmap_end;
static const struct limine_memmap_response *pmm_memmap_response;

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

/*find highest address for bitmap*/
static uint64_t find_highest_addr(const struct limine_memmap_response *memmap_response) {
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

	return highest_addr;
}

/* find region for bitmap to fit bitmap_size*/
static uint64_t find_bitmap_region(const struct limine_memmap_response *memmap_response,
		uint64_t req_size) {
	uint64_t bitmap_phys = 0;

	for(uint64_t i = 0; i < memmap_response->entry_count; i++) {
		if((memmap_response->entries[i]->type == LIMINE_MEMMAP_USABLE) &&
			(memmap_response->entries[i]->base >= PMM_SKIP_LOWER) &&
			(memmap_response->entries[i]->length >= req_size))
		{
			bitmap_phys = memmap_response->entries[i]->base;

			break;
		}
	}

	return bitmap_phys;
}

/* init bitmap: occupy all memory, free usable memory, lock bitmap */
static void init_bitmap(const struct limine_memmap_response *memmap_response) {
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

	for(uint64_t frame = bitmap_start; frame < bitmap_end; frame++) {
		pmm_setb(frame);
	}
}

/* return a single free frame and reserve it */
uint64_t pmm_alloc_frame(void) {
	for(uint64_t frame = (PMM_SKIP_LOWER / PMM_FRAME_SIZE); frame < total_frames; frame++) {
		if(pmm_getb(frame) == PMM_FRAME_FREE) {
			pmm_setb(frame);
			/* note: phys addr */
			return frame * PMM_FRAME_SIZE;
		}
	}

	return 0;
}

static int8_t pmm_is_usable(uint64_t addr) {
	for(uint64_t i = 0; i < pmm_memmap_response->entry_count; i++) {
		struct limine_memmap_entry *entry = pmm_memmap_response->entries[i];

		if((entry->type == LIMINE_MEMMAP_USABLE) && ((entry->base <= addr) && 
					(addr < (entry->base + entry->length)))) {
			return 1;
		}
	}

	return 0;
}

int8_t pmm_free_frame(uint64_t addr) {
	uint64_t frame = addr / PMM_FRAME_SIZE;

	if((frame >= total_frames) || (addr % PMM_FRAME_SIZE != 0) ||
		(addr < PMM_SKIP_LOWER) || (pmm_is_usable(addr) == 0)) {
		/* something is faulty. either frame beyond frame limits, addr is not
		 * correctly aligned or not limine_usable type*/
		return 1;
	}

	if((frame >= bitmap_start) && (frame < bitmap_end)) {
		/* bitmap region is protected and should not be freed by anything */
		return 3;
	}

	if(pmm_getb(frame) == PMM_FRAME_USED) {
		pmm_delb(frame);
		return 0;
	}

	/* something weird e.g. double-free happened */
	return 2;
}

int8_t pmm_init(void) {
	if((memmap_request.response == NULL) || (hhdm_request.response == NULL)) {
		/* memmap or hhdm faulty */
		return 1;
	}

	uint64_t hhdm_offset = hhdm_request.response->offset;
	pmm_memmap_response = memmap_request.response;

	uint64_t highest_addr = find_highest_addr(pmm_memmap_response);

	total_frames = highest_addr / PMM_FRAME_SIZE;
	bitmap_size = (total_frames + 7) / 8;

	uint64_t bitmap_phys = find_bitmap_region(pmm_memmap_response, bitmap_size);

	if(bitmap_phys == 0) {
		/* no memory for bitmap found */
		return 2;
	}

	bitmap_start = bitmap_phys / PMM_FRAME_SIZE;
	bitmap_end = (bitmap_phys + bitmap_size + PMM_FRAME_SIZE - 1) / PMM_FRAME_SIZE;

	bitmap_ptr = (uint8_t *)(hhdm_offset + bitmap_phys);

	init_bitmap(pmm_memmap_response);

	return 0;
}
