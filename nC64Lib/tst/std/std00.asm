
.include "../../lib/libCD.asm"

.PROGRAM
			
.include "../../lib/libC64.asm"


;	-------------------------------------------------------------------------
;	-------------------------------------------------------------------------
;	-------------------------------------------------------------------------
;	-------------------------------------------------------------------------

std.print_temp_address = $0100
std.print_temp_counter .byte 0

std.print_out	.proc
	ldy std.print_temp_counter
	sta (std.print_temp_address),y
	iny
	sty std.print_temp_counter
	rts
.endproc

std.print_string	.proc
	lda #<std.print_temp_address
	ldy #>std.print_temp_address
	jsr basic.std.print_string
	ldy #0
	sty std.print_temp_counter
	rts
.endproc

; #######################################################

main .proc

jmp u16

		lda #255
		jsr std.print_u8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #255
		jsr std.print_s8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #128
		jsr std.print_u8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #128
		jsr std.print_s8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #128
		jsr std.print_u8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #127
		jsr std.print_s8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #12
		jsr std.print_u8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #12
		jsr std.print_s8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #1
		jsr std.print_u8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

		lda #1
		jsr std.print_s8_dec
 
		lda #petscii.nl
		jsr kernel.chrout

u16

		load_imm_ay 1
		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout
		
		load_imm_ay  -1
		jsr std.print_s16_dec

		lda #petscii.nl
		jsr kernel.chrout

		load_imm_ay 32767
		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout
		
		load_imm_ay 32767
		jsr std.print_s16_dec

		lda #petscii.nl
		jsr kernel.chrout

		load_imm_ay 255
		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout
		
		load_imm_ay -255
		jsr std.print_s16_dec

		lda #petscii.nl
		jsr kernel.chrout

		load_imm_ay 12
		jsr std.print_u16_dec

		lda #petscii.nl
		jsr kernel.chrout
		
		load_imm_ay -12
		jsr std.print_s16_dec

		lda #petscii.nl
		jsr kernel.chrout
		
		rts
.endproc
