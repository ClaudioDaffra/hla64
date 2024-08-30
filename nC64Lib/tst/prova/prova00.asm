
.include "../../lib/libCD.asm"

* = $801
    jmp main

.include "../../lib/libC64.asm"


 
sub1 .proc

     sta zpWord0
     sty zpWord0+1
     lda (zpWord0),y
     rts
     
.endproc

 
sub2 .proc
    
     lda $ff
     rts

.endproc

sub3 .proc
    
     sta zpWord0
     sty zpWord0+1
     lda (zpWord0),y
     txa
     iny
     lda (zpWord0),y
     tax
     tya
     rts

.endproc

main .proc

    jsr sub1 
    
    jsr sub2

    jsr sub3
    
	rts

.endproc




