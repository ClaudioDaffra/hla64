
#include "../lib/cxx.h"
#include "Cmos.h"

// ******************
//     Parser
// ******************

static int functionStackOffset = 0 ;

parser_t* parserNew(lexer_t*    pLex)
{
    parser_t*    pPar = (parser_t*) new (parser_t)  ;

    pPar->pLex              = pLex ;
    pPar->pFileDebugParser  = NULL ;
    pPar->pFileDebugNode    = NULL ;
    
    pPar->pAST              = NULL ;
    
    if ( pPar->pLex->pStream->pCompiler->fDebug==true )
    {
        pPar->pFileDebugParser = streamOpen(pPar->pLex->pStream->pCompiler->fileInputName.data, "w+",sender_parser ) ;
        pPar->pFileDebugNode   = streamOpen(pPar->pLex->pStream->pCompiler->fileInputName.data, "w+",sender_node   ) ;
    }

    pPar->lexPosStart   =   0       ;
    pPar->lexPosEnd     =   0       ;
    
    pPar->p0            =   NULL    ;
    pPar->p1            =   NULL    ;
    pPar->ndx           =   0       ;
    pPar->pLastToken    =   NULL    ;

    return pPar;
}

//...................................................................... parserDelete

void parserDelete( parser_t *pPar )
{
    assert(pPar!=NULL);
    
    if ( pPar->pLex->pStream->pCompiler->fDebug==true )
    {
        if ( pPar->pFileDebugParser!=NULL )
            if ( pPar->pFileDebugParser->ptr!=NULL )    
                gcFileClose(pPar->pFileDebugParser->ptr);
            
        if ( pPar->pFileDebugNode!=NULL )
            if ( pPar->pFileDebugNode->ptr!=NULL )    
                gcFileClose(pPar->pFileDebugNode->ptr); 
    }

    delete(pPar);
}

//...................................................................... parserGetToken

token_t* parserGetToken( parser_t *pPar )
{
    pPar->pLastToken=pPar->p0;
    
    if ( pPar->ndx >= pPar->pLex->pvToken->size )
    {
        pPar->p0 = NULL ; 
        pPar->p1 = NULL ;
    }
    else
    {
        pPar->p0 = pPar->pLex->pvToken->data[ pPar->ndx ] ;
        
        if ( pPar->ndx+1 < pPar->pLex->pvToken->size ) 
            pPar->p1 = pPar->pLex->pvToken->data[ pPar->ndx+1 ] ;
        else
            pPar->p1 = NULL ;
    }            
        
    return pPar->p0 ;
}

//...................................................................... parserNextGetToken

token_t* parserNextGetToken( parser_t *pPar )
{
    if ( pPar->ndx+1 <= pPar->pLex->pvToken->size ) pPar->ndx++;
    
    return parserGetToken(pPar) ;
}

//...................................................................... parserMatchToken

int parserMatchToken( parser_t *pPar , eToken_t tokenType , const char *tok )
{
    if ( pPar != NULL )    
        if ( pPar->p0 != NULL )         
            if ( pPar->p0->type ==  tokenType )
            {
                parserNextGetToken( pPar ) ;
                return 1 ;
            }
            
    $parserErrorExtra( parsing,expected,tok )  ;
    return 0;
}

//...................................................................... parseASM

// TODO :   sostituzione termini HLA con 64tass
//      :   ex    :    int    ->    .sint

node_t*        parseASM( parser_t *pPar , node_t *pNode )
{
    if ( pPar != NULL )    
        if ( pPar->p0 != NULL )         
            if ( pPar->p0->type ==  eToken_kw_asm )
            {
                parserNextGetToken( pPar ) ;

                if ( pPar->p0->type !=  eToken_mbs ) $parserErrorExtra( parsing,expected ,"assembly code" )  ;
                    
                if ( pPar->p0->type ==  eToken_mbs )
                {
                    pNode = astMakeNodeASM(pPar,pPar->p0) ;
                    parserNextGetToken( pPar ) ;
                }
                return pNode;
            }
    
    return    NULL;
}

//...................................................................... parseProgram

