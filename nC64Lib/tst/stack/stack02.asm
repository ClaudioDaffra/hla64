
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
	
	lda stack.sp
	pha

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
	
	lda #size(local_stack)
	jsr stack.alloc

	jsr stack.get_sp_ay
	jsr basic.print_unsigned_integer	;	### 3) print size top stack - after alloc

	; .......................................................
	
	lda #' '
	jsr basic.print_byte

	jsr stack.get_bp_ay
	jsr basic.print_unsigned_integer	;	### 4) print size base stack - after alloc

	; ....................................................... GET local_stack STRUCT
	
	lda #' '
	jsr basic.print_byte
	
	lda #local_stack.d
	clc
	adc #size(local_stack.d)
	ldy #0
	jsr basic.print_unsigned_integer

	; ....................................................... GET local_stack position

	lda #' '
	jsr basic.print_byte
	
	lda #local_stack.d
	clc
	adc #size(local_stack.d)
	jsr stack.get_bp_offset_pos_x			; 	stack.get_local_stack_offset_position
	txa
	ldy #0
	jsr basic.print_unsigned_integer

	; ....................................................... GET local_stack absolute address

	lda #' '
	jsr basic.print_byte

	lda #local_stack.d
	clc
	adc #size(local_stack.d)
	jsr stack.get_bp_offset_absolute_ay		; 	stack.get_local_stack_absolute_address
	jsr basic.print_unsigned_integer
	
	lda #'='
	jsr basic.print_byte
	
	;; ###
	lda #(local_stack.d+size(local_stack.d))
	jsr stack.get_bp_offset_absolute_ay		; 	stack.get_local_stack_absolute_address
	jsr basic.print_unsigned_integer
	;; ###
	

	; ....................................................... RESTORE STACK

	pla
	sta stack.sp
	sta stack.bp

	rts

.endproc




