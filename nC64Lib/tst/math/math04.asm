
.include "../../lib/libCD.asm"

.PROGRAM
			
.include "../../lib/libC64.asm"


u8	.byte	3
u16	.word	512

s8	.char	-128
s16	.sint	45

klocalasmreal00                     .byte  $81,$33,$33,$33,$33	;	// 1.4
klocalasmreal01                     .byte  $82,$4c,$cc,$cc,$cc	;   // 3.2
klocalasmreal03                   	.byte  $8b,$00,$18,$93,$74 	;  //  1024.768
klocalasmreal04                   	.byte  $8b,$80,$18,$93,$74 	;  // -1024.768
klocalasmreal05                   	.byte  0,0,0,0,0 			;  // 0

; ###############
;	MAIN
; ###############



; ###############
;	MAIN
; ###############

main .proc

	;	.............................................  u8

		lda #8
		ldy #9

		jsr math.mul_u8s8_fast

		jsr std.print_u8_dec

		lda #petscii.nl
		jsr kernel.chrout
		
	;	.............................................  s8

		lda #-8
		ldy #9

		jsr math.mul_u8s8_fast

		jsr std.print_s8_dec

		lda #petscii.nl
		jsr kernel.chrout


	;	.............................................  u16
	
		load_imm_zpWord0	4
		load_imm_ay 		124
		
		jsr math.mul_u16s16_fast

		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout

	;	.............................................  s16
	
		load_imm_zpWord0	4
		load_imm_ay 		-123
		
		jsr math.mul_u16s16_fast

		jsr std.print_s16_dec

		lda #petscii.nl
		jsr kernel.chrout
		
	;	.............................................  real

		; 1.4
		ldfac1 klocalasmreal00
		; 3.2
		ldfac2 klocalasmreal01
 
		jsr math.mul_f40
 		jsr basic.print_fac1

	rts

.endproc
 

