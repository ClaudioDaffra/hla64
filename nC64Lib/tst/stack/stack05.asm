
; generato da High Level Assembler

.include "../../lib/libCD.asm"

.PROGRAM
			
.include "../../lib/libC64.asm"

gByte01	.byte 12
gWord01	.word 3456

kLocalAsmReal00                     .byte  $81,$33,$33,$33,$33	;	// 1.4
kLocalAsmReal01                     .byte  $82,$4c,$cc,$cc,$cc	;   // 3.2
temp			                    .byte  0,0,0,0,0			;   //

; ....................................................... f1

f1 .proc

	local_stack .struct 
		x       .byte ?
		y       .byte ?
	.endstruct 
	.cerror(size(local_stack)>stack.size)

	.weak
		x	=	local_stack.x
		y	=	local_stack.y
	.endweak
	
	stack_begin
	stack_alloc local_stack

	;

	stack_dealloc local_stack
	stack_end
	
	rts
.endproc

; ....................................................... f2

f2 .proc

	local_stack .struct 
		a       .byte ?
		c       .word ?
		b       .byte ?,?,?,?,?				
	.endstruct 
	.cerror(size(local_stack)>stack.size)
 
	.weak
		a	=	local_stack.a
		c	=	local_stack.c
		b	=	local_stack.b
	.endweak
	
	stack_begin
	stack_alloc local_stack
 
		stack_load_offset_x a
		stack_load_byte
		jsr basic.print_unsigned_char
		
		lda #' '
		jsr kernel.chrout
	
		stack_load_offset_x c
		stack_load_word
		jsr basic.print_unsigned_integer
		
		lda #' '
		jsr kernel.chrout
	
		stack_load_address	b
		jsr basic.load5_fac1
		jsr basic.print_fac1
	
		; ..................................	stack_copy_from_offset_to_mem
		
		load_address_zpWord1	temp
		stack_load_address	b
		stack_load_real

		load_address_ay	temp
		jsr basic.load5_fac1
		jsr basic.print_fac1
		
		; ..................................
		
		
	stack_dealloc local_stack
	stack_end
	
	rts
.endproc

; ....................................................... main

main .proc

	local_stack .struct 
		a       .byte ?         			;1	 1	0	255-1	254	49662
		b       .byte ?,?,?,?,? 			;5	 6	1	254-5	249 49657
		c		.word ?						;2	 8	6	249-2	247 49655
		d		.sint ?						;2	10  8	247-2	245 49653
	.endstruct 
	.cerror(size(local_stack)>stack.size)

	; ....................................................... ## SAVE BEGIN
	
	stack_begin
	stack_alloc local_stack

	; .......................................................
	param_begin

		lda #123
		jsr stack.push.byte

		load_imm_ay 456
		jsr stack.push.word

		load_address_zpWord0	kLocalAsmReal01
		
		jsr stack.push.real
	
	param_end
		
	jsr f2 ;	call(f2)

	; ............................................
	

	; ....................................................... ## STACK END

	stack_dealloc local_stack
	stack_end

	rts

.endproc




