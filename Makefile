ARCH ?= x86_64

CC := clang
CFLAGS := -std=c23 -ffreestanding -fno-stack-protector -nostdlibinc -Wall -Wextra -Werror -I include/ -I arch/$(ARCH)/include/

# TODO: change to lld (my llvm toolchain is half broken)
LD := ld
LDFLAGS := -T arch/$(ARCH)/linker.ld

NASM := nasm
NASM_FLAGS := -f elf64

BUILD := build
KERNEL_FILE := $(BUILD)/sanity-$(ARCH).elf
ISO_FILE := $(BUILD)/sanity-$(ARCH).iso

SOURCE_C_FILES := $(wildcard arch/$(ARCH)/*.c) $(wildcard bootloader/*.c) $(wildcard kernel/*.c)
SOURCE_STUB_FILES := $(wildcard arch/$(ARCH)/*.S)
SOURCE_ASM_FILES := $(wildcard arch/$(ARCH)/*.asm)

OBJECT_C_FILES := $(patsubst %, $(BUILD)/%.o, $(SOURCE_C_FILES))
OBJECT_STUB_FILES := $(patsubst %, $(BUILD)/%.o, $(SOURCE_STUB_FILES))
OBJECT_ASM_FILES := $(patsubst %, $(BUILD)/%.o, $(SOURCE_ASM_FILES))
OBJECT_FILES := $(OBJECT_C_FILES) $(OBJECT_STUB_FILES) $(OBJECT_ASM_FILES)

build: $(KERNEL_FILE)

iso: $(ISO_FILE)

run: $(ISO_FILE)
	qemu-system-$(ARCH) -cdrom $< -bios /usr/share/OVMF/x64/OVMF.4m.fd

clean:
	@rm -rf $(BUILD)

$(ISO_FILE): $(KERNEL_FILE)
	@mkdir -p $(BUILD)/iso_root/boot/limine/
	@mkdir -p $(BUILD)/iso_root/EFI/BOOT/
	cp $< $(BUILD)/iso_root/boot/
	cp bootloader/limine.conf $(BUILD)/iso_root/boot/limine/
	cp bootloader/limine/limine-uefi-cd.bin $(BUILD)/iso_root/boot/limine/
	cp bootloader/limine/BOOTAA64.EFI $(BUILD)/iso_root/EFI/BOOT/
	xorriso -as mkisofs --efi-boot boot/limine/limine-uefi-cd.bin -o $@ $(BUILD)/iso_root/

$(OBJECT_C_FILES): $(BUILD)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_STUB_FILES): $(BUILD)/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_ASM_FILES): $(BUILD)/%.asm.o: %.asm
	@mkdir -p $(@D)
	$(NASM) $(NASM_FLAGS) -o $@ $<

$(KERNEL_FILE): $(OBJECT_FILES)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ $(OBJECT_FILES)
