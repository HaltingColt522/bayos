#include <gdt_k.h>

struct __attribute__((packed)) gdt_gdtr {
	uint16_t limit; /* limit is actual size-1 */
	uint64_t offset;
};

struct __attribute__((packed)) gdt_seg_descriptor {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t acc_byte;
	uint8_t limit_high : 4;
	uint8_t flags : 4;
	uint8_t base_high;
};

static struct gdt_seg_descriptor generate_entry(uint8_t access, uint8_t flags) {
	return (struct gdt_seg_descriptor){
		.limit_low = 0x0FFFF,
		.base_low = 0,
		.base_middle = 0,
		.acc_byte = access,
		.limit_high = 0xF,
		.flags = flags,
		.base_high = 0
	};
}

static struct gdt_seg_descriptor gdt[3];
static struct gdt_gdtr gdtr;

_Static_assert(sizeof(struct gdt_seg_descriptor) == 8, "GDT Segment size faulty (!= 8)!");
_Static_assert(sizeof(struct gdt_gdtr) == 10, "GDTR size faulty (!= 10)!");

static void gdt_load(void) {
	/* load gdt */
	__asm__ volatile("lgdt %0" : : "m"(gdtr) : "memory");
}

static void gdt_reload_segments(void) {
	/* data */
	__asm__ volatile(
		"mov $0x10, %%ax\n\t"
		"mov %%ax, %%ds\n\t"
		"mov %%ax, %%es\n\t"
		"mov %%ax, %%ss\n\t"
		"mov %%ax, %%fs\n\t"
		"mov %%ax, %%gs"
		: /* no outputs */
		: /* no inputs */
		: "ax", "memory");

	/* code */
	__asm__ volatile(
		"pushq $0x08\n\t"
		"lea 1f(%%rip), %%rax\n\t"
		"pushq %%rax\n\t"
		"lretq\n\t"
		"1:"
		:
		:
		: "rax", "memory");
}

int8_t gdt_init(void) {
	/* gdt[0] is the null-descriptor. static is guaranteed to initialize all fields with 0 */
	gdt[1] = generate_entry(0x9B, 0xA);	/* kernel mode code segment */
	gdt[2] = generate_entry(0x93, 0xC); /* kernel mode data segment */

	gdtr = (struct gdt_gdtr){
		.limit = sizeof(gdt) - 1,
		.offset = (uint64_t)gdt,
	};

	gdt_load();
	gdt_reload_segments();

	return 0;
}
