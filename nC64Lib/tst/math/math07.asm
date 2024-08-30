
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
;	lib
; ###############



; ###############
;	main
; ###############

main .proc


	;	.............................................  u8

		lda #73
		ldy #9

		jsr math.mod_u8
lda#1
		jsr std.print_u8_dec


		lda #petscii.nl
		jsr kernel.chrout
		
		
	
	;	.............................................  s8	; remainder unsigned

		lda #-80
		ldy #9

		jsr math.mod_s8

		jsr std.print_s8_dec

		lda #petscii.nl
		jsr kernel.chrout
		


	;	.............................................  u16 zpWord0 / ay:zpWord1
	
		load_imm_zpWord0	124
		load_imm_ay 		4

		jsr math.mod_u16

		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout

	;	.............................................  s16	; remainder unsigned
	
		load_imm_zpWord0	-123
		load_imm_ay 		4
		
		jsr math.mod_s16

		jsr std.print_s16_dec

		lda #petscii.nl
		jsr kernel.chrout

	;	.............................................  real
	
		; 3.2
		ldfac1 klocalasmreal01
		; 1.4
		ldfac2 klocalasmreal00

		; 3.2 / 1.4 := 	2.28571428571	remainder	0.4	-> 0.22
		; 1.4 / 3.2 :=	0.4375			remainder   0	-> 0
		
		jsr math.mod_f40
		
 		jsr basic.print_fac1


	rts

.endproc
 

