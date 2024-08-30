
#include "../lib/cxx.h"
#include "Cmos.h"

// ******************
// Abstract Sintax Tree
// ******************

#include "../lib/cxx.h"
#include "Cmos.h"

//    pNode->row         = pPar->pLex->pStream->pvCharMB->data[ pPar->pLex->pvToken->data[ pPar->ndx ]->tPosStart ]->row+0 ;
//    pNode->col         = pPar->pLex->pStream->pvCharMB->data[ pPar->pLex->pvToken->data[ pPar->ndx ]->tPosStart ]->col+0 ;
//    pNode->fileName    = pPar->pLex->pStream->pCompiler->fileInputName.data ;
//    pNode->tokenMBS    = pPar->pLex->pvToken->data[pPar->ndx]->buffer.data;

//...................................................................... astDebugNode

#define astDisplayBufferOn  1
#define astDisplayBufferOff 0

void     astDebugNode( parser_t *pPar , token_t* pToken , char* msg , int flagDisplay )
{
    if ( pPar->pLex->pStream->pCompiler->fDebug==true )
    {
         fprintf ( pPar->pFileDebugNode->ptr , "\n -> %-20s ::",msg );

         if ( pToken )
         {
            fprintf ( pPar->pFileDebugNode->ptr , " RC[%3d/%3d] tTok[%03d] tNum[%03d]"
                ,pToken->row,pToken->col,pToken->type,pToken->type_num );
             if ( flagDisplay )
             {
                fprintf ( pPar->pFileDebugNode->ptr , " [%20s]", pToken->buffer.data );
             }
         }
    }
}

//...................................................................... astMakeNodeDefault

node_t*        astMakeNodeDefault( parser_t *pPar  , token_t *pToken )
{
    (void)pPar;
    
    assert(pToken!=NULL);

    node_t*    pNode = (node_t*) new ( node_t )  ;
    
    pNode->nType      = nTypeUndefined  ;
    pNode->baseType  = eTermType_null   ;
    pNode->baseKind  = eTermKind_null   ;

    // informazioni genrali del nodo, utilizzati dall'assemblatore
    pNode->pData    = pToken            ;
    
    return pNode ;
}

//...................................................................... astMakeNodeTerm

node_t*        astMakeNodeTerm( parser_t *pPar  , token_t *pToken )
{
    node_t*    pNode = astMakeNodeDefault(pPar,pToken);    

    // nodeTerm_t
    nodeTerm_t*    pNodeTerm = (nodeTerm_t*) gcMalloc( sizeof ( nodeTerm_t ) ) ;
    
    pNode->nType            = nTypeTerm ;
    pNode->pnTerm           = pNodeTerm    ;
    pNode->pnTerm->pTerm    = pToken  ;
    pNode->pnTerm->pSymbol  = NULL ;
    
    astDebugNode(pPar,pToken,"astMakeNodeTerm",astDisplayBufferOn);
    
    return pNode ;
}

//...................................................................... astMakeNodeBlock

node_t*        astMakeNodeBlock( parser_t *pPar  , token_t *pToken )
{
    node_t*    pNode = astMakeNodeDefault(pPar,pToken);    

    // nodeBlock_t
    nodeBlock_t*    pNodeBlock = (nodeBlock_t*) gcMalloc( sizeof ( nodeBlock_t ) ) ;

    pNode->nType                 = nTypeBlock ;
    pNode->pnBlock               = pNodeBlock ;
    pNode->pnBlock->bScope       = NULL ;
    
    vector_node_t*    pvNode = (vector_node_t*) gcMalloc( sizeof(vector_node_t) ) ;

    pNode->pnBlock->pvNode             = pvNode ;
    
    vectorAlloc( (*pNode->pnBlock->pvNode) , 8 ) ;

    astDebugNode(pPar,pToken,"astMakeNodeBlock",astDisplayBufferOff);
    
    return pNode ;
}

//...................................................................... astMakeFunction

