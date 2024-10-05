

*       = PROGRAM_ADDRESS
        .word (+), 2024  ;pointer, line number
        .null $9e, format("%4d", start)
+       .word 0          ;basic line end

*       = PROGRAM_ADDRESS+12
        start
            jmp cd.main

fnStackSize .var 0

.include "../nC64Lib/lib/libCD.asm"

PROGRAM_STACK_ADDRESS    =    $c100
PROGRAM_STACK_SIZE       =    $ff

.include "../nC64Lib/lib/libC64.asm"

    ; RC[  4/  5]

        intero       .sint ?
        gppluto      .sint ?
        pippo        .char ?

    ; RC[  4/  5]
    cd .namespace
        
        strutturablobale .struct
            gx_byte_1    .byte   ?
            gy_word_2    .word   ?
            gz_real_5    .byte   ?
            aaaa         .sint   ?
        .endstruct

        ; RC[ 15/  5]

            ; .........................................    sub
            sub .proc

            stack_begin

            localStack .struct
                    x            .byte ?,?,?,?,?
                    px           .byte ?,?,?,?,?
                    pw           .word ?
                    pluto1       .word ?
            .endstruct

            .weak
                    x            =  localStack.x           
                    px           =  localStack.px          
                    pw           =  localStack.pw          
                    pluto1       =  localStack.pluto1      
            .endweak

            stack_alloc localStack
            fnStackSize .var fnStackSize + size(localStack)
            .cerror(fnStackSize>stack.size)

            ; RC[ 20/  9]
            .block

                ; RC[ 22/ 17]
                ; RC[ 22/ 10]
                stack_load_address pluto1
                jsr stack.push.word
                ; RC[ 22/ 22]
                ; RC[ 22/ 20]
                stack_load_address pw
                pha
                tya
                pha
                ; RC[ 22/ 22]
                ; RC[ 22/ 22]
                ; RC[ 22/ 23]
                lda #01
                pha
                ; RC[ 22/ 22]
                lda #02
                tay
                pla
                jsr math.mul_u8s8
                cast_from_u8_to_u16
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr math.add_u16s16
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord

            .endblock

            fnStackSize .var fnStackSize - size(localStack)
            .cerror(fnStackSize<0)
            stack_end

            rts

            .endproc

        ; RC[ 25/  5]

            ; .........................................    fff
            fff .proc

            stack_begin

            localStack .struct
                    pw           .word ?
                    pb           .byte ?
                    pr           .byte ?,?,?,?,?
                    lb           .byte ?
                    lr           .byte ?,?,?,?,?
                    ptrw         .word ?
                    provaW       .word ?
                    multiblocint .sint ?
            .endstruct

            .weak
                    pw           =  localStack.pw          
                    pb           =  localStack.pb          
                    pr           =  localStack.pr          
                    lb           =  localStack.lb          
                    lr           =  localStack.lr          
                    ptrw         =  localStack.ptrw        
                    provaW       =  localStack.provaW      
                    multiblocint =  localStack.multiblocint
            .endweak

            stack_alloc localStack
            fnStackSize .var fnStackSize + size(localStack)
            .cerror(fnStackSize>stack.size)

            ; RC[ 30/  9]
            .block

                ; RC[ 35/ 17]
                ; RC[ 35/  9]
                lda gppluto+0
                ldy gppluto+1
                pha
                tya
                pha
                ; RC[ 35/ 17]
                ; RC[ 35/ 17]
                ; RC[ 35/ 19]
                lda #01
                pha
                ; RC[ 35/ 17]
                lda #02
                tay
                pla
                jsr math.mul_u8s8
                cast_from_u8_to_u16
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr math.add_u16s16
                ; RC[ 37/ 12]
                ; RC[ 37/  9]
                stack_load_address pw
                jsr stack.push.word
                ; RC[ 37/ 15]
                ; RC[ 37/ 16]
                stack_load_address ptrw
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord
                ; RC[ 39/ 16]
                ; RC[ 39/  9]
                stack_load_address provaW
                jsr stack.push.word
                ; RC[ 39/ 19]
                ; RC[ 39/ 22]
                lda #01
                ldy #00
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord
                ; RC[ 41/ 13]
                
			stack_load_address provaW
			stack_load_word
			jsr std.print_u16_dec
		
                ; RC[ 48/ 13]
                .block

                    ; RC[ 49/ 17]
                    .block

                        ; RC[ 51/ 30]
                        ; RC[ 51/ 17]
                        stack_load_address multiblocint
                        jsr stack.push.word
                        ; RC[ 51/ 33]
                        lda #01
                        pha
                        jsr stack.pop.word
                        sta zpWord0+0
                        sty zpWord0+1
                        pla
                        jsr store_pByte

                    .endblock

                .endblock

            .endblock

            fnStackSize .var fnStackSize - size(localStack)
            .cerror(fnStackSize<0)
            stack_end

            rts

            .endproc

        ; RC[ 59/  5]

            ; .........................................    main
            main .proc

            stack_begin

            localStack .struct
                    plutow       .word ?
                    plutob       .byte ?
                    pword        .word ?
                    uw16         .word ?
                    pbyte        .byte ?
                    risbyte      .byte ?
                    p2           .null ?
                    rr1          .null ?
            .endstruct

            .weak
                    plutow       =  localStack.plutow      
                    plutob       =  localStack.plutob      
                    pword        =  localStack.pword       
                    uw16         =  localStack.uw16        
                    pbyte        =  localStack.pbyte       
                    risbyte      =  localStack.risbyte     
                    p2           =  localStack.p2          
                    rr1          =  localStack.rr1         
            .endweak

            stack_alloc localStack
            fnStackSize .var fnStackSize + size(localStack)
            .cerror(fnStackSize>stack.size)

            ; RC[ 61/  9]
            .block

                
                point .struct
                    x            .sint   ?
                    y            .char   ?
                .endstruct
                
                rect .struct
                    a            .sint   ?
                    b            .byte   ?
                    c            .char   ?
                .endstruct
                ; RC[ 86/ 12]
                ; RC[ 86/  9]
                stack_load_address rr1
                pha
                tya
                pha
                ; RC[ 86/ 12]
                ; RC[ 86/ 12]
                ; RC[ 86/ 13]
                lda #02
                pha
                ; RC[ 86/ 12]
                lda #08
                tay
                pla
                jsr math.mul_u8s8
                cast_from_u8_to_u16
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr math.add_u16s16
                ; RC[ 88/ 15]
                ; RC[ 88/  9]
                stack_load_address pword
                jsr stack.push.word
                ; RC[ 88/ 20]
                ; RC[ 88/ 18]
                stack_load_address p2
                sta zpWord0+0
                sty zpWord0+1
                clc
                lda #02
                adc zpWord0+0
                sta zpWord0+0
                lda #00
                tay
                adc zpWord0+1
                sta zpWord0+1
                lda (zpWord0),y
                tax
                iny
                lda (zpWord0),y
                tay
                txa
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord
                ; RC[ 90/ 13]
                
            nop
            nop
            nop
        
                ; RC[ 96/ 14]
                ; RC[ 96/ 11]
                ; RC[ 96/  9]
                stack_load_address p2
                sta zpWord0+0
                sty zpWord0+1
                clc
                lda #02
                adc zpWord0+0
                sta zpWord0+0
                lda #00
                tay
                adc zpWord0+1
                sta zpWord0+1
                jsr stack.push.word
                ; RC[ 96/ 17]
                lday #1235
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord
                ; RC[ 98/ 16]
                ; RC[ 98/  9]
                stack_load_address plutob
                jsr stack.push.word
                ; RC[ 98/ 19]
                lda #123
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                jsr store_pByte
                ; RC[100/ 13]
                 ;49660 
 			stack_load_address plutob
			jsr std.print_u16_dec
		
                ; RC[104/ 13]
                 ; 123
 			stack_load_address plutob
			stack_load_byte
			jsr std.print_u8_dec
		
                ; RC[110/ 15]
                ; RC[110/  9]
                stack_load_address pbyte
                jsr stack.push.word
                ; RC[110/ 18]
                ; RC[110/ 19]
                stack_load_address plutob
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord
                ; RC[112/ 13]
                 ; 49655
 			stack_load_address pbyte
			jsr std.print_u16_dec
		
                ; RC[116/ 13]
                 ;  49660
 			stack_load_address pbyte
			stack_load_word
			jsr std.print_u16_dec
		
                ; RC[122/ 17]
                ; RC[122/  9]
                stack_load_address risbyte
                jsr stack.push.word
                ; RC[122/ 20]
                ; RC[122/ 21]
                stack_load_address pbyte
                jsr mem.load.word_ptr
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord
                ; RC[124/ 13]
                 ; 49654
 			stack_load_address risbyte
			jsr std.print_u16_dec
		
                ; RC[128/ 13]
                 ; 123
 			stack_load_address risbyte
			stack_load_byte
			jsr std.print_u8_dec
		
                ; RC[134/ 14]
                ; RC[134/  9]
                stack_load_address uw16
                jsr stack.push.word
                ; RC[134/ 23]
                ; RC[134/ 17]
                stack_load_address pbyte
                pha
                tya
                pha
                ; RC[134/ 23]
                ; RC[134/ 23]
                ; RC[134/ 25]
                lda #01
                pha
                ; RC[134/ 23]
                lda #01
                tay
                pla
                jsr math.mul_u8s8
                cast_from_u8_to_u16
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr math.add_u16s16
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord
                ; RC[136/ 13]
                 ; 49656
 			stack_load_address uw16
			stack_load_word
			jsr std.print_u16_dec
		
                ; RC[142/ 15]
                ; RC[142/  9]
                stack_load_address pbyte
                jsr stack.push.word
                ; RC[142/ 18]
                ; RC[142/ 19]
                stack_load_address plutob
                pha
                tya
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr store_pWord
                ; RC[144/ 13]
                 ; 44
			nop 
			nop  
			nop 
		
                ; RC[150/ 16]
                ; RC[150/  9]
                ; RC[150/ 10]
                stack_load_address pbyte
                
                stack_load_word ;
                jsr stack.push.word
                ; RC[150/ 20]
                lda #44
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                jsr store_pByte
                ; RC[152/ 13]
                 ;; 44
 			stack_load_address plutob
			stack_load_byte
			jsr std.print_u8_dec
		
                ; RC[158/ 21]
                ; RC[158/  9]
                ; RC[158/ 15]
                stack_load_address pbyte
                cast_from_u16_to_s16
                pha
                tya
                pha
                ; RC[158/ 21]
                ; RC[158/ 23]
                lda #02
                cast_from_u8_to_s16
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr math.add_u16s16
                ; RC[160/  9]
                ; RC[160/ 20]
                ; RC[160/ 20]
                ; RC[160/ 18]
                lda #01
                cast_from_u8_to_real
                jsr stack.push.real_c64
                ; RC[160/ 27]
                ; RC[160/ 29]
                ldfac1             kLocalAsmReal00
                jsr math.not_f40
                jsr stack.pop.real_c64_to_fac2
                jsr math.add_f40
                cast_from_real_to_u8
                ; RC[162/ 13]
                 
			nop 
			nop  
			nop 
		
                ; RC[168/ 20]
                ; RC[168/  9]
                ; RC[168/ 16]
                ; RC[168/ 11]
                stack_load_address pbyte
                pha
                tya
                pha
                ; RC[168/ 16]
                ; RC[168/ 16]
                ; RC[168/ 17]
                lda #02
                pha
                ; RC[168/ 16]
                lda #01
                tay
                pla
                jsr math.mul_u8s8
                cast_from_u8_to_u16
                sta zpWord0+0
                sty zpWord0+1
                pla
                tay
                pla
                jsr math.add_u16s16
                
                stack_load_word ;
                jsr stack.push.word
                ; RC[168/ 24]
                lda #44
                pha
                jsr stack.pop.word
                sta zpWord0+0
                sty zpWord0+1
                pla
                jsr store_pByte

            .endblock

            fnStackSize .var fnStackSize - size(localStack)
            .cerror(fnStackSize<0)
            stack_end

            rts

            .endproc
    .endnamespace


	kLocalAsmReal00                     .byte  $81,$00,$00,$00,$00 ;         1