node_t*        parseBlockBeginEnd( parser_t *pPar , scope_t* parentScope ) 
{
    if ( parserMatchToken( pPar , eToken_g0 , "{" ) == 0 ) return NULL ;
    
    node_t*    pBlock = astMakeNodeBlock(pPar,pPar->p0) ;

    pBlock->pnBlock->bScope          = stNewScope()     ;
    pBlock->pnBlock->bScope->prev    = parentScope      ;

    while(1)
    {
        node_t*    pNode1 = NULL ;    

        // .................................................................... parse decl
        // mantenre parse decl in testa in quanto non è previsto continue
        pNode1=parseDeclaration( pPar , NULL , eScope_local ,pBlock->pnBlock->bScope ) ;

        // ............................................................ parseTerm
        pNode1 = parseExpr(pPar,NULL,pBlock->pnBlock->bScope) ;    
        if ( pNode1 ) {         
            vectorPushBack((*pBlock->pnBlock->pvNode), pNode1) ;
            continue; 
        }
        // ............................................................ parseASM
        pNode1 = parseASM( pPar , NULL ) ;    

        if ( pNode1 ) { 
            vectorPushBack((*pBlock->pnBlock->pvNode), pNode1) ;
            continue; 
        }
        // ............................................................ parse{}
        if ( pPar->p0->type == eToken_g0 )
        {
            pNode1 = parseBlockBeginEnd( pPar , pBlock->pnBlock->bScope ) ;    
            if ( pNode1 ) {
                vectorPushBack((*pBlock->pnBlock->pvNode), pNode1) ;
                continue; 
            }
        }
        break;
    }

    parserMatchToken( pPar , eToken_g1 , "}" ) ;

    return pBlock ;
}

//...................................................................... parseFunction

node_t* parseFunction( parser_t *pPar , node_t* pNode , scope_t* parentScope , eScope_t prevScope )
{
    if ( pPar != NULL )    
        if ( pPar->p0 != NULL )         
        {
            // .................................................................... fn
            if( pPar->p0->type ==  eToken_kw_function ) // fn ()
            {
                functionStackOffset = 0 ; // dimensione dello stack azzerate
                
                node_t* pNodeFunction = astMakeNodeFunction(pPar,pPar->p0);

                pNodeFunction->pnFunction->fScope          = stNewScope()   ;
                pNodeFunction->pnFunction->fScope->prev    = parentScope    ;
             
            // .................................................................... enter scope
            
                parserNextGetToken( pPar );

            // .................................................................... id
            
                pNodeFunction->pnFunction->fName = pPar->p0->mbs ;
                
                if ( !parserMatchToken(pPar,eToken_id,"ID") ) return NULL ;

            // .................................................................... ()
            
                if ( !parserMatchToken(pPar,eToken_p0,"(") ) return NULL ;
                
                if ( !parserMatchToken(pPar,eToken_p1,")") ) return NULL ;

            // .................................................................... ->
            
                if ( !parserMatchToken(pPar,eToken_return,"->") ) return NULL ; // eToken_return , eToken_kw_return !
                
            // .................................................................... return type
        
                pNodeFunction->pnFunction->retType = pPar->p0->mbs ;
                
                if ( !stIsPrimitiveType(pPar->p0->type) )
                {
                    pNodeFunction->pnFunction->retType = NULL ;
                    $parserErrorExtra( parsing,expected,"type" )  ;
                    return NULL ;
                }
                parserNextGetToken( pPar );

            // .................................................................... decl

                parseDeclaration( pPar , pNode , eScope_param ,pNodeFunction->pnFunction->fScope ) ;

            // .................................................................... block

                if ( pPar->p0->type == eToken_g0 ) 
                    pNodeFunction->pnFunction->fBlock = parseBlockBeginEnd(pPar,pNodeFunction->pnFunction->fScope);
                else
                {
                    $parserErrorExtra( parsing,expected,"{" )  ;
                    return NULL ;
                }
                
                // add symbol function to parent
                
                pSymbol_t* newSymbol  = stNewSymbol
                ( pNodeFunction->pnFunction->fName , eTermKind_fn , stGetTypeFromName(pNodeFunction->pnFunction->retType) , prevScope ) ;

                stAddNewSymbol ( parentScope , newSymbol ) ;

                pNodeFunction->pnFunction->stackSize = functionStackOffset ; // somma degli offset
                
                if ( functionStackOffset >= pPar->pLex->pStream->pCompiler->stackMaxSize )
                {
                    $parserErrorExtra( parsing,overflow,"stack" )  ;
                }

                return pNodeFunction;
            }
        }
            
    return  pNode ;
}

