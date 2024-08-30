
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

; description: unsigned 8-bit multiplication with unsigned 16-bit result.
;                                                                        
; input: 8-bit unsigned value in t1                                      
;        8-bit unsigned value in t2                                      
;        carry=0: re-use t1 from previous multiplication (faster)        
;        carry=1: set t1 (slower)                                        
;                                                                        
; output: 16-bit unsigned value in product                               
;                                                                        
; clobbered: product, x, a, c                                            
;                                                                        
; allocation setup: t1,t2 and product preferably on zero-page.           
;                   table.square1_lo, table.square1_hi, table.square2_lo, table.square2_hi must be
;                   page aligned. each table are 512 bytes. total 2kb.    
;                                                                         
; table generation: i:0..511                                              
;                   table.square1_lo = <((i*i)/4)                               
;                   table.square1_hi = >((i*i)/4)                               
;                   table.square2_lo = <(((i-255)*(i-255))/4)                   
;                   table.square2_hi = >(((i-255)*(i-255))/4)  

	t1	.byte	0
	t2	.byte	0
product	.word	0
	
 multiply_8bit_unsigned .proc                                              
                ;bcc +                                                    
                    lda t1                                                
                    sta sm1+1                                             
                    sta sm3+1                                             
                    eor #$ff                                              
                    sta sm2+1                                             
                    sta sm4+1                                             
                ;+                                                         
;start

                ldx t2
                sec   
sm1            
				lda table.square1_lo,x
sm2            
				sbc table.square2_lo,x
                sta product+0   
sm3            
				lda table.square1_hi,x
sm4            
				sbc table.square2_hi,x
                sta product+1   

                rts
.endproc           



; description: signed 8-bit multiplication with signed 16-bit result.
;                                                                    
; input: 8-bit signed value in t1                                    
;        8-bit signed value in t2                                    
;        carry=0: re-use t1 from previous multiplication (faster)    
;        carry=1: set t1 (slower)                                    
;                                                                    
; output: 16-bit signed value in product                             
;                                                                    
; clobbered: product, x, a, c                                        
multiply_8bit_signed .proc    
                                       
                jsr multiply_8bit_unsigned                           

                ; apply sign (see c=hacking16 for details).
                lda t1                                     
                bpl :+                                     
                    sec                                    
                    lda product+1                          
                    sbc t2                                 
                    sta product+1                          
                :                                          
                lda t2                                     
                bpl :+                                     
                    sec                                    
                    lda product+1                          
                    sbc t1                                 
                    sta product+1                          
                :                                          

                rts
.endproc           



; description: unsigned 16-bit multiplication with unsigned 32-bit result.
;                                                                         
; input: 16-bit unsigned value in t1                                      
;        16-bit unsigned value in t2                                      
;        carry=0: re-use t1 from previous multiplication (faster)         
;        carry=1: set t1 (slower)                                         
;                                                                         
; output: 32-bit unsigned value in product                                
;                                                                         
; clobbered: product, x, a, c                                             
;                                                                         
; allocation setup: t1,t2 and product preferably on zero-page.            
;                   table.square1_lo, table.square1_hi, table.square2_lo, table.square2_hi must be
;                   page aligned. each table are 512 bytes. total 2kb.    
;                                                                         
; table generation: i:0..511                                              
;                   table.square1_lo = <((i*i)/4)                               
;                   table.square1_hi = >((i*i)/4)                               
;                   table.square2_lo = <(((i-255)*(i-255))/4)                   
;                   table.square2_hi = >(((i-255)*(i-255))/4)                   
multiply_16bit_unsigned     .proc                                         
                ; <t1 * <t2 = aaaa                                        
                ; <t1 * >t2 = bbbb                                        
                ; >t1 * <t2 = cccc                                        
                ; >t1 * >t2 = dddd                                        
                ;                                                         
                ;       aaaa                                              
                ;     bbbb                                                
                ;     cccc                                                
                ; + dddd                                                  
                ; ----------                                              
                ;   product!                                              

                ; setup t1 if changed
                bcc :+               
                    lda t1+0         
                    sta sm1a+1       
                    sta sm3a+1       
                    sta sm5a+1       
                    sta sm7a+1       
                    eor #$ff         
                    sta sm2a+1       
                    sta sm4a+1       
                    sta sm6a+1       
                    sta sm8a+1       
                    lda t1+1         
                    sta sm1b+1       
                    sta sm3b+1       
                    sta sm5b+1       
                    sta sm7b+1       
                    eor #$ff         
                    sta sm2b+1       
                    sta sm4b+1       
                    sta sm6b+1       
                    sta sm8b+1       
                :                    

                ; perform <t1 * <t2 = aaaa
                ldx t2+0                  
                sec                       
