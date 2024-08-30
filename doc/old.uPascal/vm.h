
#ifndef cdVM
#define cdVM

#include "asm.h"
#include <math.h>

FILE*	pFileOutputVM  		; // usato in main.c :: pointer file Virtual Machine
char*	fileNameOutputVM    ; // usato in main.c :: file name Virtual Machine


// asm.h  	: 	viene definita la struttura dell'istruzione -> struct asmInstruction_s
// asm.h	:	viene definito il vettore delle istruzioni	-> vectorType( pAsmInstruction_t , vInstr ) ;
// 				uint32_t		label 						-> indica l'indice di goto del vettore

uint32_t		vmPC ; // program counter = indice del vettore

stackType( typeOp_t, vmStack ) ; // stack 

// prototype

typeOp_t 	vmRun 		    ( void ) ;
void		vmPrintResult	( typeOp_t result ) ;



#endif
 


/**/

