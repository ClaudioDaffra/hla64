
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

		lda #0
		math_not_u8s8_fast

		jsr std.print_u8_dec
		lda #petscii.nl
		jsr kernel.chrout

		lda #128
		math_not_u8s8_fast

		jsr std.print_u8_dec
		lda #petscii.nl
		jsr kernel.chrout

		lda #255
		math_not_u8s8_fast

		jsr std.print_u8_dec
		lda #petscii.nl
		jsr kernel.chrout
		
	;	.............................................  word
	
		lday #512
		math_not_u16s16_fast

		jsr std.print_u8_dec
		lda #petscii.nl
		jsr kernel.chrout

		lday #0
		math_not_u16s16_fast

		jsr std.print_u8_dec
		lda #petscii.nl
		jsr kernel.chrout	

		lday #65535
		math_not_u16s16_fast

		jsr std.print_u8_dec
		lda #petscii.nl
		jsr kernel.chrout	
		
	;	.............................................  real

		; 1024.768
		ldfac1 kLocalAsmReal03
		math_not_f40_fast
		jsr std.print_u8_dec
		
		lda #petscii.nl
		jsr kernel.chrout
		
		; 0.0
		ldfac1 kLocalAsmReal05
		math_not_f40_fast
		jsr std.print_u8_dec

		lda #petscii.nl
		jsr kernel.chrout

		; -1024.768
		ldfac1 kLocalAsmReal04
		math_not_f40_fast
		jsr std.print_u8_dec
		
		lda #petscii.nl
		jsr kernel.chrout

		
	rts

.endproc




