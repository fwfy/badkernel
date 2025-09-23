#!/bin/bash
set -eux
echo Cleaning out folder structure...
rm -rf iso
mkdir -p iso/boot/isolinux
echo Copying kernel to iso folder...
cp linux/arch/x86/boot/bzImage iso/boot/
echo Copying other syslinux files...
cp /usr/lib/syslinux/bios/isolinux.bin iso/boot/isolinux/
cp /usr/lib/syslinux/bios/ldlinux.c32 iso/boot/isolinux/
echo Copying config...
cp isolinux.cfg iso/boot/isolinux/
echo Calling mkisofs...
mkisofs -o badkernel.iso \
  -b boot/isolinux/isolinux.bin \
  -c boot/isolinux/boot.cat \
  -no-emul-boot -boot-load-size 4 -boot-info-table \
  -V "BADKERNEL" iso
