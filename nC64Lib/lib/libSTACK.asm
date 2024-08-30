
;**********
;           STACK
;**********

;    stack.sp        :    stack pointer top
;    stack.bp        :    base pointer
;    stack.address   :    stack address bottom
;    stack.size      :    stack size

; -------------------------------------------------------------- macro
;
;    stack.alloc sp,bp
;    
; -------------------------------------------------------------- macro

stack_begin .macro 
    lda stack.sp
    pha
.endm

stack_end .macro 
    pla
    sta stack.sp
    sta stack.bp
.endm

stack_alloc .macro 
    lda #size(\1)
    jsr stack.alloc
.endm

stack_dealloc .macro 
    lda #size(\1)
    jsr stack.dealloc
.endm

stack_load_offset_x .macro 
    lda #(\1+size(\1))
    jsr stack.get_bp_offset_pos_x
.endm

stack_load_address .macro 
    lda #(\1+size(\1))
    jsr stack.get_bp_offset_absolute_ay
.endm

stack_store_word    .macro
    lda #<\1
    sta stack.address+0,x
    lda #>\1
    sta stack.address+1,x
.endm

stack_load_word    .macro
    lda stack.address+0,x
    ldy stack.address+1,x
.endm

stack_store_byte    .macro
    lda #\1
    sta stack.address+0,x
.endm

stack_load_byte    .macro
    lda stack.address+0,x
.endm

stack_store_real    .macro
    jsr stack.set.real
.endm

stack_load_real    .macro
    jsr stack.get.real
.endm

; -------------------------------------------------------------- param
;
; hanna la funzione di preservare i riferimenti allo stack
param_begin    .macro
    lda stack.sp
    pha
    lda    stack.bp
    pha
.endmacro
;    qui i riferimenti verranno modificati eseguendo il push dei parametri
param_end    .macro
    pla
    sta stack.bp
    pla
    sta    stack.sp
.endmacro

; vengono ripristinate le condizioni iniziale
; tuttavia quando si chiamerà la nuova funzione , questa allochera il nuovo stack
; tenendo conto dei parametri passati e anche dei parametri nei suoi stack locali 
; call(function)

; -------------------------------------------------------------- stack

