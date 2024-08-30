
;**********
;           MEM
;**********

mem	.proc

	; ---------------------------------------------------------- mem.copy.short
	; input :	a 		size
	; 			zpWord0	source
	;			zpWord1	dest
	;
	copy	.proc
		short	.proc
			sta mem.copy.short.size+1
			ldy #0
			-
			lda (zpWord0),y
			sta (zpWord1),y
			iny
		size:
			cpy #5	; self-modified(5) 
			bne -
			rts
		.endproc
	.endproc

	; ---------------------------------------------------------- load
	
	load	.proc
	
		byte	.proc		;	a := (ay)
		
			sta zpWord0+0
			sty zpWord0+1
		zpWord
			ldy #0
			lda (zpWord0),y
			rts
			
		.endproc
		
	; ---------------------------------------------------------- load word
	
		word	.proc		;	ay := (ay)
		
			sta zpWord0+0
			sty zpWord0+1
		zpWord
			ldy #0
			lda (zpWord0+0),y
			tax
			iny
			lda (zpWord0+0),y
			tay
			txa
			sta zpWord0+0
			sty zpWord0+1			
			rts
			
		.endproc
		
	; ---------------------------------------------------------- byte ptr
	
		byte_ptr .proc		;	a  := (ay)
		
			jsr mem.load.word
			jsr mem.load.byte
			rts
			
		.endproc
		
	; ---------------------------------------------------------- word ptr
	
		word_ptr .proc		;	ay : = (ay)

			jsr mem.load.word
			jsr mem.load.word
			rts
			
		.endproc
		
	; ---------------------------------------------------------- real
	
		real .proc			;	fac1 := ay

			jsr basic.load5_fac1
			rts
			
		.endproc
		
	; ---------------------------------------------------------- ptr
	
		real_ptr .proc		;	fac1 := (ay)

			jsr mem.load.word
			jsr mem.load.real
			rts
			
		.endproc
		
	.endproc

	; ---------------------------------------------------------- store
	
	store	.proc
	
		byte	.proc			;	 zpWord0   := a
		
			ldy #0
			sta (zpWord0),y
			rts
			
		.endproc

		byte_ptr	.proc		;	 (zpWord0)  := a
		
			pha
			jsr mem.load.word.zpWord
			ldy #0
			pla
			sta (zpWord0),y
			rts
			
		.endproc
		
		word	.proc			;	 zpWord0  := ay

			tax
			sty zpy
			ldy #0
			txa
			sta (zpWord0+0),y
			lda zpy
			iny
			sta (zpWord0+0),y
			rts
			
		.endproc

		word_ptr	.proc			;	 (zpWord0)  := ay

			pha
			tya
			pha
			jsr mem.load.word.zpWord
			pla
			tay
			pla
			jsr mem.store.word

			rts
			
		.endproc

		word_real_ptr	.proc		;	(zpWord0) := fac1

			jsr mem.load.word.zpWord
			jsr basic.store_fac1
			rts
			
		.endproc
		
	.endproc
		
.endproc
