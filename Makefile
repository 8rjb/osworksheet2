CC = gcc

# Chapter 3's C compiler flags.
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c

# Compile for a freestanding kernel linked at a fixed address.
CFLAGS += -ffreestanding -fno-pie

all: os.iso

loader.o: source/loader.asm
	nasm -f elf $< -o $@
	cp $@ source/loader.o

source/kernel.o: source/kernel.c drivers/framebuffer.h
	$(CC) $(CFLAGS) $< -o $@

kernel.elf: loader.o source/kernel.o drivers/framebuffer.o drivers/io.o source/link.ld
	ld -T ./source/link.ld -melf_i386 loader.o source/kernel.o drivers/framebuffer.o drivers/io.o -o kernel.elf

iso/boot/kernel.elf: kernel.elf
	cp kernel.elf iso/boot/kernel.elf

iso/boot/grub/stage2_eltorito: /opt/os/stage2_eltorito
	cp /opt/os/stage2_eltorito iso/boot/grub/stage2_eltorito

os.iso: iso/boot/kernel.elf iso/boot/grub/stage2_eltorito iso/boot/grub/menu.lst
	genisoimage -R \
	            -b boot/grub/stage2_eltorito \
	            -no-emul-boot \
	            -boot-load-size 4 \
	            -A os \
	            -input-charset utf8 \
	            -quiet \
	            -boot-info-table \
	            -o os.iso \
	            iso

drivers/framebuffer.o: drivers/framebuffer.c drivers/framebuffer.h drivers/io.h
	$(CC) $(CFLAGS) $< -o $@

drivers/io.o: drivers/io.s
	nasm -f elf $< -o $@

run: os.iso
	qemu-system-i386 -display curses -monitor telnet::45454,server,nowait -serial mon:stdio -boot d -cdrom os.iso -m 32 -d cpu -D logQ.txt

clean:
	rm -f loader.o source/loader.o source/kernel.o drivers/framebuffer.o drivers/io.o kernel.elf iso/boot/kernel.elf os.iso

.PHONY: all run clean #run, clean and all are always treated as actions, not files to be created/updated



