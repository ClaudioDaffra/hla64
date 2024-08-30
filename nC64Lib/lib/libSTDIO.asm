
;**********
;           STDIO
;**********

std	.proc

	; ---------------------------------------------------------- print_unsigned_integer
	; input :	ay ( integer address )
	;
	print_uword	.proc

		jsr basic.print_unsigned_integer
		rts

	.endproc
	
	; ---------------------------------------------------------- print_fac1
	; input :	fac1
	;
	print_fac1	.proc

		jsr basic.print_fac1
		rts
	
	.endproc

	; ------------------------------------------------------------------	 print_u8_dec
	;	input	: a
	;
	print_u8_dec	.proc		; https://stardot.org.uk/forums/viewtopic.php?t=3166

			pha
			jsr count_leading
			pla	
			
		loop1                   ; loop for each digit to be printed
			ldy #0              ; count = 0
		-
			cmp powers,x        ; see if subtracting 10^n would take us below 0
			bcc +          		; if so, we don't try to subtract any more
			iny                 ; increment count
			sbc powers,x        ; subtract 10^n from value
			bcs -          		; unconditionally reloop (c will always be set here)
		+
			pha                 ; preserve accumulator
			tya                 ; get count into accumulator
			adc #'0'            ; add ascii code for digit 0 (c will always be clear)

			jsr kernel.chrout
			pla
			
			dex                 ; go to next digit to be printed
			bpl loop1           ; reloop
			
		rts
		
		;	.................................................... count_leading
		
		count_leading .proc
			ldx #2				; number of digits to print, minus 1
		removeleading0s
			cmp powers,x		; if the value is >= 10^X...  
			bcs return   		; go ahead and start to print the value	
			dex              	; otherwise decrease the digit count
			bne removeleading0s
		return
		rts
		
		.endproc

		powers	.byte 1,10,100

	.endproc

	; ------------------------------------------------------------------	print_s8_dec
	;	input	: a
	;
	print_s8_dec	.proc
			cmp #128
			bcc unsigned
			
		signed
		
			eor #255
			clc
			adc #1
			
			pha
			lda #'-'
			jsr kernel.chrout
			pla	
			
		unsigned

			jsr print_u8_dec
			rts
	.endproc

	; ------------------------------------------------------------------	print_u16_dec	
	;	input	: zpWord0
	;
	print_u16_dec	.proc ; https://beebwiki.mdfs.net/Number_output_in_6502_machine_code

			sty zpWord0+1
			sta zpWord0+0
		
		    ;lda zpWord0+0 ; ridondante
			bne start
			ldy zpWord0+1
			bne start
			jsr print_u8_dec 
			rts
			
		start
		
			lda #0
			sta zpa
			ldy #8                        	; offset to powers of ten
		print_dec_u16lp1
			ldx #$ff
			sec                             ; start with digit=-1
		print_dec_u16lp2
			lda zpWord0+0
			sbc powers+0,y
			sta zpWord0+0  					; subtract current tens
			lda zpWord0+1
			sbc powers+1,y
			sta zpWord0+1
			inx	
			bcs print_dec_u16lp2    		; loop until <0
			lda zpWord0+0
			adc powers+0,y
			sta zpWord0+0  					; add current tens back in
			lda zpWord0+1
			adc powers+1,y
			sta zpWord0+1
			txa
			bne print_dec_u16digit    		; not zero, print it
			lda zpa
			bne print_dec_u16print
			beq print_dec_u16next 			; zpa<>0, use it
		print_dec_u16digit
			ldx #%00110000					; asc('0')
			stx zpa                    		; no more zero zpading
			ora #%00110000            		; print this digit
		print_dec_u16print
			jsr kernel.chrout
		print_dec_u16next
			dey
			dey
			bpl print_dec_u16lp1            ; loop for next digit
		rts

		powers .word 1,10,100,1000,10000

   .endproc

	; ------------------------------------------------------------------	print_u16_dec	
	;	input	: zpWord0
	;
    print_s16_dec .proc

			sta zpWord0+0
			sty zpWord0+1
			bne start
			ldy zpWord0+1
			bne start
			jsr print_u8_dec 
			rts
			
		start
		
			sta zpWord0+0
			sty zpWord0+1
			bpl unsigned

       signed  
	   
            lda #'-'
            jsr  kernel.chrout

			lda zpWord0+0
			eor #255
			clc
			adc #1
			sta zpWord0+0
			
			lda zpWord0+1
			eor #255
			sta zpWord0+1

        unsigned
		
            jsr print_u16_dec.start

            rts

    .endproc

.endproc

; https://www.c64-wiki.com/wiki/control_character

petscii .proc 

	nl				=  13
	clear_screen	= 147
	
.endproc

ascii .proc 

	;clear_screen	= 
	
.endproc