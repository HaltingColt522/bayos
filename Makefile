CC := x86_64-elf-gcc

CFLAGS := -ffreestanding \
		  -fno-stack-protector \
		  -fno-stack-check \
		  -fno-pic \
		  -fno-pie \
		  -fno-tree-loop-distribute-patterns \
		  -mno-red-zone \
		  -mno-80387 \
		  -mcmodel=kernel \
		  -mgeneral-regs-only \
		  -Isrc/include \
		  -Wall \
		  -Wextra \
		  -std=c11

LDFLAGS := -nostdlib \
		   -static \
		   -z max-page-size=0x1000 \
		   -Wl,--build-id=none \
		   -T src/linker.ld


BUILD_DIR := build

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

SRCS := $(wildcard src/*.c)

OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(OBJS) src/linker.ld
	$(CC) $(LDFLAGS) $(OBJS) -o $(BUILD_DIR)/kernel.elf

limine-binary/limine:
	$(MAKE) -C limine-binary

$(BUILD_DIR)/image.iso: $(BUILD_DIR)/kernel.elf limine.conf limine-binary/limine | $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/iso_root/boot
	cp $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/iso_root/boot/
	cp limine.conf $(BUILD_DIR)/iso_root/boot/
	cp limine-binary/limine-bios-cd.bin $(BUILD_DIR)/iso_root/boot/
	cp limine-binary/limine-bios.sys $(BUILD_DIR)/iso_root/boot/
	xorriso -as mkisofs -R -r -J -b boot/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		$(BUILD_DIR)/iso_root -o $(BUILD_DIR)/image.iso
	./limine-binary/limine bios-install $(BUILD_DIR)/image.iso --force

run: $(BUILD_DIR)/image.iso
	qemu-system-x86_64 -cdrom $(BUILD_DIR)/image.iso \
		-serial stdio

clean:
	rm -rf $(BUILD_DIR)

.PHONY: run clean