node_t*        astMakeNodeFunction( parser_t *pPar  , token_t *pToken )
{
    node_t*    pNode = astMakeNodeDefault(pPar,pToken);    

    // nodeFunction_t
    nodeFunction_t*    pFunction = (nodeFunction_t*) gcMalloc( sizeof ( nodeFunction_t ) ) ;

    pNode->nType                = nTypeFunction ;
    pNode->pnFunction           = pFunction ;
    pNode->pnFunction->fName    = NULL ;
    pNode->pnFunction->fScope   = NULL ;
    pNode->pnFunction->fBlock   = NULL ;

    pNode->pnFunction->stackSize= 0 ;
    
    // qui contiene tutti i simboli LOCALI con i rispettivi tipi pronti per l'assembler 
    vector_stringFunction_t*              vsg = (vector_stringFunction_t*) new (vector_stringFunction_t)  ;
    pNode->pnFunction->pvStringFunction = vsg ;
    vectorAlloc( (*pNode->pnFunction->pvStringFunction) , 8 ) ;
    vectorClear( (*pNode->pnFunction->pvStringFunction)     ) ;

    // qui contiene tutti i simboli riferimenti .weak .endweak 
    vector_stringFunction_t*              wsg = (vector_stringFunction_t*) new (vector_stringFunction_t)  ;
    pNode->pnFunction->pvWeakFunction = wsg ;
    vectorAlloc( (*pNode->pnFunction->pvWeakFunction) , 8 ) ;
    vectorClear( (*pNode->pnFunction->pvWeakFunction)     ) ;
    
    astDebugNode(pPar,pToken,"astMakeFunction",astDisplayBufferOn);

    return pNode ;
}

//...................................................................... astMakeProgram

node_t*        astMakeProgram( parser_t *pPar  , token_t *pToken )
{
    (void)pToken ;
    
    node_t*    pNode = astMakeNodeDefault(pPar,pToken);    

    // nodeProgram_t
    nodeProgram_t*    pProgram = (nodeProgram_t*) gcMalloc( sizeof ( nodeProgram_t ) ) ;

    pNode->nType                 = nTypeProgram ;
    
    pNode->pnProgram            = pProgram ;
    //pNode->pnProgram->pScope  = NULL ;
    pNode->pnProgram->body      = NULL ; // viene inizializzato internamente in parseProgram


    // qui contiene tutti i simboli GLOBALI con i rispettivi tipi pronti per l'assembler
    vector_stringGlobal_t*                vsg = (vector_stringGlobal_t*) new (vector_stringGlobal_t)  ;
    pNode->pnProgram-> pvStringGlobal = vsg ;
    vectorAlloc( (*pNode->pnProgram-> pvStringGlobal) , 8 ) ;

    astDebugNode(pPar,pToken,"astMakeProgram",astDisplayBufferOff);

    return pNode ;
}

//...................................................................... astMakeNodeASM

node_t*        astMakeNodeASM( parser_t *pPar  , token_t *pToken )
{
    (void)pToken;
    
    node_t*    pNode = astMakeNodeDefault(pPar,pToken);    

    // node ASM
    nodeASM_t*    pNodeASM = (nodeASM_t*) gcMalloc( sizeof ( nodeASM_t ) ) ;
    
    pNode->nType             = nTypeASM ;
    pNode->pnASM             = pNodeASM    ;
    pNode->pnASM->pASM       = pToken  ;

    astDebugNode(pPar,pToken,"astMakeNodeASM",astDisplayBufferOn);

    return pNode ;
}

//...................................................................... astMakeNodePrefix

node_t*        astMakeNodePrefix( parser_t *pPar   , token_t *pToken , node_t* pLeft )
{
    node_t*    pNode = astMakeNodeDefault(pPar,pToken);

    // node Prefix
    nodePrefix_t*    pNodePrefix = (nodePrefix_t*) gcMalloc( sizeof ( nodePrefix_t ) ) ;
    
    pNode->nType                = nTypePrefix ;
    pNode->pnPrefix             = pNodePrefix    ;
    pNode->pnPrefix->pLeft       = pLeft  ;
    
    astDebugNode(pPar,pToken,"astMakeNodePrefix",astDisplayBufferOn);

    return pNode ;
}

//...................................................................... astMakeNodeBinOp

