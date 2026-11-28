global loader ; entry symbol for ELF

extern sum_of_three 
extern difference
extern square 
extern framebuffer_test ; these functions are defined elsewhere

MAGIC_NUMBER equ 0x1BADB002 ; define the magic number
FLAGS equ 0x0 ; multiboot flags (request no optional features)
CHECKSUM equ -MAGIC_NUMBER ; calculate checksum, (magic number + flags + checksum) should = 0

KERNEL_STACK_SIZE equ 4096 ; define stack size in bytes

section .text ; start of the text section
align 4 ; code must be 4 byte aligned
    dd MAGIC_NUMBER ; write the magic number to machine code,
    dd FLAGS ; the flags
    dd CHECKSUM ; and the checksum

loader: ; loader label, used as entry point in the linker script
    ;mov eax, 0xCAFEBABE ; place the number 0xCAFEBABE in eax register

    mov esp, kernel_stack + KERNEL_STACK_SIZE ; point esp to the start of the stack (end of memory area)

    
    sub esp, 4 ; add 4 bytes of padding for 16 bytes
    push dword 3 ; arg3 goes on first
    push dword 2; arg2
    push dword 1; arg1 goes closest to return address
    call sum_of_three ; EAX = 6 on return
    add esp, 16 ; discard 12 argument bytes and 4 padding bytes
    mov esi, eax ; store result in esi to view in log

    sub esp, 8 ; two arguments need 8 bytes of padding for 16 bytes alignment
    push dword 10 ; right
    push dword 7 ; left
    call difference
    add esp, 16 ; discard 8 argument and 8 padding bytes
    mov edi, eax ; store result in edi to view in log

    sub esp, 12 ; 1 argument needs 12 bytes of padding to make 16 byte total
    push dword 9 ; value
    call square
    add esp, 16 ; discard 12 padding and 4 argument bytes
    ; leave final result, 81, in eax

    call framebuffer_test ; Run the C framebuffer test on the Task 2 stack.


.loop: ; loop label
    jmp .loop ; loop forever

section .bss
    align 4 ; align at 4 bytes
    kernel_stack: ; label points to beginning of memory
        resb KERNEL_STACK_SIZE ; reserve stack for the kernel

