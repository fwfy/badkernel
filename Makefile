.PHONY: all clean apple initramfs
all: linux/arch/x86/boot/bzImage
clean:
	$(MAKE) -C apple_src clean
	$(MAKE) -C linux clean
	rm -r initramfs
test: linux/arch/x86/boot/bzImage
	qemu-system-x86_64 -kernel $< \
	    -audiodev pa,id=snd0 \
		-device intel-hda \
		-device hda-output,audiodev=snd0 \
		-m 2G

apple:
	$(MAKE) -C apple_src
initramfs: apple
	mkdir -p initramfs/sbin initramfs/proc initramfs/dev
	cp apple_src/{apple,init} initramfs/sbin
	cp apple_src/badapple.txt initramfs
	cp apple_src/badapple.pcm initramfs
linux/arch/x86/boot/bzImage: .config initramfs
	cp $< linux/
	$(MAKE) -C linux