node_t*        astMakeNodeBinOp( parser_t *pPar  , token_t *pToken , node_t *left , node_t *right  , scope_t* pScope ) 
{
    node_t*    pNode = astMakeNodeDefault(pPar,pToken );    

    // node binOp
    nodeBinOp_t*    pNodeBinOp = (nodeBinOp_t*) gcMalloc( sizeof ( nodeBinOp_t ) ) ;
    
    pNode->nType             = nTypeBinOp ;
    pNode->pnBinOp           = pNodeBinOp ;
    pNode->pnBinOp->pLeft    = left  ;
    pNode->pnBinOp->pRight   = right ;

    astDebugNode(pPar,pToken,"astMakeNodeBinOp",astDisplayBufferOn);

    // canonicalizza il puntatore e applica l'aritmetica, diversamente ritorna il nodo binario creato
    // # begin
        if ( ( left->baseKind == eTermKind_ptr ) || (  right->baseKind == eTermKind_ptr ) )
        {
            if ( ( pToken->type != eToken_add )      // operatori consenti sui puntatori + - :=
            &&   ( pToken->type != eToken_sub ) 
            &&   ( pToken->type != eToken_assign ) )
            {
                $parserErrorExtra( parsing,opNotAllowed,pNode->pData->mbs ) ;
            }
        }
        if ( ( pToken->type == eToken_add ) || ( pToken->type == eToken_sub ) )
        {
        // ## canonicalize 'ptr' se ( num + ptr )     ->         segue ( ptr + num )
            if ( ( left->baseKind ==  eTermKind_const  ) && (right->baseKind == eTermKind_ptr) )
            {
                node_t *nSwap = NULL ;
                nSwap                     = pNode->pnBinOp->pLeft ;
                pNode->pnBinOp->pLeft     = pNode->pnBinOp->pRight ;
                pNode->pnBinOp->pRight     = nSwap ;
                astDebugNode(pPar,pToken,"canonicalize pointer",astDisplayBufferOn);
            }
        // ## poniter arithmentic : pa + 1(rhs)        ->        pa + 1 * sizeof(pa)
            token_t* tSave = pPar->p0;

            // ## optimise
            int fDoNotEmitMulBy1 = 0 ;
            if ( pPar->pLex->pStream->pCompiler->fOptimise == true ) 
            {
                if  ( pNode->pnBinOp->pLeft->baseType == eTermType_u8 ) fDoNotEmitMulBy1 = 1 ;
                if  ( pNode->pnBinOp->pLeft->baseType == eTermType_s8 ) fDoNotEmitMulBy1 = 1 ;
            }
            if ( fDoNotEmitMulBy1 ) 
            {
                // non emettere mul by 1 
           }
            else
            {
                node_t* ret = astMakeNodeNDX( pPar  , pToken , pNode , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight ,pScope ) ;
                pPar->p0 = tSave ;
                if ( ret != NULL ) return ret ;
            }
            
        }
    // # end
    
    return pNode ;
}

//...................................................................... astMakeNodeNDX
//
//    [v]        +        -    :=        *        &        
//    [x]        ++       --        
//
//
// questo nodo funziona da indice, viene prelevato il size dal simbolo
// e molticplicato per il valore se questo è un puntatore
// int a[3] ->                 a+3*sizeof(int)
// int pa   -> pa+3     ->    a+3*sizeof(int)
//

token_t* astNewTokenCopyFrom( parser_t *pPar, token_t* pCopyToken )
{
    (void)pPar;
    
    token_t* pToken = (token_t*) gcCalloc( sizeof(token_t) ,1);

    stringAlloc ( pToken->buffer , 8 ) ; //  buffer temporaneo di salvataggio strcat mbs
    stringFromS8( (pToken->buffer),"" );
    
    pToken->tPosStart    =  pCopyToken->tPosStart   ;
    pToken->tPosEnd      =  pCopyToken->tPosEnd     ;
    pToken->type         =  pCopyToken->type        ;
    pToken->type_num     =  pCopyToken->type_num    ;
    pToken->type_suffix  =  pCopyToken->type_suffix ;
    pToken->mbs          =  pCopyToken->mbs         ;
    pToken->f40s         =  pCopyToken->f40s        ;
    pToken->row          =  pCopyToken->row         ;
    pToken->col          =  pCopyToken->col         ;
    pToken->ptr          =  pCopyToken->ptr         ;
    
    return pToken;
}

