[BITS 16]
[ORG 0x7C00]

start:
	mov ax, 0x00
	mov ds, ax

	mov si, bmsg 	; Print "Starting..."
	call print	; ...

	mov ax, 0x4F02	; VESA function "Set video Mode"
	mov bx, 0x0107  ; 1024x768x256
	int 0x10	; BIOS video interrupt

	jmp $		; sadloop

print:
	mov ah, 0x0E	; set registers for
	mov bh, 0x00	; bios video call
	mov b1, 0x07

.next:
	loadsb		; string stuff
	or al,al	; if 0...
	jz .return	; return
	int 0x10	; print character
	jmp .next	; continue

.return:
	ret

bmsdg db 'Starting...',13,10,0

; Boot magic
times 510-($-$$) db 0
dw 0xAA55