stack    .proc

    ; ---------------------------------------------------------- stack.address
    .weak
        address    = PROGRAM_STACK_ADDRESS    ;    49408+255=49663 ; libCD.asm
        size    = PROGRAM_STACK_SIZE    ;    $c100+$ff=$c1ff ; libCD.asm
    .endweak

    ; ---------------------------------------------------------- stack.sp
    sp    .byte stack.size
    ; ---------------------------------------------------------- stack.bp
    bp    .byte stack.size
    ; ---------------------------------------------------------- stack.alloc
    ; stack size 10 255
    ;            sp 255 -245
    ;            bp 255 =255
    alloc    .proc
        sta zpa
        lda stack.sp
        sta    stack.bp
        sec
        sbc zpa
        sta stack.sp
        rts
    .endproc
    ; ---------------------------------------------------------- stack.dealloc
    ; stack size 10 255
    ;            sp 245    +255
    ;            bp 255 =255
    dealloc    .proc
        sta zpa
        lda stack.sp
        clc
        adc zpa
        sta stack.sp
        sta    stack.bp        
        rts
    .endproc
    ; ---------------------------------------------------------- stack.address ( TOP )
    get_sp_ay    .proc
        lda stack.sp
        ldy #>stack.address
        rts
    .endproc
    ; ---------------------------------------------------------- stack.address ( BASE )
    get_bp_ay    .proc
        lda stack.bp
        ldy #>stack.address
        rts
    .endproc
    ; ---------------------------------------------------------- stack.base.offset ( BASE )
    get_bp_offset_pos_x    .proc
        sta zpa
        lda stack.bp
        sec 
        sbc zpa
        tax
        rts
    .endproc
    ; ---------------------------------------------------------- stack.base.absolute ( BASE )
    get_bp_offset_absolute_ay    .proc
        jsr get_bp_offset_pos_x
        ldy #>stack.address
        rts
    .endproc

    ; ---------------------------------------------------------- store
    ; input : 
    ;    zpWord0 source
    ;    ay         dest
    ; usage :
    ;     load_address_zpWord0    kLocalAsmReal00
    ;     stack_load_address         local_stack.b
    
    set    .proc
        real    .proc
            sta zpWord1+0
            sty zpWord1+1
            lda #5
            jsr mem.copy.short
            rts
        .endproc
    .endproc

    ; swap zpWord0 zpWord1
    
    get    .proc
        real    .proc
            sta zpWord0+0
            sty zpWord0+1
            lda #5
            jsr mem.copy.short
            rts
        .endproc
    .endproc
    
    ; ---------------------------------------------------------- push
    push    .proc
    ; ---------------------------------------------------------- push.byte
    ;    input    : a
        byte    .proc
            ldx stack.sp
            dex            
            sta stack.address+0,x
            stx stack.sp
            rts
        .endproc
    ; ---------------------------------------------------------- push.word 
    ;    input    : ay
        word    .proc
            ldx stack.sp
            dex
            dex
            sta stack.address+0,x
            tya
            sta stack.address+1,x
            stx stack.sp
            rts
        .endproc
    ; ---------------------------------------------------------- push.real
    ; input :    zpWord0 ( source ) float40
    ;            zpWord1 ( dest   )
    ;
        real    .proc  

            lda stack.sp          ;    alloca 5 byte per un float40
            sec
            sbc #5                ;    ++sp
            sta stack.sp

            sta zpWord1+0         ;    set dest ( zpWord1 ) 
            ldy #>stack.address
            sty zpWord1+1

            lda #5                ;    copia 5 byte sullo stack
            jsr mem.copy.short
            
            rts
            
        .endproc
        
        real_c64    .proc  

            pla            ; backup return address
            sta zpa
            pla
            sta zpy
            
            lda $61
            pha
            lda $62
            pha
            lda $63
            pha
            lda $64
            pha
            lda $65
            pha
            lda $66
            pha
            
            lda zpy        ; restore return address
            pha
            lda zpa
            pha
            
            rts
            
        .endproc

        real_c64_to_fac2    .proc  

            pla            ; backup return address
            sta zpa
            pla
            sta zpy
            
            lda 105
            pha
            lda 106
            pha
            lda 107
            pha
            lda 108
            pha
            lda 109
            pha
            lda 110
            pha
            
            lda zpy        ;    restore return address
            pha
            lda zpa
            pha
            
            rts
            
        .endproc
        
    .endproc

    ; ---------------------------------------------------------- pop
    pop        .proc

    ; ---------------------------------------------------------- pop.byte    : out a
        byte    .proc
            ldx stack.sp
            lda stack.address,x
            inx                        ;    --sp                
            stx stack.sp
            rts
        .endproc
    ; ---------------------------------------------------------- pop.word    : out ay
        word    .proc
            ldx stack.sp
            ldy stack.address+1,x
            lda stack.address+0,x
            inx                        ;    --sp
            inx                        
            stx stack.sp
            rts
        .endproc
    ; ---------------------------------------------------------- pop.real    : out fac1
        real    .proc
            jsr stack.get_sp_ay        ;    get top of stack    
            jsr basic.store_fac1            
            lda stack.sp            ;    --sp
            clc                                    
            adc #5
            sta stack.sp
            rts
        .endproc

        real_c64    .proc  
        
            pla            ; backup return address
            sta zpa
            pla
            sta zpy
            
            pla
            sta $66
            pla
            sta $65
            pla
            sta $64
            pla
            sta $63
            pla
            sta $62
            pla
            sta $61
            
            lda zpy        ;    restore return address
            pha
            lda zpa
            pha
            
            rts
        .endproc

        real_c64_to_fac2    .proc  
        
            pla            ; backup return address
            sta zpa
            pla
            sta zpy
            
            pla
            sta 110
            pla
            sta 109
            pla
            sta 108
            pla
            sta 107
            pla
            sta 106
            pla
            sta 105
            
            lda zpy        ;    restore return address
            pha
            lda zpa
            pha
            
            rts
            
        .endproc
        
    .endproc

    ; ---------------------------------------------------------- stack.load
    load    .proc
    
    ; ---------------------------------------------------------- stack.load.address_ay
        address_ay .proc
            sta zpa
            lda stack.bp
            clc
            adc zpa
            ldy #>stack.address
            rts
        .endproc
    ; ---------------------------------------------------------- stack.load.address_zpWord0
        address_zpWord0 .proc
            jsr stack.load.address_ay
            sta    zpWord0+0
            sty zpWord0+1
            rts
        .endproc
    ; ---------------------------------------------------------- stack.load.address_zpWord1
        address_zpWord1 .proc
            jsr stack.load.address_ay
            sta    zpWord1+0
            sty zpWord1+1
            rts
        .endproc

    .endproc

.endproc

