global loader ; entry symbol for ELF

MAGIC_NUMBER equ 0x1BADB002 ; define the magic number
FLAGS equ 0x0 ; multiboot flags (request no optional features)
CHECKSUM equ -MAGIC_NUMBER ; calculate checksum, (magic number + flags + checksum) should = 0

section .text: ; start of the text section
align 4 ; code must be 4 byte aligned
    dd MAGIC_NUMBER ; write the magic number to machine code,
    dd FLAGS ; the flags
    dd CHECKSUM ; and the checksum

loader: ; loader label, used as entry point in the linker script
    mov eax, 0xCAFEBABE ; place the number 0xCAFEBABE in eax register

.loop: ; loop label
    jmp .loop ; loop forever