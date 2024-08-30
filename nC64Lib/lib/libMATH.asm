

;**********
;           MATH
;**********

math	.proc

	;	...................................................... neg u8,s8	byte

	neg_u8s8	.proc

		sta  zpa
		lda  #0
		sec
		sbc  zpa
		rts

	.endproc
	
	;	...................................................... neg u16,s16	word

    neg_u16s16       .proc
	
		sta zpa
		sty zpy
        sec
        lda  #0
        sbc  zpa
        sta  zpa
        lda  #0
        sbc  zpy
        sta  zpy
		lda  zpa
		ldy  zpy
        rts
		
    .endproc
	
	;	...................................................... neg f40	real
	
	neg_f40	.proc
	
		jsr basic.neg_fac1
		rts
		
	.endproc
	
	;	...................................................... true
	
	true	.proc
		lda #1
		rts
	.endproc
	
	;	...................................................... false
	
	false	.proc
		lda #0
		rts
	.endproc

	;	...................................................... not u8,s8	byte

	not_u8s8	.proc
		beq math.true
		bne math.false
	.endproc

	;	...................................................... not u16,s16	word

	not_u16s16	.proc
		beq +
		bne math.false
	+
		tya
		beq math.true
		bne math.false
	.endproc

	;	...................................................... not f40		real

	not_f40	.proc
		lda #<not_f40.zero
		ldy #>not_f40.zero
		jsr $bc5b  ; -> a
		; = Compare FAC1 with memory contents at A/Y (lo/high)
		;	$00 FAC1 == AY
		;	$01 FAC1  > AY
		;   $FF FAC1  < AY
		beq math.true
		bne math.false
		;rts
	zero	.byte  $00,$00,$00,$00,$00
	.endproc


	;	...................................................... add u16,s16	word
	;
	;	dest	a,y := zpWord0 + a,y

	add_u16s16	.proc
	
			sta	zpWord1+0
			sty zpWord1+1
			clc			
			lda zpWord0+0
			adc zpWord1+0
			pha		
			lda zpWord0+1
			adc zpWord1+1
			tay
			pla	
			
			rts
	
	.endproc

	;	...................................................... add_f40

	;	dest	fac1 := fac1 + fac2

	add_f40	.proc
	
		lda $61
		jsr basic.add_fac1_kb
		rts

	.endproc

	;	...................................................... mul_f40
	;
	;	dest	fac1 := fac1 * fac2

	mul_f40	.proc
	
		lda $61
		jsr basic.mul_fac1_kb
		rts

	.endproc
	
	; ---------------------------------------------------------------   mul_bytes
    ;
    ;   multiply 2 bytes A and Y, zpWord1 as byte in A  
    ;   (signed or unsigned)
    ;
    ;   input   :   a,y
    ;   output  :   a

    mul_u8          =   mul_bytes
    mul_s8          =   mul_bytes
    multiply_bytes  =   mul_bytes
    
    mul_bytes    .proc
        
            sta  zpa            ; num1
            sty  zpy            ; num2
            lda  #0
            beq  _enterloop
    _doAdd        
            clc
            adc  zpa
    _loop        
            asl  zpa
    _enterloop    
            lsr  zpy
            bcs  _doAdd
            bne  _loop
            
            rts
    .endproc

    ; --------------------------------------------------------------- mul_bytes_into_u16
    ;
    ;   multiply 2 bytes A and Y, zpWord1 as word in A/Y (unsigned)
    ;
    ;   input   :   a,y
    ;   output  :   a/y
    ;
    
    mul_u8s8    .proc
	
            sta  zpa
            sty  zpy
            stx  zpx

            lda  #0
            ldx  #8
            lsr  zpa
    -        
            bcc  +
            clc
            adc  zpy
    +        
            ror  a
            ror  zpa
            dex
            bne  -
            tay
            lda  zpa
            ldx  zpx
            
            rts
    .endproc

	;..........................................................................
    ;
    ;   multiply two 16-bit words into a 32-bit zpDWord1  (signed and unsigned)
    ;
    ;      input    :
    ;   
    ;           A/Y             = first  16-bit number, 
    ;           zpWord0 in ZP   = second 16-bit number
    ;
    ;      output   :
    ;        
    ;           zpDWord1  4-bytes/32-bits product, LSB order (low-to-high)
	;			a/Y		  16 bit
    ;
    ;     result    :     zpDWord1  :=  zpWord0 * zpWord1
    ;
    ;   LSB 0123
    ;
    ;   0   zpWord0+0   low
    ;   1   zpWord0+1
    ;   2   zpWord0+2   high
    ;   3   zpWord0+3
            
    mul_u16s16    .proc

    result = zpDWord1
    
            sta  zpWord1
            sty  zpWord1+1
            stx  zpx
    mult16        
            lda  #0
            sta  zpDWord1+2     ; clear upper bits of product
            sta  zpDWord1+3
            ldx  #16            ; for all 16 bits...
    -         
            lsr  zpWord0+1      ; divide multiplier by 2
            ror  zpWord0
            bcc  +
            lda  zpDWord1+2     ; get upper half of product and add multiplicand
            clc
            adc  zpWord1
            sta  zpDWord1+2
            lda  zpDWord1+3
            adc  zpWord1+1
    +         
            ror  a              ; rotate partial product
            sta  zpDWord1+3
            ror  zpDWord1+2
            ror  zpDWord1+1
            ror  zpDWord1
            dex
            bne  -
            ldx  zpx
            
			lda zpDWord1+0
			ldy zpDWord1+1
			
            rts

    .endproc

	; ...................................................... mul_u8s8_fast
	;
	;	https://www.nesdev.org/wiki/8-bit_Multiply
	;
	;	input	:	A moltiplicando
	;				Y moltiplicatore
	; 	output	:	A/Y
	;

	mul_u8s8_fast .proc

			lsr
			sta zpa
			tya
			beq mul8_early_return
			dey
			sty zpy
			lda #0
		
			;0
			bcc +
			adc zpy
		+
			ror
			;1
			ror zpa
			bcc +
			adc zpy
		+
			ror
			;2
			ror zpa
			bcc +
			adc zpy
		+
			ror
			;3
			ror zpa
			bcc +
			adc zpy
		+
			ror
			;4
			ror zpa
			bcc +
			adc zpy
		+
			ror
			;5
			ror zpa
			bcc +
			adc zpy
		+
			ror
			;6
			ror zpa
			bcc +
			adc zpy
		+
			ror
			;7
			ror zpa
			bcc +
			adc zpy
		+
			ror

			tay
			lda zpa
			ror
			
		mul8_early_return
			
			rts

	.endproc

	; ...................................................... mul_u16s16_fast
	;
	; https://github.com/TobyLobster/multiply_test/blob/main/tests/mult55.a
	; mult55.a
	; from The Merlin 128 Macro Assembler disk, via 'The Fridge': http://www.ffd2.com/fridge/math/mult-div.s
	; TobyLobster: with an optimisation for speed (changing pha/pla to tax/txa), then fully unrolled
	; (see mult2)
	;
	; 16 bit x 16 bit unsigned multiply, 32 bit result
	; Average cycles: 483.50
	; 344 bytes
	; acc*aux -> [acc,acc+1,ext,ext+1] (low,hi) 32 bit result

	mul_u16s16_fast	.proc

		aux = zpWord0       ; zpWord0	2 bytes   input1
		acc = zpWord1       ; AY		2 bytes   input2   } result
		ext = zpWord2       ; zpWord	2 bytes            }

			; (acc, acc+1, ext, ext+1) = (aux, aux+1) * (acc, acc+1)

			sta zpWord1
			sty zpWord1+1

		mult
		
			lda #0                          ; A holds the low byte of ext (zero for now)
			sta ext+1                       ; high byte of ext = 0
			lsr acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 1
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 2
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 3
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 4
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+

			; loop step 5
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 6
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 7
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 8
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 9
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 10
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 11
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 12
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 13
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 14
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 15
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			; loop step 16
			ror ext+1                       ; }
			ror                             ; }
			ror acc+1                       ; } acc_ext >> 1
			ror acc                         ; }
			bcc +                           ; skip if carry clear

			clc                             ;               }
			adc aux                         ;               }
			tax                             ; remember A    }
			lda aux+1                       ;               } ext += aux
			adc ext+1                       ;               }
			sta ext+1                       ;               }
			txa                             ; recall A
		+
			sta ext                         ;
			
			lda acc
			ldy acc+1
			
			rts                             ;
			
	.endproc

 ;........................................ div_s8
    ;
    ;   divide A by Y, result quotient in A, remainder in Y   (signed)
    ;
    ;   Inputs:
    ;       a       =   8-bit numerator
    ;       y       =   8-bit denominator
    ;   Outputs:
    ;       a       =   a / y       ( signed   )
    ;       y       =   remainder   ( unsigned ) 
    ;
    
    div_s8    .proc
         
            sta  zpa
            tya
            eor  zpa
            php             ; save sign
            lda  zpa
            bpl  +
            eor  #$ff
            sec
            adc  #0         ; make it positive
    +        
            pha
            tya
            bpl  +
            eor  #$ff
            sec
            adc  #0         ; make it positive
            tay
    +        
            pla
            jsr  internal_div_u8
            sta  zpByte0
            plp
            bpl  +
            tya
            eor  #$ff
            sec
            adc  #0         ; negate result
                            ;   a   result
            ldy zpByte0     ;   y   remainder
    +        
            rts

    .endproc

    internal_div_u8    .proc

            sty  zpy
            sta  zpa
            stx  zpx

            lda  #0
            ldx  #8
            asl  zpa
    -        
            rol  a
            cmp  zpy
            bcc  +
            sbc  zpy
    +        
            rol  zpa
            dex
            bne  -
            ldy  zpa
            ldx  zpx
            
            rts
    .endproc
    
    ;........................................ div_u8
    ;
    ;   divide A by Y, result quotient in A, remainder in Y   (unsigned)
    ;
    ;   Inputs:
    ;       a       =   8-bit numerator
    ;       y       =   8-bit denominator
    ;   Outputs:
    ;       a       =   a / y       ( unsigned  )
    ;       y       =   remainder   ( unsigned  ) 
    ;

    div_u8    .proc
    
        sta zpByte0
        sty zpByte1
        
        lda #0
        ldx #8
        asl zpByte0
    L1  
        rol
        cmp zpByte1
        bcc L2
        sbc zpByte1
    L2 
        rol zpByte0
        dex
        bne L1
        tay
        lda zpByte0
        
        rts
    .endproc
    
    ;.......................................................................    div_u16
    ;
    ;   divide two unsigned words (16 bit each) into 16 bit results
    ;
    ;    input:  
    ;            zpWord0    :   16 bit number, 
    ;            A/Y        :   16 bit divisor
    ;    output: 
    ;            zpWord1    :   16 bit remainder, 
    ;            A/Y        :   16 bit division result
    ;            zpWord0
    ;            flag V     :   1 = division by zero
    ;
    ;   signed word division: make everything positive and fix sign afterwards
        
    div_s16    .proc

            cmp     #$00
            bne     check_divByZero
            cpy     #$00
            bne     check_divByZero
            sev
            rts
            
     check_divByZero
     
            sta saveA
            sty saveY
            
            sta  zpWord1
            sty  zpWord1+1
            lda  zpWord0+1
            eor  zpWord1+1
            php            ; save sign
            lda  zpWord0+1
            bpl  +
            lda  #0
            sec
            sbc  zpWord0
            sta  zpWord0
            lda  #0
            sbc  zpWord0+1
            sta  zpWord0+1
    +        
            lda  zpWord1+1
            bpl  +
            lda  #0
            sec
            sbc  zpWord1
            sta  zpWord1
            lda  #0
            sbc  zpWord1+1
            sta  zpWord1+1
    +        
            tay
            lda  zpWord1
            jsr  div_u16
            ;
            pha
            lda zpWord1
            sta zpWord3
            lda zpWord1+1
            sta zpWord3+1
            pla
            ;
            plp            ; restore sign
            bpl  +
            sta  zpWord1
            sty  zpWord1+1
            lda  #0
            sec
            sbc  zpWord1
            pha
            lda  #0
            sbc  zpWord1+1
            tay
            pla
    +       
            pha
            lda zpWord3
            sta zpWord1
            lda zpWord3+1
            sta zpWord1+1
            pla

            pha
            lda saveA
            sta zpWord3
            lda saveY
            sta zpWord3+1
            pla
            
            sta zpWord0
            sty zpWord0+1
            
            clv
            rts
     
     saveA  .byte   0
     saveY  .byte   0
     
    .endproc

    ;.......................................................................    div_u16
    ;
    ;   divide two unsigned words (16 bit each) into 16 bit results
    ;
    ;    input:  
    ;            zpWord0    :   16 bit number, 
    ;            A/Y        :   16 bit divisor
    ;    output: 
    ;            zpWord1    :   in ZP: 16 bit remainder, 
    ;            A/Y        :   16 bit division result
    ;            zpWord0
    ;            flag V     :   1 = division by zero
    ;
    
    div_u16    .proc

    dividend    = zpWord0
    remainder   = zpWord1
    result      = zpWord0   ;   dividend
    divisor     = zpWord3
    
            cmp     #$00
            bne     check_divByZero
            cpy     #$00
            bne     check_divByZero
            sev
            rts
            
     check_divByZero
     
            sta  divisor
            sty  divisor+1
            stx  zpx
            lda  #0             ;preset remainder to 0
            sta  remainder
            sta  remainder+1
            ldx  #16            ;repeat for each bit: ...
    -        
            asl  dividend       ;dividend lb & hb*2, msb -> Carry
            rol  dividend+1
            rol  remainder      ;remainder lb & hb * 2 + msb from carry
            rol  remainder+1
            lda  remainder
            sec
            sbc  divisor        ;substract divisor to see if it fits in
            tay                 ;lb result -> Y, for we may need it later
            lda  remainder+1
            sbc  divisor+1
            bcc  +              ;if carry=0 then divisor didn't fit in yet

            sta  remainder+1    ;else save substraction result as new remainder,
            sty  remainder
            inc  result         ;and INCrement result cause divisor fit in 1 times
    +        
            dex
            bne  -

            lda  result
            ldy  result+1
            ldx  zpx

            clv
            rts

    .endproc

	;	...................................................... mul_f40
	;
	;	dest	fac1 := fac1 * fac2

	div_f40	.proc

		jsr stack.push.real_c64			;	push fac1
		jsr stack.push.real_c64_to_fac2	;	push fac2		
		jsr stack.pop.real_c64			;	pop  to fac1
		jsr stack.pop.real_c64_to_fac2	;	pop  to fac2		
	
	fac2_fac1
	
		lda $61
		jsr basic.div_fac1_kb
		
		rts

	.endproc

    ;   ........................................................ mod %

    mod_u8    .proc
        jsr  math.div_u8
        tya                 ;remainer
        rts
    .pend

    mod_u16    .proc
        jsr  math.div_u16
        lda zpWord1
        ldy zpWord1+1   ;   remainder
        rts
    .pend

    mod_s8    .proc
        jsr  math.div_s8
        tya                 ;remainer
        rts
    .pend

    mod_s16    .proc
        jsr  math.div_s16
        lda zpWord1
        ldy zpWord1+1   ;   remainder
        rts
    .pend

	;	...................................................... mul_f40
	;
	;	dest	fac1 := fac1 * fac2

	mod_f40	.proc

		jsr stack.push.real_c64			;	push fac1
		jsr stack.push.real_c64_to_fac2	;	push fac2		
		jsr stack.pop.real_c64			;	pop  to fac1
		jsr stack.pop.real_c64_to_fac2	;	pop  to fac2		
	
	fac2_fac1
	
		lda $61
		jsr div_f40.fac2_fac1

		jsr  $bbfc ; Stores FAC2 in FAC1.
		;jsr stack.push.real_c64_to_fac2
		;jsr stack.pop.real_c64
		
		rts

	.endproc
	
	;	...................................................... sub_u16s16
	;
	;	dest	ay := zpWord0 - ay
	
	sub_u16s16	.proc

		sta	zpWord1+0
		sty zpWord1+1			

	start
	
		sec
		lda zpWord0
		sbc zpWord1
		pha
		lda zpWord0+1
		sbc zpWord1+1

		tay
		pla	

		rts
		  
	.endproc

	;	...................................................... sub_f40

	;	dest	fac1 := fac1 - fac2

	sub_f40	.proc
	
		lda $61
		jsr basic.sub_fac1_kb
		rts

	.endproc


.endproc

math_neg_u8s8	.macro

		sta zpa
		lda #0
		sec
		sbc zpa
		
.endm

math_neg_u16s16	.macro

		sta zpa
		sty zpy
		sec		
		lda #0
		sbc zpa
		pha
		lda #0
		sbc zpy
		tay
		pla

.endm

math_neg_f40	.macro
		jsr basic.neg_fac1_kb
.endm

math_not_u8s8_fast	.macro
		tax
		ldy #01
		dey
		txa
		bne  +
		iny
	+
		tya
 .endm
 
math_not_u16s16_fast	.macro

		sty zpy
		clc
		adc zpy
		math_not_u8s8_fast
		
.endm

math_not_f40_fast	.macro

		jsr basic.sgn_fac1_kb_a
		math_not_u8s8_fast

.endm
