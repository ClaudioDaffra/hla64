
#ifndef cdASM
#define cdASM

#include <stdio.h>
#include <stdint.h>
#include "ast.h"
#include "../lib/mvector.h"
#include "../lib/mstack.h"
#include "symTable.h"
#include <assert.h>

FILE*	pFileOutputAsm    ; // usato in main.c :: pointer file assembler
char*	fileNameOutputAsm ; // usato in main.c :: file name assembler

// assembler opcode

typedef enum asmOpCode_e
{
	
	op_nop			,	// no instruction
	op_load_cInt	,	// load a constant integer into integer reg
	op_load_cReal 	,	// load a constant real into a real reg
	op_cnv2rl 		,	// converti a double il termine di sinistra / left
	op_cnv2rr 		,	// converti a double il termine di destra   / right
	op_fadd 		,	// addiziona reali
	op_fsub 		,	// sottrai reali
	op_fmul			,	// moltiplica reali
	op_fdiv			,	// dividi reali
	op_fmod			,	// modulo reali
	op_add 			,	// addiziona interi
	op_sub 			,	// sottrai interi
	op_mul			,	// moltiplica interi
	op_div			,	// dividi interi
	op_mod			,	// modulo interi
	op_pushil		,	// push left operator integer
	op_pushrl		,	// push left operator real	
    op_not 			,	// ! integer
    op_fnot 		, 	// ! real
    op_neg 			,	// - integer
    op_fneg	    	,   // - real
	op_cnv2il 		,	// converti a integer il termine di sinistra / left
	op_cnv2ir 		,	// converti a integer il termine di destra   / right
	op_store_rInt2m ,	// memorizza il registro intero nella memoria
	op_store_rReal2m,	// memorizza il registro ireale nella memoria
	op_load_rInt_m  ,	// carica il registro intero dalla memoria
	op_load_rReal_m ,	// carica il registro reale dalla memoria	
} asmOpCode_t ;

// tabella opcode

typedef struct asmOpCodeTab_s
{
	wchar_t*	value ;
} asmOpCodeTab_t ; 

#define $opcode(x) asmOpCodeTab[x].value

// 

// utilzzato dallo stack per identificare il tipo di ritorno dall'operazione

enum typeOp_e
{
	typeOpNull		,
	typeOpInteger	,
	typeOpReal		,
	typeOpID		,
	typeOpPointer	,	
} ;

typedef enum typeOp_e eTypeOp_t ;  

// questi sono i valori che può contenere un'istruzione
// utilizzato dai registri virtuali (INTERI,REALI,POINTER)

// tipo operatore registro virtuale 

struct typeOp_s
{
	eTypeOp_t	typeOp ;
	union
	{
        int64_t     integer	; 	// integer  / wchar_t  / byte
        double      real	;	// float / double
        wchar_t*	id		; 	// string
        void*		pointer ;	// generic address	
	} ;
	void* address ; // indirizzo della variabile ID
} ;

typedef struct typeOp_s 	 typeOp_t ;

typedef struct typeOp_s* 	ptypeOp_t ;

struct asmInstruction_s
{
	asmOpCode_t		opcode ;

	uint32_t		label ;

	typeOp_t lhs ;

	typeOp_t rhs ;	

} ;

typedef struct asmInstruction_s * pAsmInstruction_t ;

vectorType( pAsmInstruction_t , vInstr ) ;

stackType( ptypeOp_t , sTypeOp ) ;

// prototype

node_t* 			astAsm				(node_t* n) ;
pAsmInstruction_t 	makeInstr			(void) ;
void 				asmPrintVectorInstr	(void) ;
node_t* 			astAssemble			(node_t* n) ;

#endif



/**/
