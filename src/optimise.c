
#include "../lib/cxx.h"
#include "Cmos.h"

// ******************
// OPTMISIZE
// ******************

/*

// #0 main.c

    option    :    -$ c000:ff    -> -$ (pageZero):(size)

// #1 codegen.c

    void cgBinOp_eToken_mul_u8s8    ( assembler_t *pASM, int level )     :    jsr math.mul_u8s8     ->    jsr math.mul_u8s8_fast
    
    void cgBinOp_eToken_mul_u16s16  ( assembler_t *pASM, int level )    :    jsr math.mul_u16s16    ->    jsr math.mul_u16s16v_fast
    
    case nTypePrefix :

        case eToken_not:

        if (( pNode->pnPrefix->pLeft->baseType == eTermType_u8  )
        ||    ( pNode->pnPrefix->pLeft->baseType == eTermType_s8  ))        :    jsr math.not_u8s8    ->    math_not_u8s8_fast

        if (( pNode->pnPrefix->pLeft->baseType == eTermType_u16  )
        ||    ( pNode->pnPrefix->pLeft->baseType == eTermType_s16  ))        :    jsr math.not_u16s16    ->    math_not_u16s16_fast

        if ( pNode->pnPrefix->pLeft->baseType == eTermType_r40  )        :    jsr math.not_f40    ->    jsr math.not_f40_fast
        
//    #2    scanner.c

        optimise prefix constant ( - ! + )
        
                    case eToken_subu:
                    
                    case eToken_not:
                    
    #3 ast.c
    
        optimise mul by 1
        
            astMakeNodeBinOp    :    
            
            // dopo : aver canonicalizzato il puntatore e applica l'aritmetica, diversamente ritorna il nodo binario creato
            
            if ( fDoNotEmitMulBy1 ) ...
*/