#ifndef _HDDM_K_H
#define _HDDM_K_H

#include "limine.h"

__attribute__((used, section(".limine_requests")))
extern volatile struct limine_hhdm_request hhdm_request;

#endif
