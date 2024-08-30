
;**********
;           BASIC
;**********

;--------------------------------------------------------------- BASIC

basic .proc

	; **********
	; *	PRINT
	; **********
	
	.weak	
		_print_unsigned_integer		= 	$bdcd 	; xa
		print_string				=   $ab1e   ; ay 	print a zero byte terminated petscii string		
	.endweak
	
	;	....................................................................	print_unsigned_integer
	
	print_unsigned_integer	.proc  				; ay 
		tax
		tya
		jsr basic._print_unsigned_integer
		rts
	.endproc

	;	....................................................................	print_unsigned_char
	
	print_unsigned_char	.proc  					; a
		tax
		lda #0
		jsr basic._print_unsigned_integer
		rts
	.endproc
	
	;	....................................................................	print_fac1
	
	print_fac1	.proc
		jsr basic.conv_fac1_to_string
		lda #<$0100
		ldy #>$0100
		jsr basic.print_string
		rts
	.endproc

	;	....................................................................	print_byte
	
	print_byte	.proc
		jsr kernel.chrout
		rts
	.endproc
	
	; **********
	; *	CONV
	; **********

	;	....................................................................	conv_fac1_to_string
	
	.weak
		conv_fac1_to_string			=	$bddd	; 	f1	convert fac1 to string at $100
		conv_u8_to_fac1				=	$b3a2	;	a	Convert u8  held in Y to a FP number in FAC1
		conv_s8_to_fac1				=	$bc3c	;	a	Convert s8  held in A to a FP number in FAC1
		private_conv_s16_to_fac1	=	$b391   ;   ay  Convert s16 held in Y/A (lo/high) to a FP number in FAC1
		
		;There seems to be only a single routine in the ROMs, see Lothar Englisch, 
		;“The Advanced Machine Language Book”, Abacus Software, 1984, p 28. ;
		;IT is located at $BC9B and converts the floating-point value in the FAC ;
		;(the fractional portion is truncated) into a whole number. ;
		; The four mantissa bytes ($62-$65) contain the value in a big-endian representation.
		
		conv_fac1_to_i32			=	$bc9b
		
	.endweak

	conv_fac1_to_dWord0	.proc
		jsr basic.conv_fac1_to_i32
		lda $65
		sta dWord0+0
		lda $64
		sta dWord0+1
		lda $63
		sta dWord0+2
		lda $62
		sta dWord0+3
	.endproc
	
	conv_u16_to_fac1
	conv_s16_to_fac1	.proc
		sty zpy
		tay
		lda zpy
		jsr basic.private_conv_s16_to_fac1
		rts
	.endproc
	
	; **********
	; *	LOAD
	; **********
	
	.weak
	
		load5_fac1					=	$bba2 	; ay	load fac1 from the 5 bytes pointed to by a/y (low)/(high). 
												;		returns y = 0 and a loaded with the exponent, affecting the processor status flags.
		load5_fac2					=	$ba8c 	; ay	load fac2 from the 5 bytes pointed to by a/y (low)/(high). 
												;		returns with fac1's exponent in a.
	.endweak	

	; **********
	; *	STORE
	; **********

	.weak
		store5_fac1_xy				=	$bbd4 	; xy	stores fac1 as 5 bytes at the address pointed to by x/y (low)/(high). 
												;		checks the rounding byte ($70) beforehand, and rounds if the msb is set.
												;		as with loading, it returns with the exponent in a.
		store_fac1_in_fac2_round	=	$bc0c 	; f1,f2	stores fac1 in fac2, rounding if necessary.
		store_fac1_in_fac2			=	$bc0f 	; f1,f2	stores fac1 in fac2  skipping rounnding
		store_fac2_in_fac1			=	$bbfc 	; f1,f2	stores fac2 in fac1	
		
	.endweak	

	;	....................................................................	store_fac1
	
	store_fac1	.proc							; ay
		tax
		jsr basic.store5_fac1_xy
		rts
	.endproc

	; **********
	; *	MATH
	; **********

	.weak
		;	kLocalAsmReal03                   	.byte  $8b,$00,$18,$93,$74 	;  //  1024.768	bit segno ( ?000:0000 )
		;	kLocalAsmReal04                   	.byte  $8b,$80,$18,$93,$74 	;  // -1024.768	bit segno ( 1000:0000 ) $80
		;
		neg_fac1					= 	$bfb4	;	49076	makes fac negative
		
		add_fac1_kb					=	$b86a   ;	Entry if FAC2 already loaded. 
												;	The accumulator must load FAC1 exponent ($61) immediately 
												;	before calling to properly set the zero flag.
		mul_fac1_kb					=	$ba2b 	; 	Entry if FAC2 already loaded. 
												;	Accumulator must load FAC1 exponent ($61) beforehand to set the zero

		div_fac1_kb 				=   $bb12   ;   fac1    :=  fac1    /   fac2    ,   remainer fac2	
		
		
		sub_fac1_kb					=	$b853	; 	Entry if FAC2 already loaded.
		
		neg_fac1_kb					=   $bfb4 	;	neg fac1
		
		sgn_fac1_kb_a				=	$bc2b 	;	Evaulate sign of FAC1 (returned in A)
		sgn_fac1_kb_fac1			=	$bc39	;	Evaulate sign of FAC1 (returned in FAC1)		
		
	.endweak
	
	;	input fac1 -> a
	not_fac1	.proc
		jmp math.not_f40
		;rts
	.endproc
	
	add_fac1	.proc
		jmp math.add_f40
		;rts
	.endproc
	
	mul_fac1	.proc
		jmp math.mul_f40
		;rts
	.endproc
	
	div_fac1	.proc
		jmp math.div_f40
		;rts
	.endproc

	sub_fac1	.proc
		jmp math.sub_f40
		;rts
	.endproc

	neg_fac1	.proc
		math_neg_f40
		rts
	.endproc


.endproc

