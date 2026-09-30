#include <pmm_k.h>
#include <memmap_k.h>
#include <hhdm_k.h>
#include <serialio_k.h>
#include <stddef.h>

static uint8_t *bitmap_ptr = NULL;
static uint64_t bitmap_size;
static uint64_t total_frames;

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

	return 0;
}
