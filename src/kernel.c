#include "limine.h"
#include <stddef.h>

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST_ID,
	.revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_request_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
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
	if(!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {hcf();}

	if((framebuffer_request.response == NULL) || 
		(framebuffer_request.response->framebuffer_count < 1)) {hcf();}

	hcf();
}
