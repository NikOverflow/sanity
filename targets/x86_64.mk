ARCH_SOURCE_FILES := $(call rwildcard,arch/x86_64/*.c) $(call rwildcard,arch/x86_64/*.asm) $(call rwildcard,arch/x86_64/*.S)
QEMU := qemu-system-x86_64

CFLAGS += -target x86_64-unknown-elf -I arch/x86_64/include/
