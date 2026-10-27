#!/bin/sh

cd "$(dirname "$0")"
mkdir -p limine
cd limine/

wget "https://github.com/Limine-Bootloader/limine-protocol/raw/3a0526b700e356f0eac1b71a77697b3fd1c707a3/include/limine.h"
echo "2ca196c36857fe01aa26102839ac6089af005fd8419225236b284d1454ad6514938718c15e4971eed876e4da7bbe11326f0bd6830b3f10a3e49a788a4fd4176c  limine.h" | sha512sum --check --status || { echo "limine.h hash mismatch! File deleted."; rm -f limine.h; exit 1; }

wget "https://github.com/Limine-Bootloader/Limine/releases/download/v12.9.4/limine-binary.tar.gz"
echo "078c7e7682285f189f92224541bf5e579bea9c39c38f1009d1d8bd6f81fa5d5b7802ef1e39c3fcf6437c2558c9892d1a40bdf3d2e92a4f05d32e581a1301a270  limine-binary.tar.gz" | sha512sum --check --status || { echo "limine-binary.tar.gz hash mismatch! File deleted."; rm -f limine-binary.tar.gz; exit 1; }
tar --strip-components=1 -xvf limine-binary.tar.gz limine-binary/BOOTAA64.EFI limine-binary/limine-uefi-cd.bin
rm -f limine-binary.tar.gz

wget "https://github.com/rust-osdev/ovmf-prebuilt/releases/download/edk2-stable202605-r1/edk2-stable202605-r1-bin.tar.xz"
echo "f4160cc4a6136f05687051c309f2dbf0ccc8a61311acbf8256f4fd404cc2b0984da28b412241bed92ee89a1b5bbd812ebb3537401307111175cc4c9b73c1e510  edk2-stable202605-r1-bin.tar.xz" | sha512sum --check --status || { echo "edk2-stable202605-r1-bin.tar.xz hash mismatch! File deleted."; rm -f edk2-stable202605-r1-bin.tar.xz; exit 1; }
tar --strip-components=2 -xvf edk2-stable202605-r1-bin.tar.xz edk2-stable202605-r1-bin/x64/code.fd edk2-stable202605-r1-bin/x64/vars.fd
rm -f edk2-stable202605-r1-bin.tar.xz
