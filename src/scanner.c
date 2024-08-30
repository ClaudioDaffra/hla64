
#include "../lib/cxx.h"
#include "Cmos.h"

// ******************
//     Scanner
// ******************

// analizzatore semantico

/*
    expr.c        :
    
        (binary,unary node)
            if ( left     == NULL ) { $parserErrorExtra( parsing,expectedExpression    ,"* /: LHS" ) ; break ; }
            if ( right     == NULL ) { $parserErrorExtra( parsing,expected                ,"* /: RHS" ) ; break ; }
        
    ast.c        :    
    
        astMakeNodeBinOp
            // ## canonicalize 'ptr' se ( num + ptr )     ->         segue ( ptr + num )
            // ## poniter arithmentic : pa + 1(rhs)        ->        pa + 1 * sizeof(pa)
            
    scanner.c    :
    
            (binary,unary node)
            
            :=
                #1)
                    if (  pNode->pData->type == eToken_assign ) 
                            stackAddrValue[kStackAddrValue] = emitAddr  ; 
                    else 
                            stackAddrValue[kStackAddrValue] = emitValue ;
                #2)
                    pNode->baseType = stPromote( pNode->pnBinOp->pLeft->baseType , pNode->pnBinOp->pRight->baseType ) ;
                    pNode->baseKind = eTermKind_const ;
                    
                #3) CAST IMPLICITO ( nodi binari ) 
                    
                    se il node binario è una assegnazione il casto viene fatto diversamente
                
                    if (  pNode->pData->type != eToken_assign ) // ## per i nodi binari si promuove per mantenere il tipo
                    {
                        pNode->baseType = stPromote( pNode->pnBinOp->pLeft->baseType , pNode->pnBinOp->pRight->baseType ) ;
                        pNode->baseKind = eTermKind_const ;
                    }
                    else // ## per il nodo assign si promuove per promuovere il tipo a quello della variabile (LHS)
                    {
                        pNode->baseType = pNode->pnBinOp->pLeft->baseType ;
                        pNode->baseKind = eTermKind_const ;
                    }
                    
                #4) CAST IMPLICITO ( nodi assegnazione ) 
                
                    // se il tipo del nodo Left/Right è diverso da quello base emetti CAST
                    // base ( type ) := LHS ( type ) + RHS ( TYPE )
                    if  ( pNode->pnBinOp->pLeft->baseType  != pNode->baseType ) 
                    {
                        // copia i riferimenti al nodo attuale RC ...
                        token_t* newToken         = astNewTokenCopyFrom(pPar,pNode->pData); 
                        newToken->type = eToken_type ;
                        newToken->buffer.data = newToken->mbs  = stGetCastNameFromType(pNode->baseType);
                        
                        pNode->pnBinOp->pLeft = astMakeNodePrefix ( pPar , newToken , pNode->pnBinOp->pLeft )  ;
                        
                        $ASTdebug("%s","Cast Injection LHS :: ");
                        ast( pPar , pNode->pnBinOp->pLeft , pScope ) ;
                    }
                    ... RHS
                    
                    
    scanner.c    :
                    (codegen.c) cgEmitSymbolLocal, fa riferimento al nodo : node_function_t
                    
                        - vengono emesse i simboli in assembly per lo stack locale 
                        - vengono emesse i simboli in assembly per .weak .endweak
                    
                
    expr.c        : 
                    parsePrefix
                    
                        - aggiunto il controllo sui cast per i tipi ( kludge ) 
                    
                    
    scanner.c    :    function :

                        - base - ( type / kind ) function
                        
                            pType_t* pRetType = stFind_pTypeDef( pScope,pNode->pnFunction->retType ) ;
                            assert(pRetType!=NULL);
                            pNode->baseType = pRetType->type ;
                            pNode->baseKind = pRetType->kind ;
            
    expr.c        :
                    viene da la possibilita di fare poke 53281,0 in altre parole 53281 := 0
                    normalmente è un errore ma nel c64 si può fare
                    per capire se mettere un byte o una word si fa riferimento al tipo RHS
                    
    cmos.h        :
                    nodeTerm_t
                    
                    addizionato il riferimento al simbolo :     pSymbol_t*         pSymbol    ;
                    cosicchè a livello di codegen non dobbiamo più usare findSymbol
                    
    scanner.c    :            
                    inverti di segno  :
                    
                    case eToken_subu:
                         if (pNode->baseType==eTermType_u8 )    pNode->baseType = eTermType_s8 ;                    
                         if (pNode->baseType==eTermType_s8 )    pNode->baseType = eTermType_u8 ;
                         if (pNode->baseType==eTermType_u16)    pNode->baseType = eTermType_s16;                    
                         if (pNode->baseType==eTermType_s16)    pNode->baseType = eTermType_u16;
                         
                         
    scanner.c    :    nTypeBinOp

                         // calcola l'arithmetica dei puntatori nei nodi binary
                         // caso speciale per il cast dei tipi a puntatori : $type * ( )
                         
                         pNode = astMakeNodeNDX(  pPar , pNode->pData ,  pNode , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight );
                    
*/

