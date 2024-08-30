
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
	
		lda #05
		math_neg_u8s8
	

		jsr std.print_s8_dec
		lda #petscii.nl
		jsr kernel.chrout
		
		lda #-05
		math_neg_u8s8

		jsr std.print_u8_dec
		lda #petscii.nl
		jsr kernel.chrout
		

	;	.............................................  word
	
		lday #513
		math_neg_u16s16

		jsr std.print_s16_dec
		lda #petscii.nl
		jsr kernel.chrout

		lday #-513
		math_neg_u16s16

		jsr std.print_u16_dec
		lda #petscii.nl
		jsr kernel.chrout		

	;	.............................................  real

		; 1024.768
		ldfac1 kLocalAsmReal03
		math_neg_f40
		jsr basic.print_fac1
		
		lda #petscii.nl
		jsr kernel.chrout
		
		; -1024.768
		ldfac1 kLocalAsmReal04
		math_neg_f40
		jsr basic.print_fac1

		lda #petscii.nl
		jsr kernel.chrout


	rts

.endproc




