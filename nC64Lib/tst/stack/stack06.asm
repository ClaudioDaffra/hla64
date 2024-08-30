

; generato da High Level Assembler

.include "../nC64Lib/lib/libCD.asm"

.PROGRAM

.include "../nC64Lib/lib/libC64.asm"

fnStackSize .var 0

	    greal3               .byte 0,0,0,0,0
	    gword2               .word 0
	    gbyte1               .byte 0


	    ; 	........................................ subvoid1

	    subvoid1 .proc

		    rts

	    .endproc

	    ; 	........................................ subvoid2

	    subvoid2 .proc

		    .block

		    stack_begin

		    B_local_stack .struct
			    beta                 .sint 0 
			    alpha                .sint 0 
		    .endstruct
		    stack_alloc B_local_stack
		    fnStackSize .var fnStackSize + size(B_local_stack)
		    .cerror(fnStackSize>stack.size)

		    .weak
			    beta                 = B_local_stack.beta
			    alpha                = B_local_stack.alpha
		    .endweak

		    .endblock

		    rts

	    .endproc

	    ; 	........................................ sub

	    sub .proc

		    stack_begin

		    F_local_stack .struct
			    y                    .sint 0 
			    x                    .sint 0 
		    .endstruct
		    stack_alloc F_local_stack
		    fnStackSize .var fnStackSize + size(F_local_stack)
		    .cerror(fnStackSize>stack.size)

		    .weak
			    y                    = F_local_stack.y
			    x                    = F_local_stack.x
		    .endweak

		    lda gbyte1

		    lda gword2+0
		    ldy gword2+1

		    load_address_ay greal3+1
		    jsr basic.load5_fac1


		    stack_end

		    fnStackSize .var fnStackSize - size(F_local_stack)

		    rts

	    .endproc

	    ; 	........................................ main

	    main .proc

		    stack_begin

		    F_local_stack .struct
			    pizza                .sint 0 
		    .endstruct
		    stack_alloc F_local_stack
		    fnStackSize .var fnStackSize + size(F_local_stack)
		    .cerror(fnStackSize>stack.size)

		    .weak
			    pizza                = F_local_stack.pizza
		    .endweak

		    .block

		    B_local_stack .struct
			    lreal30              .byte 0,0,0,0,0 
			    lword20              .word 0 
			    lbyte10              .byte 0 
		    .endstruct
		    stack_alloc B_local_stack
		    fnStackSize .var fnStackSize + size(B_local_stack)
		    .cerror(fnStackSize>stack.size)

		    .weak
			    lreal30              = B_local_stack.lreal30
			    lword20              = B_local_stack.lword20
			    lbyte10              = B_local_stack.lbyte10
		    .endweak

		    lday #01

		    ldfac1 kLocalAsmReal00

		    stack_load_address lbyte10
		    stack_load_byte

		    stack_load_address lword20
		    stack_load_word

		    stack_load_address lreal30
		    jsr basic.load5_fac1

		    fnStackSize .var fnStackSize - size(B_local_stack)

		    .endblock

		    stack_end

		    fnStackSize .var fnStackSize - size(F_local_stack)

		    rts

	    .endproc


	    kLocalAsmReal00                     .byte  $82,$4c,$cc,$cc,$cc

