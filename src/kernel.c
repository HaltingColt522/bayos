#include "limine.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST_ID,
	.revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_request_start_marker[] = LIMINE_REQUEST_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_request_end_marker[] = LIMINE_REQUEST_END_MARKER;
