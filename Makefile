rwildcard = $(filter $(subst *,%,$(notdir $1)),$(wildcard $(dir $1)*)) $(foreach d,$(wildcard $(dir $1)*/),$(call rwildcard,$d$(notdir $1)))

ARCH ?= x86_64

BUILD := build
KERNEL_FILE := $(BUILD)/sanity-$(ARCH).elf
DEBUG_KERNEL_FILE := $(BUILD)/sanity-$(ARCH)-debug.elf
ISO_FILE := $(BUILD)/sanity-$(ARCH).iso

CC := clang
CFLAGS := -std=c17 -ffreestanding -nostdlibinc -MMD -MP -g -fno-common -fno-stack-protector -fPIE -ffunction-sections -fdata-sections -Wall -Wextra -Werror -DARCHITECTURE=$(ARCH) -I include/ -isystem limine/

CLANG_TIDY := clang-tidy
CLANG_TIDY_FLAGS := --quiet

# TODO: change to lld (my llvm toolchain is half broken)
LD := ld
LDFLAGS := --gc-sections --entry limine_main -pie --no-dynamic-linker -T targets/$(ARCH).ld

# TODO: change to llvm-objcopy (my llvm toolchain is half broken)
OBJCOPY := objcopy
OBJCOPY_FLAGS := --strip-all

QEMU_FLAGS := -m 512M -drive if=pflash,format=raw,readonly=on,file=limine/code.fd -drive if=pflash,format=raw,file=limine/vars.fd

GDB := gdb
GDB_FLAGS := -q -se $(DEBUG_KERNEL_FILE) -ex "target remote localhost:1234"

include targets/$(ARCH).mk

SOURCE_C_FILES := $(call rwildcard,bootloader/*.c) $(call rwildcard,kernel/*.c) $(filter %.c,$(ARCH_SOURCE_FILES))
SOURCE_STUB_FILES := $(filter %.S,$(ARCH_SOURCE_FILES))

OBJECT_C_FILES := $(patsubst %, $(BUILD)/%.o, $(SOURCE_C_FILES))
OBJECT_STUB_FILES := $(patsubst %, $(BUILD)/%.o, $(SOURCE_STUB_FILES))
OBJECT_FILES := $(OBJECT_C_FILES) $(OBJECT_STUB_FILES) $(OBJECT_ASM_FILES)

-include $(OBJECT_FILES:.o=.d)

.PHONY: all build iso run debug lint clang clean

all: iso

build: $(KERNEL_FILE)

iso: $(ISO_FILE)

run: $(ISO_FILE)
	$(QEMU) $(QEMU_FLAGS) -cdrom $<

debug: $(ISO_FILE)
	setsid $(QEMU) $(QEMU_FLAGS) -s -S -cdrom $< & \
	PID=$$!; \
	$(GDB) $(GDB_FLAGS); \
	kill -9 $$PID

lint:
	$(CLANG_TIDY) $(CLANG_TIDY_FLAGS) $(SOURCE_C_FILES) -- $(CFLAGS)

clang:
	$(file > compile_flags.txt)
	$(foreach flag,$(CFLAGS),$(file >> compile_flags.txt,$(flag)))

clean:
	@rm -rf $(BUILD)

$(ISO_FILE): $(KERNEL_FILE)
	@mkdir -p $(BUILD)/iso_root/boot/limine/
	@mkdir -p $(BUILD)/iso_root/EFI/BOOT/
	cp $< $(BUILD)/iso_root/boot/
	cp bootloader/limine.conf $(BUILD)/iso_root/boot/limine/
	cp limine/limine-uefi-cd.bin $(BUILD)/iso_root/boot/limine/
	cp limine/BOOTAA64.EFI $(BUILD)/iso_root/EFI/BOOT/
	xorriso -as mkisofs --efi-boot boot/limine/limine-uefi-cd.bin -o $@ $(BUILD)/iso_root/

$(OBJECT_C_FILES): $(BUILD)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_STUB_FILES): $(BUILD)/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEBUG_KERNEL_FILE): $(OBJECT_FILES)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ $(OBJECT_FILES)

$(KERNEL_FILE): $(DEBUG_KERNEL_FILE)
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@
