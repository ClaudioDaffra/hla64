

;--------------------------------------------------------------- cpu, encoding

CPU6502	.macro
	.cpu  '6502'
	.enc  'none'
.endmacro

;--------------------------------------------------------------- basic program with sys call

.weak
	PROGRAM_ADDRESS	=	$0801
	PROGRAM_LINE	=	2024
.endweak 

.weak
	PROGRAM_STACK_ADDRESS	=	$c100
	PROGRAM_STACK_SIZE   	=	$ff
.endweak 

PROGRAM	.macro	_BASIC_ADDRESS=PROGRAM_ADDRESS,_LINE=PROGRAM_LINE

*       = $0801
        .word (+), 2024  ;pointer, line number
        .null $9e, format("%4d", start) 
+       .word 0          ;basic line end

*       = $080d

		start:
			jmp main

.endmacro

;--------------------------------------------------------------- global macro

lday	.macro
	lda <\1
	ldy >\1
.endmacro

load_imm_ay	.macro
	lda #<\1
	ldy #>\1
.endmacro

ldfac1	.macro
	load_imm_ay \1
	jsr basic.load5_fac1
.endmacro

ldfac2	.macro
	load_imm_ay \1
	jsr basic.load5_fac2
.endmacro

load_address_ay	.macro
	lda #<\1
	ldy #>\1
.endmacro

load_address_zpWord0	.macro
	load_imm_ay \1
	sta zpWord0+0
	sty zpWord0+1
.endmacro

load_address_zpWord1	.macro
	load_imm_ay \1
	sta zpWord1+0
	sty zpWord1+1
.endmacro

load_imm_zpWord0	.macro
	load_imm_ay \1
	sta zpWord0+0
	sty zpWord0+1
.endmacro

load_imm_zpWord1	.macro
	load_imm_ay \1
	sta zpWord1+0
	sty zpWord1+1
.endmacro

C2	.macro
		eor #255
		clc
		adc #1
.endmacro

sev .macro
    sec
    lda #$80    ; set overflow
    sbc #$01
.endm
