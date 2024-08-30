
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
	
		lda #03
		pha
		lda #05
		sta zpa
		pla
		clc
		adc zpa

		jsr std.print_u8_dec

		lda #petscii.nl
		jsr kernel.chrout
	
	;	.............................................  word
	
		lday #256
		pha
		tya
		pha
		lday #257
		sta zpWord0+0
		sta zpWord0+1
		pla
		tay
		pla
		jsr math.add_u16s16

		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout
	
	;	.............................................  real

		ldfac1 kLocalAsmReal00
		jsr stack.push.real_c64
		ldfac1 kLocalAsmReal01
		jsr stack.pop.real_c64_to_fac2
		jsr math.add_f40

		jsr basic.print_fac1

		lda #petscii.nl
		jsr kernel.chrout
		
	rts

.endproc




