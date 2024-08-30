
;**********
;           C64
;**********

;--------------------------------------------------------------- zero page

zpa     	= $02
zpx     	= $2a
zpy     	= $52

zpByte0		= 251
zpByte1		= 252
zpByte2		= 253
zpByte3		= 254

zpwa		= 251
zpwy		= 252
zpWord00	= 251
zpWord01	= 252
zpWord10	= 253
zpWord11	= 254

; zpDWord0

zpDWord0_addr = $fb

zpDWord0        = $fb
    zpWord0     = $fb
        zpWord0hi   = $fb
        zpWord0lo   = $fb+1
    zpWord1     = $fd
        zpWord1hi   = $fd
        zpWord1lo   = $fd+1
		
; zpDWord1

zpDWord1_addr = $62	;	conv_fac1_to_dWord0

zpDWord1    = $62
    zpWord2     = $62
        zpWord2hi   = $62
        zpWord2lo   = $62+1
    zpWord3     = $64
        zpWord3hi   = $64
        zpWord3lo   = $64+1

; zpDWord2

zpDWord2_addr = $03

zpDWord2    = $03
    zpWord4     = $03       ;   $B1AA, execution address of routine converting floating point to integer.
        zpWord4hi   = $03
        zpWord4lo   = $03+1
    zpWord5     = $05       ;   $B391, execution address of routine converting integer to floating point.
        zpWord5hi   = $05
        zpWord5lo   = $05+1
		
;--------------------------------------------------------------- include

.include "libMATH.asm"
.include "libKERNEL.asm"
.include "libBASIC.asm"
.include "libSTACK.asm"
.include "libSTDIO.asm"
.include "libMEM.asm"
.include "libCONV.asm"

;-rw-r--r-- 1 claudio claudio 20175 Apr 24 12:34 libArray.asm
;-rw-r--r-- 1 claudio claudio  7188 Apr 24 12:34 libBasic.asm	-
;-rw-r--r-- 1 claudio claudio 52784 May 17 12:33 libC64.asm		-
;-rw-r--r-- 1 claudio claudio 20435 Apr 24 12:34 libConv.asm	-
;-rw-r--r-- 1 claudio claudio 24541 Apr 24 12:34 libFloat.asm
;-rw-r--r-- 1 claudio claudio 21553 Apr 24 12:34 libGraph.asm
;-rw-r--r-- 1 claudio claudio 37836 Apr 24 12:34 libMath.asm	-
;-rw-r--r-- 1 claudio claudio 24255 Apr 24 12:34 libSTDIO.asm	-
;-rw-r--r-- 1 claudio claudio 42449 May 18 13:12 libStack.asm	-
;-rw-r--r-- 1 claudio claudio  9876 Apr 24 12:34 libString.asm
;-rw-r--r-- 1 claudio claudio  9876 Apr 24 12:34 libKernel.asm  -

store_pByte .proc	; source : a , dest (zpWord0)
		    ldy #0
		    sta (zpWord0),y
			rts
.endproc

store_pWord .proc	; source : ay , dest (zpWord0)
			tax
			tya
			sty zpy
		    ldy #0
			txa 
		    sta (zpWord0+0),y
			iny
		    lda zpy
		    sta (zpWord0+0),y
			rts
.endproc

store_pReal .proc	; source : fac1 , dest (zpWord0)

			lda zpWord0+0
			ldy zpWord0+1
			jsr basic.store_fac1	; memorizza fac1 in (ay)
			rts
.endproc
