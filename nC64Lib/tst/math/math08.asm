
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
		pha
		lda #03
		sta zpa
		pla
		sec
		sbc zpa

		jsr std.print_u8_dec

		lda #petscii.nl
		jsr kernel.chrout

	;	.............................................  word
	
		lday #513
		sta zpWord0+0
		sty zpWord0+1
		
		lday #257

		jsr math.sub_u16s16

		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout
	
	;	.............................................  real

		; 1.4
		ldfac1 kLocalAsmReal00
		jsr stack.push.real_c64
		; 3.2
		ldfac1 kLocalAsmReal01
		jsr stack.pop.real_c64_to_fac2
		jsr math.sub_f40
		; -1.8
		jsr basic.print_fac1

		lda #petscii.nl
		jsr kernel.chrout


	rts

.endproc




