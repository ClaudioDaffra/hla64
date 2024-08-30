
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

	; ---------------------------------------------------------------   mul_bytes
    ;
    ;   multiply 2 bytes A and Y, zpWord1 as byte in A  
    ;   (signed or unsigned)
    ;
    ;   input   :   a,y
    ;   output  :   a/y
	;
	; mult6.a
	; from eurorusty: 	https://everything2.com/user/eurorusty/writeups/fast+6502+multiplication
	; from 			:	https://github.com/ClaudioDaffra/multiply_test/blob/main/tests/mult6.a
	; 8 bit x 8 bit unsigned multiply, 16 bit result
	; average cycles: 137.21
	; 620 bytes
	;
	; multiplies zpa * zpy
	; result stored in zpWord1+0 and zpWord1+1
	; code is compatible with http://skilldrick.github.io/easy6502/
	;
	; align tables to page boundary for speed
	
.align $40
math_mul_u8s8_table_lo

    .byte 0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225
    .byte 0, 33, 68, 105, 144, 185, 228, 17, 64, 113, 164, 217, 16, 73, 132, 193
    .byte 0, 65, 132, 201, 16, 89, 164, 241, 64, 145, 228, 57, 144, 233, 68, 161
    .byte 0, 97, 196, 41, 144, 249, 100, 209, 64, 177, 36, 153, 16, 137, 4, 129
    .byte 0, 129, 4, 137, 16, 153, 36, 177, 64, 209, 100, 249, 144, 41, 196, 97
    .byte 0, 161, 68, 233, 144, 57, 228, 145, 64, 241, 164, 89, 16, 201, 132, 65
    .byte 0, 193, 132, 73, 16, 217, 164, 113, 64, 17, 228, 185, 144, 105, 68, 33
    .byte 0, 225, 196, 169, 144, 121, 100, 81, 64, 49, 36, 25, 16, 9, 4, 1
    .byte 0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225
    .byte 0, 33, 68, 105, 144, 185, 228, 17, 64, 113, 164, 217, 16, 73, 132, 193
    .byte 0, 65, 132, 201, 16, 89, 164, 241, 64, 145, 228, 57, 144, 233, 68, 161
    .byte 0, 97, 196, 41, 144, 249, 100, 209, 64, 177, 36, 153, 16, 137, 4, 129
    .byte 0, 129, 4, 137, 16, 153, 36, 177, 64, 209, 100, 249, 144, 41, 196, 97
    .byte 0, 161, 68, 233, 144, 57, 228, 145, 64, 241, 164, 89, 16, 201, 132, 65
    .byte 0, 193, 132, 73, 16, 217, 164, 113, 64, 17, 228, 185, 144, 105, 68, 33
    .byte 0, 225, 196, 169, 144, 121, 100, 81, 64, 49, 36, 25, 16, 9, 4, 1
	
.align $40
math_mul_u8s8_table_hi

    .byte 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    .byte 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3
    .byte 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8
    .byte 9, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15
    .byte 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24
    .byte 25, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35
    .byte 36, 36, 37, 38, 39, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48
    .byte 49, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
    .byte 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79
    .byte 81, 82, 83, 84, 85, 86, 87, 89, 90, 91, 92, 93, 95, 96, 97, 98
    .byte 100, 101, 102, 103, 105, 106, 107, 108, 110, 111, 112, 114, 115, 116, 118, 119
    .byte 121, 122, 123, 125, 126, 127, 129, 130, 132, 133, 135, 136, 138, 139, 141, 142
    .byte 144, 145, 147, 148, 150, 151, 153, 154, 156, 157, 159, 160, 162, 164, 165, 167
    .byte 169, 170, 172, 173, 175, 177, 178, 180, 182, 183, 185, 187, 189, 190, 192, 194
    .byte 196, 197, 199, 201, 203, 204, 206, 208, 210, 212, 213, 215, 217, 219, 221, 223
    .byte 225, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254

math_mul_u8s8_table .proc

		sta zpa
		sty zpy
		
	mult
		;if a & b are odd, r = -1, else r = 0
		lda zpa
		and zpy
		and #1
		beq _1
		lda #255

	_1
		sta zpWord1+0
		sta zpWord1+1

		;m = a / 2
		lda zpa
		lsr
		sta zpWord0+0
		bcc _2

		;a is odd, r += b
		clc
		lda zpWord1+0
		adc zpy
		sta zpWord1+0
		lda zpWord1+1
		adc #0
		sta zpWord1+1

	_2
		;n = b / 2
		lda zpy
		lsr
		sta zpWord0+1
		bcc _3

		;b is odd, r += a
		clc
		lda zpWord1+0
		adc zpa
		sta zpWord1+0
		lda zpWord1+1
		adc #0
		sta zpWord1+1

	_3
		;normalize so that m >= n
		lda zpWord0+0
		cmp zpWord0+1
		bcs _4

		;n > m, swap m and n
		tay
		lda zpWord0+1
		sta zpWord0+0
		sty zpWord0+1

	_4
		;r += (m + n)^2
		clc
		lda zpWord0+0
		adc zpWord0+1
		tay
		lda zpWord1+0
		adc math_mul_u8s8_table_lo,y
		sta zpWord1+0
		lda zpWord1+1
		adc math_mul_u8s8_table_hi,y
		sta zpWord1+1

		;r -= (m - n)^2
		sec
		lda zpWord0+0
		sbc zpWord0+1
		tay
		lda zpWord1+0
		sbc math_mul_u8s8_table_lo,y
		sta zpWord1+0
		lda zpWord1+1
		sbc math_mul_u8s8_table_hi,y
		sta zpWord1+1

		lda zpWord1+0
		ldy zpWord1+1

		;done!
		rts
		
.endproc

; ###############
;	main
; ###############

main .proc

	;	.............................................  u8

		lda #8
		ldy #9

		jsr math_mul_u8s8_table

		jsr std.print_u8_dec

		lda #petscii.nl
		jsr kernel.chrout
		
	;	.............................................  s8

		lda #-8
		ldy #9

		jsr math_mul_u8s8_table

		jsr std.print_s8_dec

		lda #petscii.nl
		jsr kernel.chrout

	;	.............................................  u16
.comment	
		load_imm_zpWord0	4
		load_imm_ay 		124
		
		;jsr math_mul_u16s16_table ; ok
		jsr multiply_16bit_unsigned
		
		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout

	;	.............................................  s16
	
		load_imm_zpWord0	4
		load_imm_ay 		-123
		
		; ko jsr math_mul_u16s16_table
		
		jsr multiply_16bit_signed
		
		jsr std.print_s16_dec ; ko

		lda #petscii.nl
		jsr kernel.chrout
		
		
	;	.............................................  real

		; 1.4
		ldfac1 klocalasmreal00
		; 3.2
		ldfac2 klocalasmreal01
 
		jsr math_mul_f40
 		jsr basic.print_fac1
.endcomment

	rts

.endproc
 

