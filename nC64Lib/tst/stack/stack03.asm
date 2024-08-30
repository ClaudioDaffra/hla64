
.include "../../lib/libCD.asm"

.PROGRAM
			
.include "../../lib/libC64.asm"


kLocalAsmReal00                     .byte  $81,$33,$33,$33,$33	;	// 1.4
kLocalAsmReal01                     .byte  $82,$4c,$cc,$cc,$cc	;   // 3.2
		
main .proc

	; ## LOCAL STACK
	local_stack .struct 
		a       .byte 0         ;1	 1	0
		b       .byte 0,0,0,0,0 ;5	 6	1
		c		.word 0			;2	 8	6
		d		.sint 0			;2	10  8
	.endstruct 

	; ....................................................... ## SAVE STACK
	
	stack_begin

	; ....................................................... 
	
	lda #'['
	jsr kernel.chrout
	
	lda #size(local_stack)
	jsr basic.print_unsigned_integer	;	### 1) print size stack

	lda #']'
	jsr kernel.chrout
	
	lda #' '
	jsr kernel.chrout
	
	jsr stack.get_sp_ay
	jsr basic.print_unsigned_integer	;	### 2) print size top stack - before alloc

	lda #' '
	jsr kernel.chrout
	
	; ....................................  ### STACK.LOCAL.ALLOC	bp=sp --sp
	
	stack_alloc local_stack

	jsr stack.get_sp_ay
	jsr basic.print_unsigned_integer	;	### 3) print size top stack - after alloc

	; .......................................................
	
	lda #petscii.nl
	jsr basic.print_byte

	jsr stack.get_bp_ay
	jsr basic.print_unsigned_integer	;	### 4) print size base stack - after alloc

	; ....................................................... GET local_stack STRUCT
	
	lda #' '
	jsr basic.print_byte
	
	lda #(local_stack.d+size(local_stack.d))
	ldy #0
	jsr basic.print_unsigned_integer

	; ....................................................... GET local_stack position x

	lda #' '
	jsr basic.print_byte
	
	stack_load_offset_x local_stack.d
	txa
	ldy #0
	jsr basic.print_unsigned_integer

	; ....................................................... GET local_stack absolute address ay

	lda #' '
	jsr basic.print_byte

	stack_load_address local_stack.d
	jsr basic.print_unsigned_integer

	; ....................................................... ## RESTORE STACK

	stack_end

	rts

.endproc