sm1a:           lda table.square1_lo,x          
sm2a:           sbc table.square2_lo,x          
                sta product+0             
sm3a:           lda table.square1_hi,x          
sm4a:           sbc table.square2_hi,x          
                sta _aa+1                 

                ; perform >t1_hi * <t2 = cccc
                sec                          
sm1b:           lda table.square1_lo,x             
sm2b:           sbc table.square2_lo,x             
                sta _cc+1                    
sm3b:           lda table.square1_hi,x             
sm4b:           sbc table.square2_hi,x             
                sta _cc+1                    

                ; perform <t1 * >t2 = bbbb
                ldx t2+1                  
                sec                       
sm5a:           lda table.square1_lo,x          
sm6a:           sbc table.square2_lo,x          
                sta _bb+1                 
sm7a:           lda table.square1_hi,x          
sm8a:           sbc table.square2_hi,x          
                sta _bb+1                 

                ; perform >t1 * >t2 = dddd
                sec                       
sm5b:           lda table.square1_lo,x          
sm6b:           sbc table.square2_lo,x          
                sta _dd+1                 
sm7b:           lda table.square1_hi,x          
sm8b:           sbc table.square2_hi,x          
                sta product+3             

                ; add the separate multiplications together
                clc                                        
_aa:            lda #0                                     
_bb:            adc #0                                     
                sta product+1                              
_bb:            lda #0                                     
_cc:            adc #0                                     
                sta product+2                              
                bcc :+                                     
                    inc product+3                          
                    clc                                    
                :                                          
_cc:            lda #0                                     
                adc product+1                              
                sta product+1                              
_dd:            lda #0                                     
                adc product+2                              
                sta product+2                              
                bcc :+                                     
                    inc product+3                          
                :                                          

                rts
.endproc           



; description: signed 16-bit multiplication with signed 32-bit result.
;                                                                     
; input: 16-bit signed value in t1                                    
;        16-bit signed value in t2                                    
;        carry=0: re-use t1 from previous multiplication (faster)     
;        carry=1: set t1 (slower)                                     
;                                                                     
; output: 32-bit signed value in product                              
;
; clobbered: product, x, a, c

 multiply_16bit_signed .proc
                jsr multiply_16bit_unsigned

                ; apply sign (see c=hacking16 for details).
                lda t1+1
                bpl :+
                    sec
                    lda product+2
                    sbc t2+0
                    sta product+2
                    lda product+3
                    sbc t2+1
                    sta product+3
                :
                lda t2+1
                bpl :+
                    sec
                    lda product+2
                    sbc t1+0
                    sta product+2
                    lda product+3
                    sbc t1+1
                    sta product+3
                :

                rts
.endproc

calculate_table .proc

      ldx #$00
      txa
      .byte $c9   ; CMP #immediate - skip TYA and clear carry flag
lb1  
	 tya
      adc #$00
ml1  
		sta table.square1_hi,x
      tay
      cmp #$40
      txa
      ror
ml9  
	  adc #$00
      sta ml9+1
      inx
ml0  
	 sta table.square1_lo,x
      bne lb1
      inc ml0+2
      inc ml1+2
      clc
      iny
      bne lb1
	  
	ldx #$00
	ldy #$ff
-
   lda table.square1_hi+1,x
   sta table.square2_hi+$100,x
   lda table.square1_hi,x
   sta table.square2_hi,y
   lda table.square1_lo+1,x
   sta table.square2_lo+$100,x
   lda table.square1_lo,x
   sta table.square2_lo,y
   dey
   inx
bne -

	rts
	
.endproc
	  
main .proc

	;	.............................................  byte
	
	;jsr calculate_table
	
		lda #8
		sta t1
		lda #9
		sta t2
		
		clc
		jsr multiply_8bit_unsigned
		
		lda product+0
		ldy product+1

		jsr std.print_u8_dec


		
	;	.............................................  word
	
 
	
	;	.............................................  real

 
	rts

.endproc

;	................................................................... multiplication table
;
; 	tabella per moltiplicazioni u8 s8 u16 s16 ( 2k )
;

imul       := range(0,512)

table .namespace

	square1_lo	.byte <((imul*imul)/4)
	square1_hi	.byte >((imul*imul)/4)
	square2_lo	.byte <(((imul-255)*(imul-255))/4)
	square2_hi	.byte >(((imul-255)*(imul-255))/4) 

.endnamespace


		


