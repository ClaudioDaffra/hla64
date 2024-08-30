
.include "../../lib/libCD.asm"

.PROGRAM
			
.include "../../lib/libC64.asm"


;	-------------------------------------------------------------------------

u8	.byte	3
u16	.word	?

s8	.char	-128
s16	.sint	?

kLocalAsmReal00                     .byte  $81,$33,$33,$33,$33	;	// 1.4
kLocalAsmReal01                     .byte  $82,$4c,$cc,$cc,$cc	;   // 3.2
kLocalAsmReal02                 	.byte  $87,$76,$e9,$78,$d4  ;  // 123.456  ; 0,0,0,123
kLocalAsmReal03                   	.byte  $8b,$00,$18,$93,$74 	;  // 1024.768	; 0,0,4,  0

main .proc
		
		;	------------------------------------------ u8
		
		lda #3
		cast_from_u8_to_u16
		jsr std.print_u16_dec
		lda #petscii.nl
		jsr kernel.chrout

		lda #128
		cast_from_u8_to_s8
		jsr std.print_s8_dec
		lda #petscii.nl
		jsr kernel.chrout

		lda #128
		cast_from_u8_to_s16
		jsr std.print_s16_dec
		lda #petscii.nl
		jsr kernel.chrout

		lda #255
		cast_from_u8_to_real
		jsr basic.print_fac1
		lda #petscii.nl
		jsr kernel.chrout
		
		;	------------------------------------------ s8
		
		lda #-128
		cast_from_s8_to_s16
		jsr std.print_s16_dec
		lda #petscii.nl
		jsr kernel.chrout

		lda #-128
		cast_from_s8_to_real
		jsr basic.print_fac1
		lda #petscii.nl
		jsr kernel.chrout
		
		;	------------------------------------------ u16

		lday #3
		cast_from_u16_to_real
		jsr basic.print_fac1
		lda #petscii.nl
		jsr kernel.chrout
		
		;	------------------------------------------ s16

		lday #-3
		cast_from_s16_to_real
		jsr basic.print_fac1
		lda #petscii.nl
		jsr kernel.chrout
		
		;	------------------------------------------ real

		;	kLocalAsmReal00	.byte  $81,$33,$33,$33,$33	;	// 1.4		; 0,0,0,  1
		;	kLocalAsmReal01	.byte  $82,$4c,$cc,$cc,$cc	;   // 3.2		; 0,0,0,  3
		;	kLocalAsmReal02	.byte  $87,$76,$e9,$78,$d4  ;	// 123.456  ; 0,0,0,123
		;	kLocalAsmReal03	.byte  $8b,$00,$18,$93,$74 		// 1024.768	; 0,0,4,  0

		ldfac1	kLocalAsmReal00	
		cast_from_real_to_u8
		jsr std.print_u8_dec
		lda #petscii.nl
		jsr kernel.chrout
		
		ldfac1	kLocalAsmReal01	
		cast_from_real_to_s8
		jsr std.print_s8_dec
		lda #petscii.nl
		jsr kernel.chrout
		
		ldfac1	kLocalAsmReal02	
		cast_from_real_to_u16
		jsr std.print_u16_dec
		lda #petscii.nl
		jsr kernel.chrout
		
		ldfac1	kLocalAsmReal03	
		cast_from_real_to_s16
		jsr std.print_s16_dec
		lda #petscii.nl
		jsr kernel.chrout

		;	------------------------------------------ real
		ldfac1	kLocalAsmReal03	
		jsr basic.conv_fac1_to_dWord0
		
		rts
.endproc
