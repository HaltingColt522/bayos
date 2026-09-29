#ifndef _MEMMAP_K_H
#define _MEMMAP_K_H

#include "limine.h"

__attribute__((used, section(".limine_requests")))
extern volatile struct limine_memmap_request memmap_request;


#endif