//...................................................................... parseDeclaration

pType_t* parserCheckDeclType( parser_t *pPar ,  scope_t* parentScope ) 
{
    if ( pPar != NULL )    
        if ( pPar->p0 != NULL )
            if ( (pPar->p0->type == eToken_id) || (stIsPrimitiveType(pPar->p0->type)) ) // eToken_id, eToken_kw_...
            {        
                return    stFind_pTypeDef    ( parentScope,pPar->p0->mbs ) ;
            }
    return NULL ;
}

//...................................................................... parseDeclaration struct

int parseDeclarationStruct( parser_t *pPar , node_t* pNode , eScope_t scope , scope_t* pScope ) 
{
    (void)pNode ;
    (void)scope ;
    
    if  ( ( pPar->p0->type == eToken_id ) && ( pPar->p1->type == eToken_g0 ) )
    {
        token_t* pID = pPar->p0 ;
        // ........................................ point {
        parserMatchToken(pPar,eToken_id,"ID") ;
        parserMatchToken(pPar,eToken_g0,"{")  ;

        // ## definisce un nuovo tipo
        pType_t* pNewType = stNewType(pID->mbs,eTermKind_record,eRecordType,0,scope);
        
        pNewType->kRecordField = 0 ;
        
        // ............................................ {

        // !! Si accettano solo i tipi primitivi : byte char int word real
        
        for(;;)
        {
            // ........................................ byte x
            token_t* pStructIDtype  = NULL  ;
            token_t* pStructIDfield = NULL  ;   
                    
            if ( stIsPrimitiveType(pPar->p0->type) )
            {
                pStructIDtype = pPar->p0 ;

                // Qui dobbiamo trovare un tipo già esistente ( byte, char, word, int, real )
                pType_t* pTypeFound = NULL ; 
                
                pTypeFound = stFind_pTypeDef(pScope,pStructIDtype->mbs ) ;
                
                if ( pTypeFound != NULL )
                {
                    parserNextGetToken( pPar );

                    pStructIDfield = pPar->p0 ;

                    if ( parserMatchToken(pPar,eToken_id,"ID") ) 
                    {
                        // ## definisce un nuovo campo della struttura,ottengo tutte le informazioni del tipo in questione

                        pNewType->aRecordField_type[pNewType->kRecordField] = pTypeFound->type ;
                        
                        pNewType->aRecordField_name[pNewType->kRecordField] = gcStrDup(pStructIDfield->mbs) ;

                        // il size del nuovo tipo è la somma dei tipi
                        pNewType->size += pTypeFound->size ; 

                        pNewType->scope = scope ;

                        pNewType->offset = pNewType->size ;

                        pNewType->aRecordField_offset[pNewType->kRecordField] = pNewType->offset ;

                        ++pNewType->kRecordField;

                        assert ( pNewType->kRecordField < BUFFER_STRING_16 ) ;

                        continue ;
                    }
                }
                else
                {
                    printf ( "?? Error : unknown type name ‘%s’.",pStructIDtype->mbs);
                    exit(-1); // controllo ridondante
                }
            }
            else
            {
                if ( pPar->p0->type != eToken_g1) 
                {
                    printf ( "?? Error : not a primitive type ‘%s’.",pPar->p0->mbs);
                    exit(-1);
                }
            }
            break;
        }
        // ............................................ }
        parserMatchToken(pPar,eToken_g1,"}")  ;

        // lo mette nello scope di riferimento come per i simboli
        vectorPushBack(  (*pScope->pTypeDef), pNewType ) ;

        return 1 ;
    }
    return 0 ;
}

//...................................................................... parseDeclaration

