#!/bin/bash
set -exu
cd linux
mkdir -p initramfs/sbin initramfs/proc initramfs/dev
cp ../.config .
echo Copying Bad Apple into the initramfs dir...
cp ../apple_src/badapple.txt initramfs/badapple.txt
echo Building init...
cd ../apple_src
gcc -s -Os -static -o ../linux/initramfs/sbin/init init.c
echo Building apple...
g++ -s -Os -static -o ../linux/initramfs/sbin/apple apple.cpp
cd ../linux
echo Building the kernel image...
make -j$(nproc)
echo Launching QEMU!
qemu-system-x86_64 -kernel arch/x86/boot/bzImage
