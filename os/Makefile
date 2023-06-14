#.ONESHELL:	# to have all commands running in the same shell, so exit works properly

os-image:
	# the path to bzImage may vary
	@if [ ! -f ./linux*/arch/x86_64/boot/bzImage ]; then \
		make kernel; \
	fi
	@if [ ! -f ./busybox/initramfs.cpio.gz ]; then \
		make initrd; \
	fi
	@if [ ! -f ./busybox/initramfs.cpio.gz ] || [ ! -f ./linux*/arch/x86_64/boot/bzImage ]; then \
		exit; \
	fi
	tput setaf 2
	@echo "***   Making OS Image   ***"
	tput sgr0
	mkdir -p iso iso/boot iso/boot/grub
	cp ./linux*/arch/x86_64/boot/bzImage ./iso/boot/bzImage
	cp ./busybox/initramfs.cpio.gz ./iso/boot/initramfs.cpio.gz
	tput setaf 4
	@echo "What system is using the targeted machine?"
	@echo "     [1] uefi"
	@echo "     [2] bios"
	@echo " -> Note that any unexpected response will lead to the selection of the system on this machine"
	tput sgr0
	@read kind;	\
	if [[ ($$kind == "1") || (-d "/sys/firmware/efi" && $$kind != "2") ]]; then \
		# your system is using uefi; \
		echo set default=0 > ./iso/boot/grub/grub.conf; \
		echo set timeout=10 >> ./iso/boot/grub/grub.conf; \
		echo insmod efi_gop >> ./iso/boot/grub/grub.conf; \
		echo insmod font >> ./iso/boot/grub/grub.conf; \
		echo if loadfont /boot/grub/fonts/unicode.pf2 >> ./iso/boot/grub/grub.conf; \
		echo then >> ./iso/boot/grub/grub.conf; \
		echo "	insmod gfxterm" >> ./iso/boot/grub/grub.conf; \
		echo "	set gfxmode=auto" >> ./iso/boot/grub/grub.conf; \
		echo "	set gfxpayload=keep" >> ./iso/boot/grub/grub.conf; \
		echo "	terminal_output gfxterm" >> ./iso/boot/grub/grub.conf; \
		echo fi >> ./iso/boot/grub/grub.conf; \
		echo menuentry \'districompOS - https://github.com/titofra \' --class os { >> ./iso/boot/grub/grub.conf; \
		echo "	insmod gzio" >> ./iso/boot/grub/grub.conf; \
		echo "	insmod part_msdos" >> ./iso/boot/grub/grub.conf; \
		echo "	linux /boot/bzImage" >> ./iso/boot/grub/grub.conf; \
		echo "	initrd /boot/initramfs.cpio.gz" >> ./iso/boot/grub/grub.conf; \
		echo } >> ./iso/boot/grub/grub.conf; \
	else \
		# your system is using bios; \
		echo set default=0 > ./iso/boot/grub/grub.conf; \
		echo set timeout=10 >> ./iso/boot/grub/grub.conf; \
		echo menuentry \'districompOS - https://github.com/titofra \' --class os { >> ./iso/boot/grub/grub.conf; \
		echo "	insmod gzio" >> ./iso/boot/grub/grub.conf; \
		echo "	insmod part_msdos" >> ./iso/boot/grub/grub.conf; \
		echo "	linux /boot/bzImage" >> ./iso/boot/grub/grub.conf; \
		echo "	initrd /boot/initramfs.cpio.gz" >> ./iso/boot/grub/grub.conf; \
		echo } >> ./iso/boot/grub/grub.conf; \
	fi
	grub-mkrescue -o ./districomp.iso iso/
	tput setaf 2
	@echo "OS Image done !"
	@echo "Available on ./districomp.iso"
	tput sgr0

kernel:
	tput setaf 2
	@echo "***   Making Kernel   ***"
	tput sgr0
	@if [ -d ./linux* ]; then \
		cd ./linux*; \
		tput setaf 3; \
		echo "In the next TUI, edit as you want, save and quit."; \
		echo "WARNING: your display/console must be at least 19 lines by 80 columns."; \
		read -p "Press any key to continue."; \
		tput sgr0;	\
		make menuconfig; \
		make; \
	else \
		tput setaf 1; \
		echo "ERROR: You must manually install a linux kernel. For that, check out https://www.kernel.org/. Make sure to download it here."; \
		tput sgr0; \
		exit; \
	fi
	tput setaf 2
	@echo "Kernel done !"
	tput sgr0

initrd:
	tput setaf 2
	@echo "***   Making Initrd   ***"
	tput sgr0
	@if [ ! -f $(netSetup) ]; then \
		tput setaf 1; \
		echo "ERROR: No networking setup script found. Make sure to have a networking setup script on $(netSetup)"; \
		tput sgr0; \
		exit; \
	fi
	@if [ ! -f $(clientO) ]; then \
		tput setaf 1; \
		echo "ERROR: No client found. Make sure to have client binaries under $(clientO)"; \
		tput sgr0; \
		exit; \
	fi
	@if [ ! -d ./busybox ]; then \
		git submodule add https://git.busybox.net/busybox ./busybox; \
	fi
	cd ./busybox; \
	make defconfig; \
	tput setaf 3; \
	echo "WARNING: In the next TUI, go to Settings and checked 'Build static binary (no shared libs)', then edit as you want, save and quit."; \
	echo "WARNING: your display/console must be at least 19 lines by 80 columns."; \
	read -p "Press any key to continue."; \
	tput sgr0; \
	make menuconfig; \
	make install; \
	cd _install; \
	mkdir dev proc sys; \
	cp ../../$(netSetup) ./networkSetup.sh; \
	cp ../../$(clientO) ./client.o; \
	echo "#!/bin/sh" > init; \
	echo "mount -t devtmpfs none /dev" >> init; \
	echo "mount -t proc none /proc" >> init; \
	echo "mount -t sysfs none /sys" >> init; \
	echo "exec networkSetup.sh" >> init; \
	echo "exec client.o" >> init; \
	chmod +x init; \
	find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz; \
	tput setaf 2
	@echo "Initrd done !"
	tput sgr0

clean:
	tput setaf 3
	@echo "Note: Only clean basic things, not all the files built during install. To do so, follow instructions on busybox and linux Makefile"
	tput sgr0
	rm -rf ./iso ./busybox/initramfs.cpio.gz ./busybox/_install ./linux*/arch/x86_64/boot/bzImage