node_t* parseDeclaration( parser_t *pPar , node_t* pNode , eScope_t scope , scope_t* pScope ) 
{
    //if ( pPar != NULL )    
    //if ( pPar->p0 != NULL ) // inizia TOP DOWN parser Decl        
    //{
        for(;;)
        {
            // ......................................................... check for def struct
            pType_t*    pScopeType = parserCheckDeclType( pPar , pScope )    ;

            // check for ID o primitive type (

            if ( parseDeclarationStruct( pPar,pNode,scope,pScope ) ) continue;

            // ......................................................... check for typeDef
            if ( pScopeType != NULL ) 
            {
                int size = stGetSizeFromType( pScopeType->type ) ;

                functionStackOffset += size ;
                            
                parserNextGetToken( pPar );
            // ......................................................... check for ptr    
                    int fPointer=0;
                    if ( pPar->p0->type ==  eToken_mul ) // pointer 1 level ...
                    {
                         fPointer=1;
                         parserNextGetToken( pPar ) ;
                    }
                    else fPointer=0;
                    
            // ......................................................... check for ID
                    token_t* tID=pPar->p0 ;        
                    if ( parserMatchToken(pPar,eToken_id,"ID") ) // exit next token
                    {

                        if ( stFind_pSymbolDef(pScope,tID->mbs ) != NULL )
                        {
                            $parserErrorExtraLast( parsing,duplicateDefinition,pPar->pLastToken->mbs ) ;
                        } 

            // ......................................................... check for []    

            // ......................................................... check for ()
                
            // ......................................................... check for :=

            // ......................................................... ### make new symbol

                        pSymbol_t* newSymbol  =     stNewSymbol( tID->mbs , pScopeType->kind , pScopeType->type , scope ) ;

                        newSymbol->pRecord = pScopeType ; // rect   rr1

                        newSymbol->size = pScopeType->size ;
                        
                        newSymbol->pRecord->scope = scope ;

                        if ( fPointer ) newSymbol->kind = eTermKind_ptr ;
                        
                        newSymbol->offset = functionStackOffset; // attuale offset
                                                    
                        stAddNewSymbol ( pScope , newSymbol ) ;
                    }
            }
            else return pNode ;
        }
    //}

    return  pNode ;
}

//...................................................................... parseProgram

node_t*        parseProgram( parser_t *pPar )
{
    // check end of program 'program'
    parserMatchToken( pPar , eToken_kw_program , "program" ) ;
    
    if ( kError ) return NULL ;
    
    node_t* program = astMakeProgram(pPar,pPar->p0) ;
    
    node_t*    pBlock  = astMakeNodeBlock(pPar,pPar->p0) ;

    pBlock->pnBlock->bScope             = stNewScope();
    
    pBlock->pnBlock->bScope->prev       = NULL ;

    pBlock->pnBlock->bScope->pTypeDef   =  stInitDefaultType();

    while(1)
    {
        node_t*    pNode1 = NULL ;    

        // ............................................................ parse decl
        // mantenere parse decl in testa in quanto non è previsto continue
        pNode1=parseDeclaration( pPar , NULL , eScope_global ,pBlock->pnBlock->bScope ) ;    
        
        // ............................................................ parseTerm
        pNode1 = parseExpr(pPar,NULL,pBlock->pnBlock->bScope) ;    
        if ( pNode1 ) { 
            vectorPushBack((*pBlock->pnBlock->pvNode), pNode1) ;
            continue; 
        }

        // ............................................................ parseFunction
        pNode1 = parseFunction(pPar,NULL,pBlock->pnBlock->bScope , eScope_global ) ;    
        if ( pNode1 ) { 
            vectorPushBack((*pBlock->pnBlock->pvNode), pNode1) ;
            continue; 
        }

        // ............................................................ parseASM
        pNode1 = parseASM( pPar , NULL ) ;    
        if ( pNode1 ) { 
            vectorPushBack((*pBlock->pnBlock->pvNode), pNode1) ;
            continue; 
        }

        // ............................................................ parse{}
        if ( pPar->p0->type == eToken_g0 )
        {
            pNode1 = parseBlockBeginEnd( pPar , pBlock->pnBlock->bScope ) ;    
            if ( pNode1 ) {
                vectorPushBack((*pBlock->pnBlock->pvNode), pNode1) ;
                continue; 
            }
        }
        break;
    }

    // check end of program 'end'
    parserMatchToken( pPar , eToken_kw_end , "end" ) ;

    program->pnProgram->body = pBlock ;

    return program ;
}

// ***********
//    parserRun    :    program
// ***********

node_t* parserRun( parser_t *pPar )
{
    node_t*    program =  NULL ;

    parserGetToken( pPar ) ;

    program = parseProgram(pPar);
    
    pPar->pAST = program ;
    
    if ( !kError )    ast(pPar,pPar->pAST,program->pnProgram->body->pnBlock->bScope);

    return program ;
}