node_t*    astMakeNodeNDX( parser_t *pPar , token_t *pToken , node_t* pNode , node_t *left , node_t *right , scope_t* pScope ) 
{
    if ( ( pToken->type == eToken_add ) || ( pToken->type == eToken_sub ) )
    {
        char*       tMBS    = NULL ;
        eToken_t    tType   = pToken->type ;
        
        switch ( tType ) 
        {
            case eToken_add:
                tMBS = gcStrDup("+"); //tType     = eToken_add ;
            break;
            case eToken_sub:
                tMBS = gcStrDup("-"); //tType     = eToken_sub ;
            break;
            case eToken_assign:
                tMBS = gcStrDup(":="); //tType    = eToken_sub ;
            break;
            default:
                $parserErrorExtra( parsing,opNotAllowed,pNode->pData->mbs ) ;
            break;
        }

        if ( ( left->baseKind == eTermKind_ptr )   )
        {
            uint8_t size ;

            pSymbol_t* pSymBase = stFind_pSymbolDef(pScope,left->pData->buffer.data ) ;
          
  
                if ( pSymBase != NULL )
                {
                    size = pSymBase->size;
                }
                else
                    size =stGetSizeFromType(left->baseType); // pointer size base

            // .......................................................................... make token integer

            token_t* newToken           = astNewTokenCopyFrom(pPar,pToken); // R C ...

            newToken->type              = eToken_integer;
            newToken->type_num          = eTermType_u8;
            newToken->type_suffix       = gcStrDup("u8");
            newToken->mbs               = gcStrDup("sizeof");
            newToken->buffer.data       = gcStrDup("sizeof");
            newToken->u64               = size;

            // .......................................................................... make node const intgeger

            node_t* newMulRight     = astMakeNodeTerm(pPar , newToken);
            newMulRight->baseKind   = eTermKind_const   ;
            newMulRight->baseType   = eTermType_u8 ;

            // .......................................................................... *

            pToken->type = eToken_mul ;
            pToken->mbs  = gcStrDup("*");
            pToken->buffer.data  = gcStrDup("*");

            astDebugNode(pPar,pToken,"arithmetic pointer *",astDisplayBufferOn);

            // .......................................................................... +
            // eToken_mul ,    //    *  303
            // eToken_add ,    //    +  307

            pNode->pnBinOp->pRight = astMakeNodeBinOp( pPar, pToken , right , newMulRight , pScope ) ;

            pNode->pData                 = astNewTokenCopyFrom(pPar,pToken)  ; // R C
            pNode->pData->type           = tType ;
            pNode->pData->mbs            = tMBS;
            pNode->pData->buffer.data    = tMBS;

            astDebugNode(pPar,pNode->pData,"arithmetic pointer +",astDisplayBufferOn);
        }
    }
    return pNode ;
}

//...................................................................... astMakeNodeParamFunction

node_t*        astMakeNodeParamFunction( parser_t *pPar , token_t *pToken )
{
    node_t*    pNode = astMakeNodeDefault(pPar,pToken);    

    nodeParamFunction_t*    pNodeParamFunction = (nodeParamFunction_t*) gcMalloc( sizeof ( nodeParamFunction_t )  );
    
    pNode->nType                     = nTypeParamFunction  ;
    pNode->pnParamFunction           = pNodeParamFunction  ;
    pNode->pnParamFunction->fName    = pPar->p0->mbs    ;
    pNode->pnParamFunction->retType  = NULL  ;
    pNode->pnParamFunction->fBlock   = NULL  ; 
    
    pNode->baseKind = eTermKind_fn    ;
    pNode->baseType = eTermType_u8    ; // GET RET FROM SYMBOL TABLE ( TODO fatta in scanner.c ... ?) 
    
    astDebugNode(pPar,pToken,"astMakeNodeParamFunction",astDisplayBufferOn);

    return pNode ;
}


//...................................................................... astMakeNodeDot

node_t*        astMakeNodeDot( parser_t *pPar  , token_t *pToken , node_t *left , node_t *right  , scope_t* pScope , char* member ) 
{
    (void)pScope;
    (void)right;
    
    node_t*    pNode = astMakeNodeDefault(pPar,pToken );    

    // node Dot
    nodeDot_t*    pNodeDot = (nodeDot_t*) gcMalloc( sizeof ( nodeDot_t ) ) ;
    
    pNode->nType           = nTypeDot ;
    pNode->pnDot           = pNodeDot ;
    pNode->pnDot->pLeft    = left  ;
    pNode->pnDot->member   = gcStrDup(member)  ;

    astDebugNode(pPar,pToken,"astMakeNodeDot",astDisplayBufferOn);

    return pNode ;
}

