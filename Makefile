all: os.iso # building the iso is default action when make is ran

loader.o: source/loader.asm # assembling the kernel source code to object file in repo root
	nasm -f elf source/loader.asm
	cp loader.o source/loader.o

kernel.elf: loader.o source/link.ld #link the object using memory layout and entry point from link.ld
	ld -T ./source/link.ld -melf_i386 loader.o -o kernel.elf

iso/boot/kernel.elf: kernel.elf #copy kernel to correct path on disc
	cp kernel.elf iso/boot/kernel.elf

iso/boot/grub/stage2_eltorito: /opt/os/stage2_eltorito #copy grub bootloader to disc
	cp /opt/os/stage2_eltorito iso/boot/grub/stage2_eltorito

os.iso: iso/boot/kernel.elf iso/boot/grub/stage2_eltorito iso/boot/grub/menu.lst #package disc (iso) file
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

run: os.iso #start up the os and log cpu state in txt file
	qemu-system-i386 -nographic -boot d -cdrom os.iso -m 32 -d cpu -D logQ.txt



.PHONY: all run #run and all are always treated as actions, not files to be created/updated



