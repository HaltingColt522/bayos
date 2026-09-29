#ifndef _FRAMEBUFFER_K_H
#define _FRAMEBUFFER_K_H

#include "limine.h"

__attribute__((used, section(".limine_requests")))
extern volatile struct limine_framebuffer_request framebuffer_request;

void *getPointAddressFromFramebuffer(struct limine_framebuffer *fb,uint64_t x, uint64_t y);

#endif
