#ifndef _PMM_K_H
#define _PMM_K_H

#include <stdint.h>

#define PMM_FRAME_SIZE 0x1000ULL
#define PMM_FRAME_FREE 0
#define PMM_FRAME_USED 1
#define PMM_SKIP_LOWER 0x100000ULL

int8_t pmm_init(void);
uint64_t pmm_alloc_frame(void);
int8_t pmm_free_frame(uint64_t addr);

#endif