int astInternalError( char*s , int t)
{
    fprintf ( stdout , "\n!! internal error : ast ," ) ;
    fprintf ( stdout , " %s ",s ) ;
    fprintf ( stdout , " %d ",t ) ;
    fprintf ( stdout , " -> not recognize\n" ) ;
    exit(-1);
}

#define $ASTdefault(S,T)\
    default:\
    fprintf ( stdout , "\n?? internal error : ast ," ) ;\
    fprintf ( stdout , " %s ",S ) ;\
    fprintf ( stdout , " %d ",T ) ;\
    fprintf ( stdout , " -> not recognize\n" ) ;\
    exit(-1);\
    break;

void astTab( parser_t *pPar,int level)
{
    if (pPar->pLex->pStream->pCompiler->fDebug) 
    {
        fprintf ( pPar->pFileDebugParser->ptr , "\n"  );
        for (int i=0;i<level;i++) fprintf ( pPar->pFileDebugParser->ptr , "  "  );
    }
}

#define $ASTdebug(FORMAT,...)\
do{\
    if (pPar->pLex->pStream->pCompiler->fDebug)\
    {   astTab(pPar,level);\
        fprintf(pPar->pFileDebugParser->ptr,FORMAT,__VA_ARGS__);\
    }\
}while(0);


#define $ASTdebugNode(FORMAT,...)\
do{\
    if (pNode)\
        if (pPar->pLex->pStream->pCompiler->fDebug)\
        {   astTab(pPar,level);\
            fprintf(pPar->pFileDebugParser->ptr,"RC[%3d/%3d] B[%3d]K[%3d] "\
                    ,pNode->pData->row,pNode->pData->col,pNode->baseType,pNode->baseKind );\
            fprintf(pPar->pFileDebugParser->ptr,FORMAT,__VA_ARGS__);\
        }\
}while(0);

#define $ASTnl    fprintf(pPar->pFileDebugParser->ptr,"\n");

// ****
// AST
// ****

void astDebugScope( parser_t *pPar ,node_t* pNode , scope_t* pScope , int level )
{
    if ( pScope )
    {
        $ASTdebugNode("%s 0x%lx","parent : ",(size_t)pScope);
        $ASTdebugNode("%s 0x%lx","prev   : ",(size_t)pScope->prev);
            $ASTdebugNode("%s","## Type");        
            if ( pScope->pSymbolDef->size )
            {
                //    char*          id      ;    //    int        real           point
                //    eTermKind_t    kind    ;    //    var        var            record
                //    eTermType_t    type    ;    //    u16        f40            record
                //    uint8_t        size    ;    //    $(u16)    $(f40)        $(record)
                for(size_t i=0;i<pScope->pTypeDef->size;i++)
                {
                    pType_t* ptrType=pScope->pTypeDef->data[i] ;
                    $ASTdebugNode("  ID[%10s] K[%3d] T[[%3d] $[%3d]",ptrType->id,ptrType->kind,ptrType->type,ptrType->size);
                }
            }    
            $ASTdebugNode("%s","## Symbol");
            if ( pScope->pSymbolDef->size )
            {
                //    char*             id      ; // ID name
                //    eTermKind_t       kind    ; // genere     :       null,var,ptr,const,arr,fn,addr
                //    eTermType_t       type    ; // tipo       :       null,byte,char,int,word,real,(string=*byte,byte[]),record
                //    eScope_t          scope   ; // scope      :       null,local,param,global  
                //    int               offset  ; // stack      :       0    ,  offset
                //    uint8_t           size    ; // ID         :       0    ,  size
                for(size_t i=0;i<pScope->pSymbolDef->size;i++)
                {
                    pSymbol_t* ptrSymbol=pScope->pSymbolDef->data[i] ;
                    $ASTdebugNode("  ID[%10s] K[%3d] T[%3d] $[%3d] s[%3d] o[%3d]"
                        ,ptrSymbol->id
                        ,ptrSymbol->kind
                        ,ptrSymbol->type
                        ,ptrSymbol->size
                        ,ptrSymbol->scope
                        ,ptrSymbol->offset
                    );
                        
                    // DEBUG SCOPE E STRUCT
                    
                    if ( ptrSymbol->type == eRecordType ) 
                    {

                        pType_t* ptrType = stFind_pTypeDef ( pScope , ptrSymbol->pRecord->id ) ;
 
                        if ( ptrType != NULL )
                        {
                            // TODO stampa come code GEN
                        }
                        else
                        {
                            // printf(" NO FIELD OF %s ::\n",ptrSymbol->pRecord->id);
                        }

                    } 
                    
                }
            }
    }
    else
    {
        $ASTdebugNode("%s 0x%lx","parent : ",(size_t)0);
        $ASTdebugNode("%s 0x%lx","prev   : ",(size_t)0);
    }
}

