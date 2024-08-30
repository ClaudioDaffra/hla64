

.include "../../lib/libCD.asm"

.PROGRAM

.include "../../lib/libC64.asm"

kLocalAsmReal00                     .byte  $81,$33,$33,$33,$33	;	// 1.4
kLocalAsmReal01                     .byte  $82,$4c,$cc,$cc,$cc	;   // 3.2

		
main .proc

	; ## LOCAL STACK
	local_stack .struct      
		a       .byte 0         ;1
		b       .byte 0,0,0,0,0 ;5
	.endstruct 

	; ....................................................... ## SAVE STACK
	
	lda stack.sp
	pha

	jsr stack.get_sp_ay
	jsr basic.print_unsigned_integer

	;	..................................... ## STACK.ALLOC	; bp=sp --sp
	
	lda #size(local_stack)
	jsr stack.alloc	

	lda #' '
	jsr kernel.chrout
	
	jsr stack.get_sp_ay
	jsr basic.print_unsigned_integer

	;	..................................... store const real on stack
	load_address_zpWord0	kLocalAsmReal00
	jsr stack.push.real

	;	..................................... get real on top and print
	jsr stack.get_sp_ay
	jsr basic.load5_fac1
	jsr basic.print_fac1

	;	..................................... get real on mem and print
	load_address_ay	kLocalAsmReal00
	jsr basic.load5_fac1
	jsr basic.print_fac1

	;	..................................... carica il secondo numer0
	load_address_ay	kLocalAsmReal01
	jsr basic.load5_fac1					; carica il reale nel fac1

	;	..................................... lo memorizza al top dello stack
	jsr stack.get_sp_ay
	jsr basic.store_fac1				; memorizza fac1 nello stack

	jsr stack.get_sp_ay					; get stack bottom
	jsr basic.load5_fac1				; caricalo
	jsr basic.print_fac1				; stampa fac1

	;	..................................... jsr stack.pop_real
	
	lda	#petscii.nl
	jsr kernel.chrout

	; BEFORE POP -> TOP
	jsr stack.get_sp_ay					; get stack bottom
	jsr basic.print_unsigned_integer
	
	jsr stack.pop.real
	
	; AFTER POP -> TOP
	lda	#petscii.nl
	jsr kernel.chrout

	jsr stack.get_sp_ay					; get stack bottom
	jsr basic.print_unsigned_integer
	
	; ....................................................... RESTORE STACK

	pla
	sta stack.sp
	sta stack.bp

	lda	#petscii.nl
	jsr kernel.chrout
	jsr stack.get_sp_ay					; get stack bottom
	jsr basic.print_unsigned_integer
	
	rts

.endproc




