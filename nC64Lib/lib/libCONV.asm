

;**********
;           CD
;**********

;	*		u8	u16	s8	s16	real
;	u8		x	=	=	=	=		
;	u16		.	x	.	.	.	
;	s8		.	.	x	.	.	
;	s16		.	.	.	x	.	
;	real	.	.	.	.	x	

conv .proc

	fac1_to_string .proc
		jsr basic.conv_fac1_to_string
		rts
	.endproc

.endproc

;	.......................................................... u8

cast_from_u8_to_u16	.macro
	ldy	#0
.endmacro

cast_from_u8_to_s8	.macro
	;
.endmacro

ext_sign_u8_to_s16	.proc
		bpl	+
		ldy #0
		rts
	 +
		ldy #128
		rts
.endproc

cast_from_u8_to_s16	.macro
	jsr ext_sign_u8_to_s16
.endmacro

;$b3a2 = Convert unsigned 8-bit integer held in Y to a FP number in FAC1
cast_from_u8_to_real	.macro
	tay
	jsr basic.conv_u8_to_fac1
.endmacro

;	.......................................................... s8

cast_from_s8_to_u16	.macro
	ldy	#0
.endmacro

cast_from_s8_to_u8	.macro
.endmacro

cast_from_s8_to_s16	.macro
	ldy #255
.endmacro

;$bc3c = Convert signed 8-bit integer held in A to a FP number in FAC1
cast_from_s8_to_real	.macro
	jsr basic.conv_s8_to_fac1
.endmacro

;	.......................................................... u16

cast_from_u16_to_u8	.macro
.endmacro

cast_from_u16_to_s8	.macro
.endmacro

cast_from_u16_to_s16	.macro
.endmacro

;$b391 = Convert signed 16-bit integer held in Y/A (lo/high) to a FP number in FAC1
cast_from_u16_to_real	.macro
		jsr basic.conv_s16_to_fac1
.endmacro

;	.......................................................... s16

cast_from_s16_to_u8	.macro
	ldy #0
.endmacro

cast_from_s16_to_s8	.macro
	ldy #0
.endmacro

cast_from_s16_to_u16	.macro
	;
.endmacro

;$b391 = Convert signed 16-bit integer held in Y/A (lo/high) to a FP number in FAC1
cast_from_s16_to_real	.macro
	jsr basic.conv_s16_to_fac1
.endmacro

;	.......................................................... real

cast_from_real_to_u8	.macro
		jsr basic.conv_fac1_to_i32
		lda $65
.endmacro

cast_from_real_to_s8	.macro
		jsr basic.conv_fac1_to_i32
		lda $65
.endmacro


cast_from_real_to_u16	.macro
		jsr basic.conv_fac1_to_i32
		lda $65
		ldy $64
.endmacro

;$bc44 = Convert signed 16-bit integer held in FAC1 at $63 (lo) and $62 (high) to FP 
cast_from_real_to_s16	.macro
		jsr basic.conv_fac1_to_i32
		lda $65
		ldy $64
.endmacro
 