//
// Le funzioni astMakeSymbol-Global/Local, generano i riferimenti agli indirizzi dei simboli
// vengono generati i riferimenti assembly già, nella fase semantica
//
// ex        Global
//        
//            greal3               .byte 0,0,0,0,0
//            gword2               .word 0
//            gbyte1               .byte 0
//
// ex        Local
//     
//             .struct
//                beta                 .sint 0 
//                alpha                .sint 0 
//            .endstruct
//
//            stack_alloc B_local_stack
//
//            fnStackSize .var fnStackSize + size(B_local_stack)
//            .cerror(fnStackSize>stack.size)
//            
//            .weak
//                beta                 = B_local_stack.beta
//                alpha                = B_local_stack.alpha
//            .endweak
//

static node_t* pNodeFunction=NULL ;
            
void astMakeSymbolGlobal(  node_t* pNode , scope_t* pScope  )
{
    char string[BUFFER_STRING_256];
    
    if ( pScope ) // ptrSymbol->scope=1
    {
        if ( pScope->pSymbolDef->size )
        {
            for(size_t i=0;i<pScope->pSymbolDef->size;i++)
            {
                pSymbol_t* ptrSymbol=pScope->pSymbolDef->data[i] ;

                if ( ptrSymbol->scope == eScope_global ) 
                {
                    if ( ptrSymbol->kind != eTermKind_fn ) 
                    {
                        snprintf ( string , BUFFER_STRING_256 , "        %-12s %-5s ?"
                            ,ptrSymbol->id,stGet64TassNameFromType(ptrSymbol->type) );
                        vectorPushBack( (*pNode->pnProgram->pvStringGlobal) , gcStrDup(string) ) ;
                    }
                }
            }
        }
    }
}

void astMakeSymbolLocal(   scope_t* pScope  )
{
    if  (pNodeFunction!=NULL ) 
    {
        char string[BUFFER_STRING_256];
        
        if ( pScope ) // ptrSymbol->scope=2,3
        {
            if ( pScope->pSymbolDef->size )
            {
                for(size_t i=0;i<pScope->pSymbolDef->size;i++)
                {
                    pSymbol_t* ptrSymbol=pScope->pSymbolDef->data[i] ;

                    if ( ( ptrSymbol->scope == eScope_param ) || ( ptrSymbol->scope == eScope_local)  )
                    {
                        if ( ptrSymbol->kind != eTermKind_fn ) // Todo REAL real .byte 0,0,0,0,0
                        {
                            // descrizione stack struct
                                if ( ( ptrSymbol->type == eTermType_r32 ) 
                                ||   ( ptrSymbol->type == eTermType_r40 ) 
                                ||   ( ptrSymbol->type == eTermType_r64 ) 
                                ||   ( ptrSymbol->type == eTermType_r80 ) )
                                    snprintf ( string , BUFFER_STRING_256 , "        %-12s .byte ?,?,?,?,?"
                                        ,ptrSymbol->id );
                                else
                                    snprintf ( string , BUFFER_STRING_256 , "        %-12s %-5s ?"
                                        ,ptrSymbol->id,stGet64TassNameFromType(ptrSymbol->type) );
                                
                                vectorPushBack( (*pNodeFunction->pnFunction->pvStringFunction) , gcStrDup(string) ) ;
                            // descrizione stack weak endweak :  sym = locaStack.sym
                                snprintf ( string , BUFFER_STRING_256 , "        %-12s =  localStack.%-12s",ptrSymbol->id,ptrSymbol->id );
                                vectorPushBack( (*pNodeFunction->pnFunction->pvWeakFunction) , gcStrDup(string) ) ;
                        }
                    }
                }
            }
        }
    }
}

// .............................................................................. makeNewTokenBinOp

token_t* makeNewTokenBinOp( parser_t* pPar, node_t* pNode )
{
    token_t* newToken       = astNewTokenCopyFrom(pPar,pNode->pData); 
    newToken->type          = eToken_type ;
    newToken->type_num      = pNode->pnBinOp->pRight->pData->type_num ;
    newToken->buffer.data   = newToken->mbs  = stGetCastNameFromType(pNode->baseType);
    
    return newToken ;
}

// .............................................................................. ast

#define emitValue   0
#define emitAddr    1
static int    stackAddrValue[SIZEOF_STACK];
static int    kStackAddrValue=0;

//

