#include <limine.h>
#include <framebuffer_k.h>
#include <memmap_k.h>
#include <hhdm_k.h>
#include <serialio_k.h>
#include <stddef.h>
#include <pmm_k.h>
#include <printf_k.h>

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_request_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_request_end_marker[] = LIMINE_REQUESTS_END_MARKER;

/* halt and catch fire */
__attribute__((noreturn))
static void hcf() {
	for(;;) {
		__asm__ volatile("hlt");
	}
}

__attribute__((noreturn))
void kmain() {
	if(!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) { hcf(); }

	if((framebuffer_request.response == NULL) || 
		(framebuffer_request.response->framebuffer_count < 1)) { hcf(); }

	if((memmap_request.response == NULL) ||
		(memmap_request.response->entry_count < 1)) { hcf(); }

	if(hhdm_request.response == NULL) { hcf(); }

	serial_init();

	if(pmm_init() != 0) { hcf(); }

	struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

	for (uint64_t y = 100; y < 150; y++) {
		for (uint64_t x = 100; x < 150; x++) {
			uint32_t *pixel = (uint32_t *) getPointAddressFromFramebuffer(fb, x, y);
			*pixel = 0x00FF0000;
		}
	}

	kprintf("Hello world!\n");

	hcf();
}
