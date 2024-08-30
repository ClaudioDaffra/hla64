
.include "../../lib/libCD.asm"

.PROGRAM
			
.include "../../lib/libC64.asm"


kLocalAsmReal00                     .byte  $81,$33,$33,$33,$33	;	// 1.4
kLocalAsmReal01                     .byte  $82,$4c,$cc,$cc,$cc	;   // 3.2
temp			                    .byte  0,0,0,0,0			;   //
		
main .proc

	; ## LOCAL STACK
	local_stack .struct 
		a       .byte 0         ;1	 1	0	255-1	254	49662
		b       .byte 0,0,0,0,0 ;5	 6	1	254-5	249 49657
		c		.word 0			;2	 8	6	249-2	247 49655
		d		.sint 0			;2	10  8	247-2	245 49653
	.endstruct 

	; ....................................................... ## SAVE BEGIN
	
	stack_begin
	
	jsr stack.get_sp_ay
	jsr basic.print_unsigned_integer
	
	stack_alloc local_stack

	lda #' '
	jsr kernel.chrout

	; .......................................................
	
	stack_load_offset_x local_stack.c
	jsr basic.print_unsigned_char

	; .......................................................	load/store word

	lda #' '
	jsr kernel.chrout
	
	stack_load_offset_x local_stack.c
	; store word	; $04D2	210 004
	stack_store_word 1234

	stack_load_offset_x local_stack.c

	stack_load_word

	jsr basic.print_unsigned_integer
	
	; .......................................................	load/store byte

	lda #' '
	jsr kernel.chrout
	
	stack_load_offset_x local_stack.a
	; store btye	; $0c	12 
	stack_store_byte 12

	stack_load_byte

	jsr basic.print_unsigned_char

	; .......................................................	store real5 imm inz stack

	lda #' '
	jsr kernel.chrout

	; zpWord0 	= source
	load_address_zpWord0	kLocalAsmReal00
	; ay		=	dest
	stack_load_address 		local_stack.b
	; zpWord1	= dest
	stack_store_real

	stack_load_address 		local_stack.b
	jsr basic.load5_fac1
	jsr basic.print_fac1

	; ....................................................... load real5 form stsck

	; .......................................................
	
	lda #' '
	jsr kernel.chrout

	; zpWord1	= dest
	load_address_zpWord1	temp
	; ay		= sourcce
	stack_load_address 		local_stack.b
	
	stack_load_real

	; BUG ? stack in ZERO PAGE
	; se usi pagina zero 1.40002432 alrimenti 1.4 
	
	load_address_ay	temp
	jsr basic.load5_fac1
	jsr basic.print_fac1
	
	; ....................................................... ## STACK END

	stack_dealloc local_stack

	lda #' '
	jsr kernel.chrout
	
	jsr stack.get_sp_ay
	jsr basic.print_unsigned_integer
	
	stack_end

	rts

.endproc