node_t*        ast( parser_t *pPar ,node_t* pNode , scope_t* pScope )
{
    static int level=0;

    if ( pNode==NULL ) 
    {
        $ASTdebugNode("%-20s","{pNode=null}" );
        return NULL ;
    }
    
    ++level;
    
    switch( pNode->nType )
    {
        // ________________________________________________________________ nTypeTerm
        case    nTypeTerm :
        {
            // ### VALUE
            
            if  ( stackAddrValue[kStackAddrValue] == emitValue ) 
            {
                switch( pNode->pData->type ) 
                {
            // ................................................................ nTypeTerm type
                    case eToken_integer:
                        $ASTdebugNode("%-20s t[%3d]tn[%3d] [%5d]","eToken_integer"
                        ,pNode->pData->type,pNode->pData->type_num,(int)pNode->pData->u64 );
                        pNode->baseType = pNode->pData->type_num ;
                    break;
                    case eToken_real:
                        $ASTdebugNode("%-20s t[%3d]tn[%3d] [%5d] {%s}","eToken_real"
                        ,pNode->pData->type,pNode->pData->type_num,(int)pNode->pData->r80,pNode->pData->f40s );
                        pNode->baseType = pNode->pData->type_num ;
                    break;
                    case eToken_mbs:
                        $ASTdebugNode("%-20s t[%3d]        {%5s}","eToken_mbs"
                        ,pNode->pData->type,pNode->pData->mbs );
                        pNode->baseType = eTermType_u16;
                    break;
                    case eToken_id:
                    {
                        pSymbol_t* pFindSymbol = stFind_pSymbolDef    ( pScope , pNode->pData->mbs );

                        if ( pFindSymbol != NULL )
                        {
                            $ASTdebugNode("%-20s t[%3d]        {%5s}","eToken_id",pNode->pData->type,pNode->pData->mbs );

                                 if ( pNode->baseKind  == eTermKind_arr ) pNode->baseType = eTermType_u16 ;
                            else if ( pNode->baseKind  == eTermKind_fn  ) pNode->baseType = eTermType_u16 ;
                            else if ( pNode->baseKind  == eTermKind_ptr ) 
                                 { 
                                   pNode->baseType = eTermType_u16 ;
                                 }
                            else 
                            {
                                pNode->baseType = pNode->pnTerm->pSymbol->type ;
                                pNode->baseSize = pFindSymbol->size ;
                            }
                        }
                        else
                        {
                            // si tratta du una struttura gestita in nTypeDot
                        }
                    }
                    break;
                    $ASTdefault("scanner.c -> switch( pNode->nType ) -> case    nTypeTerm -> switch( pNode->pData->type )"
                                ,pNode->pnTerm->pTerm->type);
                }
                pNode->baseKind = eTermKind_const ;
            }
            
            // ### ADDRESS
            
            if  ( stackAddrValue[kStackAddrValue] == emitAddr ) 
            {
                switch( pNode->pData->type ) 
                {
            // ................................................................ nTypeTerm type
                    case eToken_integer:
                        if ( ( pNode->pData->type_num >= eTermType_s8  )  
                        &&      ( pNode->pData->type_num <= eTermType_s16 ) )
                          $parserErrorExtraLast( scanning,LValueRequired,pNode->pData->buffer.data ) ;
                        
                        $ASTdebugNode("%-20s @t[%3d]        {%5s}","eToken_integer",pNode->pData->type,pNode->pData->buffer.data );
                        pNode->baseType = pNode->pData->type_num;
                    break;
                    case eToken_real:
                        $ASTdebugNode("%-20s @t[%3d]        {%5s}","eToken_real",pNode->pData->type,pNode->pData->buffer.data );
                        pNode->baseType = pNode->pData->type_num;
                    break;
                    case eToken_mbs:
                         $ASTdebugNode("%-20s @t[%3d]        {%5s}","eToken_mbs",pNode->pData->type,pNode->pData->buffer.data );
                         pNode->baseType = eTermType_u16;
                    break;
                    case eToken_id:
                        $ASTdebugNode("%-20s @t[%3d]        {%5s}","eToken_id",pNode->pData->type,pNode->pData->buffer.data );
                        
                        if (pNode->pnTerm->pSymbol ) // else è membro di struttura
                            pNode->baseType = pNode->pnTerm->pSymbol->type;
                        
                         if ( pNode->baseKind  == eTermKind_arr  ) pNode->baseType = eTermType_u16 ;
                         if ( pNode->baseKind  == eTermKind_addr ) pNode->baseType = eTermType_u16 ;
                         
                    break;
                    $ASTdefault("switch( pNode->pnTerm->pTerm->type )",pNode->pnTerm->pTerm->type);
                }
                // diventa un'indirizzo,mantiene le dimensioni (u8 s16 ... )
                pNode->baseKind = eTermKind_addr ;
            }

            assert (( stackAddrValue[kStackAddrValue-1]==0 ) || ( stackAddrValue[kStackAddrValue-1]==1)) ;
        }            
        break ;
        
        // ________________________________________________________________ nTypeDot
        case    nTypeDot:
        {
            ast( pPar , pNode->pnDot->pLeft  , pScope ) ;
            
            char *rightID = pNode->pnDot->member ;
            
            char *leftID  = pNode->pnDot->pLeft->pData->buffer.data ;

            // #0 ex point p2 ; p2.x   ;
            
            // #1 ricerca il simbolo  : p2
            
            pSymbol_t*  pSymbol = stFind_pSymbolDef    ( pScope , leftID ) ;
            if ( pSymbol == NULL ) 
            {// puts ("nTypeDot : undeclared symbol"); exit(-1); 
                $parserErrorExtraLast( coding,undeclared,leftID ) ;
            } 

            // #2 ottieni il record del simbolo : point

            // #3 ottieni l'offsetType
            
            eTermType_t offsetType = eTermType_null ;
            uint8_t     offsetPos  = -1 ;
            for ( int i=0; i< pSymbol->pRecord->kRecordField; i++)
            {
                if ( strcmp(pSymbol->pRecord->aRecordField_name[i], pNode->pnDot->member) == 0 )
                {
                    offsetType = pSymbol->pRecord->aRecordField_type[i] ;
                    offsetPos  = pSymbol->pRecord->aRecordField_offset[i] ;
                }
            }
            if ( offsetType==eTermType_null ) 
            { //puts ("nTypeDot : undeclared member"); exit(-1);
                $parserErrorExtraLast( coding,undeclared,pNode->pnDot->member ) ;        
            } 

            // #4 identifica il nodo con il tipo corretto
            
            pNode->baseType = offsetType        ;
            pNode->baseKind = eTermKind_const   ;

            // #5 debug node correttamente

            $ASTdebugNode("%-20s t[%3d]        {%5s} offset {%02d}","member_id",pNode->pData->type,rightID,offsetPos );
            
        }
        break ;
        
        // ________________________________________________________________ nTypeBinOp
        case    nTypeBinOp:
        {
            assert( ++kStackAddrValue < SIZEOF_STACK ) ;
            
            if (  pNode->pData->type == eToken_assign ) 
                    stackAddrValue[kStackAddrValue] = emitAddr  ; 
            else 
                    stackAddrValue[kStackAddrValue] = emitValue ;

            $ASTdebug("%s","- LHS");
            
            ast( pPar , pNode->pnBinOp->pLeft , pScope ) ;

            if ( stackAddrValue[kStackAddrValue] == emitAddr )
            {
                pNode->baseType = pNode->pnBinOp->pLeft->baseType ;
            }

            assert(--kStackAddrValue >=   0 ) ;    
            
            $ASTdebug("%s","- RHS");
            
            ast( pPar , pNode->pnBinOp->pRight , pScope ) ;

/*
            // ### pointer arithmetic binOp, special case after custom cast pointer ex : $int * ( x ) ...

            if ( pNode->pnBinOp->pLeft->baseKind == eTermKind_ptr )
                //if ( ( pNode->pnBinOp->pLeft->baseType != eTermType_u8 ) && ( pNode->pnBinOp->pLeft->baseType != eTermType_s8 ) )
                {
    //pNode = astMakeNodeNDX(  pPar , pNode->pData ,  pNode , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight );
                }
*/
            // ### ... occorre prima ovviamente mettere baseType,baseKind 

            if (  pNode->pData->type != eToken_assign ) // ## per i nodi binari si promuove per mantenere il tipo
            {
                pNode->baseType = stPromote( pNode->pnBinOp->pLeft->baseType , pNode->pnBinOp->pRight->baseType ) ;
                pNode->baseKind = eTermKind_const ;
                
                if ( pNode->pData->ptr == 1) {  pNode->baseKind = eTermKind_ptr ; }
            }
            else // per il nodo assign si promuove al tipo della variabile in (LHS)
            {
                pNode->baseType = pNode->pnBinOp->pLeft->baseType ;
                pNode->baseKind = eTermKind_const ;
/*                
                if ( pNode->pData->ptr == 1) pNode->baseKind = eTermKind_ptr ;


                if  ( pNode->pnBinOp->pRight->baseType != pNode->pnBinOp->pLeft->baseType ) 
                {
                    pNode->pnBinOp->pRight = astMakeNodePrefix ( pPar , makeNewTokenBinOp(pPar,pNode) , pNode->pnBinOp->pRight )  ;
                    
                    $ASTdebug("%s","Cast Injection RHS :: ");
                    ast( pPar , pNode->pnBinOp->pRight , pScope ) ; // Debug all child node
                }
*/
            }

            // CAST IMPLICITO
    
            // Per i nodi binari si promuove per mantenere il tipo, si mantiene invariato per il nodo assegnazione :=
            if (  pNode->pData->type != eToken_assign ) 
            {
                // Se il tipo del nodo Left/Right è diverso da quello base emetti CAST
                // base ( type ) := LHS ( type ) + RHS ( TYPE )
                if  ( pNode->pnBinOp->pLeft->baseType  != pNode->baseType ) 
                {
                    pNode->pnBinOp->pLeft = astMakeNodePrefix ( pPar , makeNewTokenBinOp(pPar,pNode) , pNode->pnBinOp->pLeft )  ;
                    
                    $ASTdebug("%s","Cast Injection LHS :: ");
                    ast( pPar , pNode->pnBinOp->pLeft , pScope ) ; // Debug all child node
                    // Per evitare debug ripetizione albero nodi figli si legge male
                    // $ASTdebugNode("%-20s t[%3d] [%s]","nTypePrefix",pNode->pnBinOp->pLeft->pData->type,pNode->pnBinOp->pLeft->pData->mbs );
                }

                if  ( pNode->pnBinOp->pRight->baseType != pNode->baseType ) 
                {
                    pNode->pnBinOp->pRight = astMakeNodePrefix ( pPar , makeNewTokenBinOp(pPar,pNode) , pNode->pnBinOp->pRight )  ;
                    
                    $ASTdebug("%s","Cast Injection RHS :: ");
                    ast( pPar , pNode->pnBinOp->pRight , pScope ) ; // Debug all child node
                    // Per evitare debug ripetizione albero nodi figli si legge male 
                    // $ASTdebugNode("%-20s t[%3d] [%s]","nTypePrefix",pNode->pnBinOp->pRight->pData->type,pNode->pnBinOp->pRight->pData->mbs );
                }
            }
            $ASTdebugNode("%-20s t[%3d] [%s]","nTypeBinOp",pNode->pData->type,pNode->pData->buffer.data );
        }
        break ;
        
        // ________________________________________________________________ nTypePrefix
        case nTypePrefix :
        {
                assert( ++kStackAddrValue < SIZEOF_STACK ) ;
                
                // case eToken_addr:
                if ( ( pNode->pData->type == eToken_addr )  ) 
                        stackAddrValue[kStackAddrValue] = emitAddr  ; 
                else 
                        stackAddrValue[kStackAddrValue] = emitValue ;

                ast( pPar , pNode->pnPrefix->pLeft , pScope ) ;    

                pNode->baseType = pNode->pnPrefix->pLeft->baseType ; 
                pNode->baseKind = pNode->pnPrefix->pLeft->baseKind ;

                assert(--kStackAddrValue >=   0 ) ;
                
                switch ( pNode->pData->type )
                {
                    // .......................................................... eToken_addr
                    case eToken_addr:
                        pNode->baseType = eTermType_u16 ; 
                        pNode->baseKind = eTermKind_addr ;
                        // Emesso in top nTypePrefix
                    break;
                    // .......................................................... eToken_ptr
                    case eToken_ptr:
                        pNode->baseType = pNode->pnPrefix->pLeft->baseType ; 
                        pNode->baseKind = pNode->pnPrefix->pLeft->baseKind ;
                        // *
                    break;
                    // .......................................................... eToken_addu
                    case eToken_addu:
                        // +
                    break;
                    // .......................................................... eToken_subu
                    case eToken_subu:
                    {
                         pNode->baseKind = pNode->pnPrefix->pLeft->baseKind ;

                                     // ### OPTIMIZE
/*
                                        if ( pNode->nType == nTypePrefix ) // 2 livelli
                                            if ( pNode->pnPrefix->pLeft->nType == nTypePrefix )
                                                pNode->pnPrefix->pLeft = pNode->pnPrefix->pLeft->pnPrefix->pLeft ;
*/
/*
                                     int integer=0;
                                     if (pNode->pnPrefix->pLeft->baseType==eTermType_u8 )    integer=1;
                                     if (pNode->pnPrefix->pLeft->baseType==eTermType_s8 )    integer=1;
                                     if (pNode->pnPrefix->pLeft->baseType==eTermType_u16)    integer=1;
                                     if (pNode->pnPrefix->pLeft->baseType==eTermType_s16)    integer=1;
*/
                                    int integer = stIsInteger( pNode->pnPrefix->pLeft->baseType ) ; 
                         
                                    // se sono presenti 1 o + unary risale al nodo terminale
                                    if ( pPar->pLex->pStream->pCompiler->fOptimise==true )
                                        while( (pNode->nType == nTypePrefix) && ( pNode->pnPrefix->pLeft->nType == nTypePrefix ) )
                                            pNode->pnPrefix->pLeft = pNode->pnPrefix->pLeft->pnPrefix->pLeft ;
                                    
                                     if ( pPar->pLex->pStream->pCompiler->fOptimise==true ) // se prefix - e left const real
                                     {
                                         if ( (pNode->pnPrefix->pLeft->baseType==eTermType_r40) )
                                         {
                                            double real80     = -1.0 * pNode->pnPrefix->pLeft->pData->r80 ;
                                            char*  negf40s     = convertDoubleToMBF( real80 ) ;
                                            
                                            pNode->pnPrefix->pLeft->pData->r80    = real80  ;
                                            pNode->pnPrefix->pLeft->pData->f40s    = negf40s ;

                                            $ASTdebug("%s","Optimise Unary - Real ::");
                                            pNode->pData->type = eToken_addu ; // traforma questo in un nodo senza emissione di codice
                                         }
                                         if ( integer==1 )
                                         {
                                            signed long  integer64     = -1 * pNode->pnPrefix->pLeft->pData->u64 ;

                                            pNode->pnPrefix->pLeft->pData->s64 = integer64  ;
                                        
                                            $ASTdebug("%s","Optimise Unary - integer ::");
                                            pNode->pData->type = eToken_addu ; // traforma questo in un nodo senza emissione di codice
                                         }
                                     }
                    }
                    break;
                    // .......................................................... eToken_not
                    case eToken_not:
                        pNode->baseType = pNode->pnBinOp->pLeft->baseType ; 
                        pNode->baseKind = pNode->pnBinOp->pLeft->baseKind ;
                        //pNode->baseType = eTermType_u8; // convenzione compilatori ! 1.0 -> 0

                                 // ### OPTIMIZE
/*
                                    if ( pNode->nType == nTypePrefix ) // 2 livelli
                                        if ( pNode->pnPrefix->pLeft->nType == nTypePrefix )
                                            pNode->pnPrefix->pLeft = pNode->pnPrefix->pLeft->pnPrefix->pLeft ;
*/
                                    // se sono presenti 1 o + unary risale al nodo terminale
                                    if ( pPar->pLex->pStream->pCompiler->fOptimise==true )
                                        while( (pNode->nType == nTypePrefix) && ( pNode->pnPrefix->pLeft->nType == nTypePrefix ) )
                                            pNode->pnPrefix->pLeft = pNode->pnPrefix->pLeft->pnPrefix->pLeft ;

/*
                                     int integer=0;
                                     if (pNode->pnPrefix->pLeft->baseType==eTermType_u8 )    integer=1;
                                     if (pNode->pnPrefix->pLeft->baseType==eTermType_s8 )    integer=1;
                                     if (pNode->pnPrefix->pLeft->baseType==eTermType_u16)    integer=1;
                                     if (pNode->pnPrefix->pLeft->baseType==eTermType_s16)    integer=1;
*/
                                    int integer = stIsInteger( pNode->pnPrefix->pLeft->baseType ) ; 


                                     if ( pPar->pLex->pStream->pCompiler->fOptimise==true ) // se prefix - e left const real
                                     {
                                         if ( (pNode->pnPrefix->pLeft->baseType==eTermType_r40) )
                                         {
                                            double real80     = ! pNode->pnPrefix->pLeft->pData->r80 ;
                                            char*  negf40s    = convertDoubleToMBF( real80 ) ;
                                            
                                            pNode->pnPrefix->pLeft->pData->r80    = real80  ;
                                            pNode->pnPrefix->pLeft->pData->f40s   = negf40s ;

// add cast fac1 to u8
//pNode = astMakeNodePrefix ( pPar , makeNewTokenBinOp(pPar,pNode) , pNode->pnBinOp->pLeft )  ;

                                            $ASTdebug("%s","Optimise Unary ! Real ::");
                                            pNode->pData->type = eToken_addu ; // traforma questo in un nodo senza emissione di codice
                                         }
                                         if ( integer==1 )
                                         {
                                            signed long  integer64     = ! pNode->pnPrefix->pLeft->pData->u64 ;
                                            pNode->pnPrefix->pLeft->pData->s64 = integer64  ;
                                        
                                            $ASTdebug("%s","Optimise Unary ! integer ::");
                                            pNode->pData->type = eToken_addu ; // traforma questo in un nodo senza emissione di codice
                                         }
                                     }
                                 
                    break;
                    // .......................................................... eToken_type    ( CAST ESPLICITO )
                    case eToken_type:
                    {
                        pType_t* pType = stFind_pTypeDef( pScope,&pNode->pData->mbs[1] ) ;
                        assert(pType!=NULL);
                        
                        pNode->baseType = pType->type ;
                        pNode->baseKind = pNode->pnBinOp->pLeft->baseKind ;

                        if ( pNode->pData->ptr == 1 )   // come addr va conservato il tipo
                        {
                            char buffer[BUFFER_STRING_64];
                            snprintf( buffer , BUFFER_STRING_64 , "$%s *",&pNode->pData->mbs[1]);
                            pNode->baseKind = eTermKind_ptr ; 
                            // vedi binOp arithmetic ndx
                            //$scannerWarningExtra( scanning,ptrArithmetic,gcStrDup(buffer),pNode->pData->row,pNode->pData->col); 
                        }

                    }
                    break;
                    $ASTdefault("nTypePrefix :: switch ( pNode->type ",( pNode->pData->type ));
                }
                $ASTdebugNode("%-20s t[%3d] [%s]","nTypePrefix",pNode->pData->type,pNode->pData->mbs );
        }
        break ;

        // ________________________________________________________________ nTypeBlock [scope]
        case nTypeBlock :
        {
            $ASTdebug("%s","--------------------------------------")

            astDebugScope(pPar,pNode,pNode->pnBlock->bScope,level);

            $ASTdebug("%s","{");    
            ++level;
            
            // costruisci vettore symboli locali per tutti i blocchi ( 1 sola volta ) !
            astMakeSymbolLocal( pNode->pnBlock->bScope  ) ; 
                
            for(uint32_t i=0;i<pNode->pnBlock->pvNode->size;i++)
            {
                ast( pPar , pNode->pnBlock->pvNode->data[i]  , pNode->pnBlock->bScope ) ;
            }
            
            --level;
            $ASTdebug("%s","}");
        }
        break ;

        // ________________________________________________________________ nodeASM_t
        case    nTypeASM:
        {
            $ASTdebugNode("%-20s","asm");
            $ASTdebug("%s","{");
            $ASTdebug("%s",pNode->pData->mbs);
            $ASTdebug("%s","}");
        }
        break ;

        // ________________________________________________________________ nTypeProgram [scope]
        case    nTypeProgram:
        {
            $ASTdebugNode("%-20s","nTypeProgram");

            astMakeSymbolGlobal( pNode,pNode->pnProgram->body->pnBlock->bScope  ) ;

            ast( pPar , pNode->pnProgram->body , pNode->pnProgram->body->pnBlock->bScope ) ;
        }
        break ;

        // ________________________________________________________________ nTypeFunction [scope]
        case    nTypeFunction:
        {
            $ASTdebugNode("%-20s fn %5s () -> %s $[%03d]","nTypeFunction"
                ,pNode->pnFunction->fName,pNode->pnFunction->retType,pNode->pnFunction->stackSize );
            
            $ASTdebug("%s","--------------------------------------");            

            pNodeFunction = pNode ; // nodeo funzione  ; // VAR GLOBAL
            
            astMakeSymbolLocal( pNode->pnFunction->fScope ) ;

            astDebugScope(pPar,pNode,pNode->pnFunction->fScope,level);

            ast( pPar , pNode->pnFunction->fBlock , pNode->pnFunction->fScope ) ;
            
            //pNode->pnFunction->pvStringFunction = pNodeFunction->pnFunction->pvStringFunction ;

            pNodeFunction = NULL ;

            $ASTdebug("%s","");
            
            // base - ( type / kind ) function
            pType_t* pRetType = stFind_pTypeDef( pScope,pNode->pnFunction->retType ) ;
            assert(pRetType!=NULL);
            pNode->baseType = pRetType->type ;
            pNode->baseKind = pRetType->kind ;
            
        }
        break ;
        
        // ________________________________________________________________  nTypeParamFunction
        case nTypeParamFunction :

                $ASTdebugNode("%-20s %s ( ... )",pNode->pnParamFunction->fName,"nTypeParamFunction");

                $ASTdebug("%s","(");

                if ( pNode->pnParamFunction->fBlock!=NULL)
                {
                    for (unsigned int i=0;i<pNode->pnParamFunction->fBlock->pnBlock->pvNode->size;i++)
                    {
                        ast( pPar , pNode->pnParamFunction->fBlock->pnBlock->pvNode->data[i] , pScope ) ;
                        ast( pPar , NULL , pScope ) ;
                    }
                }

                $ASTdebug("%s",")");
                
                // ERR printf ( "retype[%s]\n",pNode->pnFunction->retType) ; exit(-1);
                
                //if ( pNode->pnParamFunction->retType == NULL )  { puts("ret tye null"); exit(-1);}
                //else
                //pNode->baseType = stGetTypeFromName(pNode->pnParamFunction->retType);
                
                
                //pNode->baseKind = pRetType->kind ;
            
                return pNode;
        break ;
        
        // ________________________________________________________________ END switch pNode
        $ASTdefault("switch( pNode->nType )",pNode->nType);
    }

    --level;
    return pNode ;    
}
