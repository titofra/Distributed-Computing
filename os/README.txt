### Instructions to build you are own os with a linux kernel ###

Note that this is a """generic""" tutorials.
For our application, some modifications are done. Check out the Makefile if you are interested.
So, if you want to build the OS for the distrinet, run $make instead of doing this tuto.

This is the sum up of those tutorials, explanations are available on both:
	https://phoenixnap.com/kb/build-linux-kernel
	https://medium.com/@ThyCrow/compiling-the-linux-kernel-and-creating-a-bootable-iso-from-it-6afb8d23ba22 

***  The kernel   ***
1)	Donwload the linux kernel
		check https://www.kernel.org/
2)	Make the config file
		$make menuconfig
		Edit as you want, save and quit
3)	Build the kernel
		$make

***  The initial ramdisk   ***
4)	Download Busybox
		check https://www.busybox.net/downloads/
5)	Make the default config file
		$make defconfig
6)	Edit the config file
		$make menuconfig
7)	Forced the busybox file to be statically linked
		In the TUI, go to Settings and checked "Build static binary (no shared libs)"
		Once again, edit as you want, save and quit
8)	Build the file system
		$make install
9)	Go to our file system's root
		$cd _install
10)	Add some useful folders
		In fact, do what you want here, for this tuto I need dev, proc and sys folders
		$mkdir -p dev proc sys
11)	Add the first executed script
		Create a file named init and put in your script
		For this tuto let's append that:
			#!/bin/sh
			mount -t devtmpfs none /dev
			mount -t proc none /proc
			mount -t sysfs none /sys
			echo "Welcome to my Linux!"
			exec /bin/sh
12)	Create the initrd's file
		$find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz

***  The OS image   ***
13)	Create the iso folder that will contain all our needs
		$cd somewhere/else/
		$mkdir iso
14)	Create the boot folder that will contain the boot system
		$mkdir iso/boot
15)	Create the grub folder that will contain the grub config file
		$mkdir iso/boot/grub
16) Copy the kernel image (created step 3) into our iso folder
		$cp path/to/your/bzImage ./iso/boot/bzImage
17) Copy the initramfs file (created step 12) into our iso folder
		$cp path/to/your/initramfs.cpio.gz ./iso/boot/initramfs.cpio.gz
18)	Create the grub config file
		$echo > ./iso/boot/grub/grub.conf
19)	Fill the grub config file
	If your host is booted using BIOS, put this into grub.conf:
		set default=0
		set timeout=10
		menuentry 'myos' --class os {
			insmod gzio
			insmod part_msdos
			linux /boot/bzImage
			initrd /boot/initramfs.cpio.gz
		}
	If you are using UEFI, put these lines in it:
		set default=0
		set timeout=10
		# Load EFI video drivers. This device is EFI so keep the
		# video mode while booting the linux kernel.
		insmod efi_gop
		insmod font
		if loadfont /boot/grub/fonts/unicode.pf2
		then
				insmod gfxterm
				set gfxmode=auto
				set gfxpayload=keep
				terminal_output gfxterm
		fi
		menuentry 'myos' --class os {
			insmod gzio
			insmod part_msdos
			linux /boot/bzImage
			initrd /boot/initramfs.cpio.gz
		}
20)	Create the OS image
		$grub-mkrescue -o myos.iso iso/