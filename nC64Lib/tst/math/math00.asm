
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

main .proc

	;	print unsigned integer
	lda #123
	jsr math.neg_u8s8
	jsr std.print_s8_dec


	lda #petscii.nl
	jsr kernel.chrout
	
	;	print unsigned integer
	load_imm_ay -456
	jsr math.neg_u16s16
	
	jsr std.print_u16_dec

	lda #petscii.nl
	jsr kernel.chrout

	;	neg float

	ldfac1	kLocalAsmReal03
	jsr basic.neg_fac1
	jsr basic.print_fac1
	

	ldfac1	kLocalAsmReal03
	jsr basic.neg_fac1
	load_address_ay $c000
	jsr basic.store_fac1

	;	print float
	
	ldfac1 $c000
	jsr basic.print_fac1

	rts

.endproc




