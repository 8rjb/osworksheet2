global outb              ; Make outb visible to the linker.


; outb - send a byte to an I/O port
; stack: [esp + 8] the data byte
; [esp + 4] the I/O port
; [esp ] return addres

; e.g: C calls outb(0x3D4,14), 14 is moved to al, 0x3D4 is moved to DX, ret address is at ESP, 14 is sent to port number 0x3D4
outb:
    mov al, [esp + 8]    ; Read the data byte from the second argument.
    mov dx, [esp + 4]    ; Read the port number from the first argument.
    out dx, al           ; Send the byte in AL to the port in DX.
    ret                  ; Return to the C caller.