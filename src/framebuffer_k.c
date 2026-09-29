#include <framebuffer_k.h>

__attribute__((used, section(".limine_requests")))
volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST_ID,
	.revision = 0
};

void *getPointAddressFromFramebuffer(struct limine_framebuffer *fb,uint64_t x, uint64_t y) {
	uint8_t *fb_bytes = (uint8_t *) fb->address;
	return (void *)(fb_bytes + (y * fb->pitch) + (x * (fb->bpp / 8)));
}
