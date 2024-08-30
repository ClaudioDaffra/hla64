
.include "../../lib/libCD.asm"

.PROGRAM
			
.include "../../lib/libC64.asm"


;	-------------------------------------------------------------------------

	u8	.byte	3
	u16	.word	456

	s8	.char	-128
	s16	.sint	-456

	kLocalAsmReal00                     .byte  $81,$33,$33,$33,$33	;	// 1.4
	kLocalAsmReal01                     .byte  $82,$4c,$cc,$cc,$cc	;   // 3.2
	kLocalAsmReal03                   	.byte  $8b,$00,$18,$93,$74 	;  //  1024.768
	kLocalAsmReal04                   	.byte  $8b,$80,$18,$93,$74 	;  // -1024.768
	kLocalAsmReal05                   	.byte  0,0,0,0,0 			;  // 0

;	-------------------------------------------------------------------------

	pu8		.byte	<u8					, >u8
	pu16	.byte	<u16				, >u16
	pf40	.byte	<kLocalAsmReal04 	, >kLocalAsmReal04
	
;	-------------------------------------------------------------------------

main .proc
	
	; LOAD #1
	
	; ........................................  u8
	
	load_address_ay	u8
	jsr std.print_u16_dec	;	2255

	lda #petscii.nl
	jsr kernel.chrout
	
	load_address_ay	u8		;	3
	jsr mem.load.byte
	jsr std.print_u8_dec
	
	lda #petscii.nl
	jsr kernel.chrout

	; ........................................  ptr u8
	
	load_address_ay	pu8		
	jsr std.print_u16_dec

	lda #petscii.nl
	jsr kernel.chrout
	
	load_address_ay	pu8
	jsr mem.load.byte_ptr
	jsr std.print_u8_dec	;	3
	
	lda #petscii.nl
	jsr kernel.chrout

	; ........................................  u16
	
	load_address_ay	u16
	jsr std.print_u16_dec

	lda #petscii.nl
	jsr kernel.chrout
	
	load_address_ay	u16
	jsr mem.load.word
	jsr std.print_u16_dec	;	456
	
	lda #petscii.nl
	jsr kernel.chrout

	; ........................................  ptr u16
	
	load_address_ay	pu16
	jsr std.print_u16_dec

	lda #petscii.nl
	jsr kernel.chrout
	
	load_address_ay	pu16
	jsr mem.load.word_ptr
	jsr std.print_u16_dec	;	456	
	
	lda #petscii.nl
	jsr kernel.chrout

	; ........................................  ptr f40
	
	load_address_ay	kLocalAsmReal04
	jsr mem.load.real
	jsr std.print_fac1

	lda #petscii.nl
	jsr kernel.chrout
	
	load_address_ay	pf40
	jsr mem.load.real_ptr
	jsr std.print_fac1
	
	lda #petscii.nl
	jsr kernel.chrout
	
	; STORE #2

	; ........................................  pu8
	
	load_address_zpWord0	pu8
	lda #7
	jsr mem.store.byte_ptr

	load_address_ay	u8
	jsr mem.load.byte
	jsr std.print_u8_dec
	
	lda #petscii.nl
	jsr kernel.chrout

	; ........................................  pu16

	load_address_zpWord0	pu16
	load_imm_ay 789
	jsr mem.store.word_ptr

	load_address_ay	u16
	jsr mem.load.word
	jsr std.print_u16_dec

	; STORE #3

	lda #petscii.nl
	jsr kernel.chrout
	
	; load pointer pf40 = &	kLocalAsmReal04 -1024.768
	load_address_zpWord0	pf40
	; load fac1 			kLocalAsmReal00 	1.4
	ldfac1	kLocalAsmReal00
	jsr mem.store.word_real_ptr
	
	ldfac1	kLocalAsmReal04
	jsr std.print_fac1
	
	rts
	
.endproc
