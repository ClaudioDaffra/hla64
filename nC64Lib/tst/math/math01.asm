
.include "../../lib/libCD.asm"

.PROGRAM
			
.include "../../lib/libC64.asm"


u8	.byte	3
u16	.word	?

s8	.char	-128
s16	.sint	?

kLocalAsmReal00                     .byte  $81,$33,$33,$33,$33	;	// 1.4
kLocalAsmReal01                     .byte  $82,$4c,$cc,$cc,$cc	;   // 3.2
kLocalAsmReal03                   	.byte  $8b,$00,$18,$93,$74 	;  //  1024.768
kLocalAsmReal04                   	.byte  $8b,$80,$18,$93,$74 	;  // -1024.768
kLocalAsmReal05                   	.byte  0,0,0,0,0 			;  // 0



main .proc

	;	.............................................  byte
	
	lda #0					;	1
	jsr math.not_u8s8
	jsr std.print_u8_dec

	lda #petscii.nl
	jsr kernel.chrout
	
	lda #123				;	0
	jsr math.not_u8s8
	jsr std.print_u8_dec

	lda #petscii.nl
	jsr kernel.chrout

	;	.............................................  word
	
	load_imm_ay 0					;	1
	jsr math.not_u16s16
	jsr std.print_u8_dec

	lda #petscii.nl
	jsr kernel.chrout

	load_imm_ay 456					;	0
	jsr math.not_u16s16
	jsr std.print_u8_dec

	lda #petscii.nl
	jsr kernel.chrout

	;	.............................................  real
	
	ldfac1	kLocalAsmReal00	;	0
	jsr math.not_f40
	jsr std.print_u8_dec
	lda #petscii.nl
	jsr kernel.chrout

	ldfac1	kLocalAsmReal05	;	1
	jsr math.not_f40
	jsr std.print_u8_dec
	lda #petscii.nl
	jsr kernel.chrout
	
	rts

.endproc




