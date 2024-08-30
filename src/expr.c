
#include "../lib/cxx.h"
#include "Cmos.h"

// ******************
//     EXPR
// ******************

/*
--------------------------------------------------------------------------------------------
Precedence    Operator    Description                                          Associativity
--------------------------------------------------------------------------------------------
1           ++ --         Postfix/suffix increment and decrement               Left-to-right
            ()            Function call
            []            Array subscripting
            .             Structure and union member access
            ->            Structure and union member access through pointer
            (type)        {list}    Compound literal(C99)
--------------------------------------------------------------------------------------------        
2           ++ --          Prefix increment and decrement[note 1]               Right-to-left
            + -              Unary plus and minus                                                   v
            ! ~              Logical NOT and bitwise NOT
            (type)        Cast
            *             Indirection (dereference)                                                 v
            &              Address-of                                                               v
            sizeof        Size-of[note 2]
            _Alignof      Alignment requirement(C11)
--------------------------------------------------------------------------------------------
3           * / %         Multiplication, division, and remainder              Left-to-right        v
--------------------------------------------------------------------------------------------
4           + -           Addition and subtraction                                                  v
--------------------------------------------------------------------------------------------
5           << >>         Bitwise left shift and right shift
--------------------------------------------------------------------------------------------
6           < <=          For relational operators < and ≤ respectively                             v
            > >=          For relational operators > and ≥ respectively
--------------------------------------------------------------------------------------------
7           == !=         For relational = and ≠ respectively                                        v
--------------------------------------------------------------------------------------------
8           &              Bitwise AND
--------------------------------------------------------------------------------------------
9           ^              Bitwise XOR (exclusive or)
--------------------------------------------------------------------------------------------
10          |             Bitwise OR (inclusive or)
--------------------------------------------------------------------------------------------
11          &&              Logical AND
--------------------------------------------------------------------------------------------
12          ||              Logical OR
--------------------------------------------------------------------------------------------
13          ?:              Ternary conditional[note 3]                         Right-to-left
--------------------------------------------------------------------------------------------
14                                                                              Right-to-left
                           Simple assignment                                                        v
            += -=          Assignment by sum and difference
            *= /= %=       Assignment by product, quotient, and remainder
            <<= >>=        Assignment by bitwise left shift and right shift
            &= ^= |=       Assignment by bitwise AND, XOR, and OR
--------------------------------------------------------------------------------------------
15          ,                Comma                                               Left-to-right
--------------------------------------------------------------------------------------------

*/

/*
    in HLA le espressioni potrebbe risultare ambigue, occorre pertanto mettere la parentesi , non c'è il ';'
    
    ex  *1+2    :    viene vista come *1+2*3+1      ,    senza le parentesi
        *3+1
    
    ex  (*1+2)    :    viene vista come *1+2*       ,    con le parentesi
        (*3+1)                         *3+1
*/


//...................................................................... parse post fix ()

node_t*        parsePostFixParamFunction( parser_t *pPar , node_t *pNode , scope_t* pScope , pSymbol_t* pSymbol , char* idName )
{
    if (pPar->p0->type==eToken_p0)
    {
        node_t*    pBlock = astMakeNodeBlock(pPar,pPar->p0) ;
        parserNextGetToken( pPar ) ;

        for(;;)
        {
            node_t* pNode1=NULL ;

            pNode1=parseExpr(pPar,NULL , pScope) ;
            if ( pNode1 ) vectorPushBack((*pBlock->pnBlock->pvNode), pNode1) ;

            if (pNode1==NULL) break ;
            if ( pPar->p0->type==eToken_p1) break ;
        } 

        pNode = astMakeNodeParamFunction( pPar ,pPar->p0 ) ;
        pNode->pnParamFunction->fName  = idName ;
        pNode->pnParamFunction->fBlock = pBlock ;

        parserMatchToken    ( pPar , eToken_p1 , ")" ) ;
        
        //printf ( "size param [%d]\n",pNode->pnParamFunction->fBlock->pnBlock->pvNode->size) ;
        pNode->baseType         = pSymbol->type ;
        pNode->baseKind         = pSymbol->kind ;                        
        return pNode ;
    }

  return pNode;
}

//...................................................................... parse post : ()

node_t*        parsePostFix( parser_t *pPar , node_t *pNode , scope_t* pScope , pSymbol_t* pSymbol , char* idName )
{
    node_t* retNode=NULL ;
    
    // ()
    retNode = parsePostFixParamFunction( pPar , pNode ,  pScope , pSymbol , idName ) ;
    if ( retNode != NULL ) return retNode ;
    
    return retNode;
}

//...................................................................... parseTerm

node_t*        parseTerm( parser_t *pPar , node_t *pNode , scope_t* pScope )
{
    if ( pPar->p0 == NULL ) return pNode ;

    switch( pPar->p0->type )
    {
        case    eToken_p0: // ......................................................... ( ... )
        
                if ( pPar->p0->type == eToken_p0 )
                {
                    int fPushErrMsgParOpen=0;
                        
                    parserNextGetToken( pPar ) ;

                    if ( pPar->p0 == NULL  ) break ;

                    pNode = parseExpr( pPar , pNode , pScope ) ;

                    if ( pPar->p0 == NULL  )
                    {
                        fPushErrMsgParOpen=1;
                        $parserErrorExtra( parsing,expected," ')' indeed EOT" )  ;
                        return pNode ;
                    } 
                    if (  !fPushErrMsgParOpen && pPar->p0->type != eToken_p1 )
                    {
                        fPushErrMsgParOpen=1;
                        $parserErrorExtra( parsing,expected,")" )  ; 
                        return pNode ;
                    }
                    parserNextGetToken( pPar ) ;
                }
        break;
        
        case    eToken_integer:        // ............................................. eToken_integer
        
                pNode = astMakeNodeTerm( pPar , pPar->p0 ) ;

                pNode->pData->type        =    eToken_integer;
                pNode->pData->type_num    =    stGetReduceTypeFromTermType(pNode->pData); // -> token_t*
                pNode->pData->ptr        =   0 ;

                pNode->baseType = pNode->pData->type_num ;
                pNode->baseKind = eTermKind_const ;
                
                parserNextGetToken( pPar ) ;
        break;
        
        case    eToken_real:        // ............................................. eToken_real

                pNode = astMakeNodeTerm( pPar , pPar->p0 ) ;
                
                pNode->pData->type        =    eToken_real;
                pNode->pData->type_num    =    stGetReduceTypeFromTermType(pNode->pData); // -> token_t*
                pNode->pData->ptr        =   0 ;
                
                pNode->baseType = pNode->pData->type_num ;
                pNode->baseKind = eTermKind_const ;
                
                parserNextGetToken( pPar ) ;

        break;
        
        case    eToken_id:            // ............................................. eToken_id 
        {
                pSymbol_t* pSymbol = stFind_pSymbolDef    ( pScope , pPar->p0->mbs ) ;
                
                // l'ho messo prima per evitare di far comparire : expected expression before : (:= LHS).
                pNode = astMakeNodeTerm( pPar , pPar->p0 ) ; // pNode->pnTerm->pTerm    = pToken  ;
                pNode->pData->ptr        =   0 ;
                
                char *idName            = pPar->p0->mbs;
                pNode->pData->type      = eToken_id;    
                    
                if ( pSymbol == NULL )
                {
                    pNode->baseType         = eRecordType ;
                    pNode->baseKind         = eTermKind_record ;
                }
                else
                {
                    pNode->baseType         = pNode->pData->type_num ;
                    pNode->baseKind         = pSymbol->kind ;
                }
        
                parserNextGetToken( pPar ) ;
                
                // parser POST FIX
                
                node_t* retNode =  parsePostFix( pPar ,NULL , pScope , pSymbol ,idName ) ;
                if ( retNode != NULL ) return retNode ;
                
                // 

                if ( pSymbol )
                {
                    pNode->baseType         = pSymbol->type ;
                    pNode->baseKind         = pSymbol->kind ;
                    pNode->pnTerm->pSymbol  = pSymbol ;
                }
                else
                {
                    pNode->baseType         = eRecordType ;
                    pNode->baseKind         = eTermKind_record ;
                    pNode->pnTerm->pSymbol  = NULL ;
                }
                
                    // ------------------------------------------------------------
                    
        }
        break;
        
        case    eToken_mbs:            // ............................................. eToken_mbs
        
                pNode = astMakeNodeTerm( pPar , pPar->p0 ) ;
                
                pNode->pData->type        =    eToken_mbs;
                
                pNode->baseType = eTermType_u8 ;
                pNode->baseKind = eTermKind_addr ;
                
                parserNextGetToken( pPar ) ;
        break;
        
        default:
            // potrebbe esserci un nodo NULL, o no potrebbe esseri expressione;
            //printf("?? INTERNAL ERROR parseTerm() switch :: not implemented yet [%d].\n",pPar->p0->type);exit(-1);
        break;
    }

    return pNode ;
}

//...................................................................... parseDotOp [ . ]

node_t*        parseDotOp( parser_t *pPar , node_t *pNode  , scope_t* pScope )
{
    node_t *left=NULL;

    if ( pPar->p0==NULL )  return pNode; 

    left = parseTerm( pPar ,  pNode , pScope ) ;
    
    if (pPar->p0->type==eToken_pv) { parserNextGetToken( pPar ) ; return left ;}
    if ( pPar->p0==NULL )  return left ; 

    if ( (pPar->p0->type == eToken_dot )  )   // ...................  Left-to-right
    {
        if ( left == NULL ) { $parserErrorExtra( parsing,expectedExpression,". LHS" ) ; return NULL  ; }
        
        token_t* pTokenType = pPar->p0 ;

        parserNextGetToken( pPar ) ;

        if ( pPar->p0==NULL ) { $parserErrorExtra( parsing,expected,"eof . RHS" ) ; return NULL  ; }

        node_t* right =  parseTerm( pPar ,  NULL  , pScope ) ;

        if ( right == NULL ) { $parserErrorExtra( parsing,expected,". RHS" ) ; return NULL  ; }

        left = astMakeNodeDot ( pPar  , pTokenType , left   , right , pScope , right->pData->mbs ) ;
        
        if (pPar->p0->type==eToken_pv) { parserNextGetToken( pPar ) ; return left ;}        
        if ( pPar->p0 ==    NULL ) return NULL ;
    } ;

    return left ;
}

//...................................................................... parsePrefix [ + - & * ! ]

// is cast
 
int    parseIsCast( parser_t *pPar , scope_t* pScope )
{
    token_t* tID=pPar->p0 ;    

    pType_t* find=NULL;

    // se un simbolo inizia con $ andiamo a vedere se è un CAST ( semplificazione )
    find=stFind_pTypeDef(pScope,&tID->mbs[1]); // salta $

    if ( find  != NULL ) // può esserci int o no
    {
        //$parserErrorExtraLast( parsing,undeclared,pPar->pLastToken->mbs ) ;
        return 1 ;
    }
    if ( tID->mbs[0]=='$' ) 
    {
        $parserErrorExtra( parsing,invalidCastName,tID->mbs )
    }
    return 0 ;
}

// stack type def 

stackTypeDef( token_t * , stackPrefixToken ) ;

eToken_t parserIsCheckPrefix(parser_t* pPar , scope_t* pScope )
{
    if ( pPar->p0->type == eToken_and )  return  eToken_addr ;
    if ( pPar->p0->type == eToken_mul )  return  eToken_ptr  ;
    if ( pPar->p0->type == eToken_add )  return  eToken_addu ;
    if ( pPar->p0->type == eToken_sub )  return  eToken_subu ;
    if ( pPar->p0->type == eToken_not )  return  eToken_not  ;

    if ( pPar->p0->type == eToken_id )
    {
        if ( pPar->p0->buffer.data[0] == '$' )
            if ( parseIsCast( pPar , pScope ) )
            {
                return eToken_type ; // occorre vedere poi di che token si tratta nella ST
            }
    }
 
    return eToken_null;  ;
}

node_t*        parsePrefix( parser_t *pPar , node_t *pNode , scope_t* pScope )
{
    node_t* left = pNode ;

    if (pPar->p0->type==eToken_pv) { parserNextGetToken( pPar ) ; return pNode ;}

    stack_stackPrefixToken_t* pstackPrefixToken = (stack_stackPrefixToken_t*) gcMalloc( sizeof(stack_stackPrefixToken_t) ) ;    
    
    stackAlloc( (*pstackPrefixToken), 8 ) ;

    if ( pPar->p0==NULL )  return pNode; 

    eToken_t tPrefix=eToken_null;
    
    int kprefix_subu     = 0 ;
    int kprefix_addu     = 0 ;
    int kprefix_not      = 0 ;
    int kprefix_addr     = 0 ;
    int kprefix_ptr      = 0 ;
    
    while ( (tPrefix=parserIsCheckPrefix(pPar,pScope)) ) 
    {
        if ( tPrefix==eToken_null ) break ;

        pPar->p0->type = tPrefix ;
        pPar->p0->ptr=0;
        
        if ( tPrefix == eToken_subu )
            if ( kprefix_subu++ > 0 )
                { $parserErrorExtra( parsing,opNotAllowed,"Allow only one prefix unary ‘-’." ) ; return NULL ; } ;
        if ( tPrefix == eToken_addu )
            if ( kprefix_addu++ > 0 )
                { $parserErrorExtra( parsing,opNotAllowed,"Allow only one prefix unary ‘+’." ) ; return NULL ; } ;
        if ( tPrefix == eToken_not )
            if ( kprefix_not++ > 0 )
                { $parserErrorExtra( parsing,opNotAllowed,"Allow only one prefix unary ‘!’." ) ; return NULL ; } ;
        if ( tPrefix == eToken_addr )
            if ( kprefix_addr++ > 0 )
                { $parserErrorExtra( parsing,opNotAllowed,"Allow only one prefix unary ‘&’." ) ; return NULL ; } ;
        if ( tPrefix == eToken_ptr )
            if ( kprefix_ptr++ > 0 )
                { $parserErrorExtra( parsing,opNotAllowed,"Allow only one prefix unary ‘*’." ) ; return NULL ; } ;

        //if (( kprefix_subu + kprefix_addu + kprefix_not + kprefix_addr + kprefix_ptr )>2)
        //   { $parserErrorExtra( parsing,opNotAllowed,"Allow up to two unary operators." ) ; return NULL ; } ;
            
        if (tPrefix==eToken_type) 
        {
            if ( pPar->p1->type == eToken_mul ) // se il token successivo indica un puntatore
            {
                pPar->p0->ptr=1;
                puts("not allowed cast type to pointer"); exit(-1);
            }
        }
        // viene eseguito un controllo su p1 per evitare di modificare p0
        stackPush( (*pstackPrefixToken), pPar->p0 ) ;

        parserNextGetToken(pPar);
        
        // se ora trova il puntaore (skip) già verificato nella if precedente pPar-p1
        if ( pPar->p0->type == eToken_mul )    parserNextGetToken(pPar);
        
        if (pPar->p0->type==eToken_pv) { parserNextGetToken( pPar ) ; return pNode ;}
    }

    left = parseDotOp ( pPar , pNode  , pScope ) ;
    
    node_t* firstLeft = left ;
    
    if ( !stackEmpty( (*pstackPrefixToken) )  )
    {
        if ( left == NULL ) { $parserErrorExtra( parsing,expectedExpression,"unary LHS" ) ; return NULL ; }

        while( !stackEmpty( (*pstackPrefixToken) ) )
        {
            if (firstLeft) // CHECK INVALID REAL and Signed
            {
                if ( ( firstLeft->pData->type       == eToken_real   )
                ||     ( firstLeft->pData->type_num == eTermType_s8  )
                ||     ( firstLeft->pData->type_num == eTermType_s16 )
                ||     ( firstLeft->pData->type_num == eTermType_s32 )
                ||     ( firstLeft->pData->type_num == eTermType_s64 ) )
                {
                    if (( stackTop((*pstackPrefixToken))->type == eToken_ptr ) ) 
                        { $parserErrorExtra( parsing,invTypeArgUnaryPtr        ,firstLeft->pData->buffer.data ) ; }
                    if (( stackTop((*pstackPrefixToken))->type == eToken_addr )) 
                        { $parserErrorExtra( parsing,invTypeArgUnaryAddrOf    ,firstLeft->pData->buffer.data ) ; }
                }
            }

            if ( stackTop((*pstackPrefixToken))!=NULL ) 
            {
                left = astMakeNodePrefix    ( pPar, stackTop((*pstackPrefixToken)) , left  ) ;
                stackPop((*pstackPrefixToken)) ;
            }
        }
        return left;
    }
    
    stackDealloc( (*pstackPrefixToken) ) ;
    
    delete( pstackPrefixToken) ;
    
    return left ;
}

//...................................................................... parseMulDivMod [ * / : ]

node_t*        parseMulDivMod( parser_t *pPar , node_t *pNode , scope_t* pScope )
{
    node_t *left=NULL;

    if ( pPar->p0==NULL )  return pNode; 

    left = parsePrefix( pPar ,  pNode  , pScope ) ;
    
    if (pPar->p0->type==eToken_pv) { parserNextGetToken( pPar ) ; return left ;}
    
    if ( pPar->p0==NULL )  return left ; 

    while ( (pPar->p0->type == eToken_mul)     // ...................  Left-to-right
    ||      (pPar->p0->type == eToken_div)
    ||      (pPar->p0->type == eToken_mod)    )
    {
        if ( left == NULL ) { $parserErrorExtra( parsing,expectedExpression,"*/:  LHS" ) ; break ; }
        
        token_t* pTokenType = pPar->p0 ;

        parserNextGetToken( pPar ) ;

        if ( pPar->p0==NULL ) { $parserErrorExtra( parsing,expected,"eof */: RHS" ) ; break ; }

        node_t* right =  parsePrefix( pPar ,  NULL  , pScope ) ;

        if ( right == NULL ) { $parserErrorExtra( parsing,expected,"*/: RHS" ) ; break ; }

        left = astMakeNodeBinOp    ( pPar  , pTokenType , left   , right , pScope ) ;
        
        if (pPar->p0->type==eToken_pv) { parserNextGetToken( pPar ) ; return left ;}        
        if ( pPar->p0 ==    NULL ) break ;
    } ;

    return left ;
}

//...................................................................... parseAddSubOp [ + - ]

node_t*        parseAddSubOp( parser_t *pPar , node_t *pNode  , scope_t* pScope )
{
    node_t *left=NULL;

    if ( pPar->p0==NULL )  return pNode; 

    left = parseMulDivMod( pPar ,  pNode , pScope ) ;
    
    if (pPar->p0->type==eToken_pv) { parserNextGetToken( pPar ) ; return left ;}
    if ( pPar->p0==NULL )  return left ; 

    while ( (pPar->p0->type == eToken_add)     // ...................  Left-to-right
    ||      (pPar->p0->type == eToken_sub)    )
    {
        if ( left == NULL ) { $parserErrorExtra( parsing,expectedExpression,"+- LHS" ) ; break ; }
        
        token_t* pTokenType = pPar->p0 ;

        parserNextGetToken( pPar ) ;

        if ( pPar->p0==NULL ) { $parserErrorExtra( parsing,expected,"eof +- RHS" ) ; break ; }

        node_t* right =  parseMulDivMod( pPar ,  NULL  , pScope ) ;

        if ( right == NULL ) { $parserErrorExtra( parsing,expected,"+- RHS" ) ; break ; }

        // ## canonicalize 'ptr' se ( num + ptr ) -> segue ( ptr + num ) in astMakeNodeBinOp
        left = astMakeNodeBinOp    ( pPar  , pTokenType , left   , right , pScope ) ;
        
        if (pPar->p0->type==eToken_pv) { parserNextGetToken( pPar ) ; return left ;}        
        if ( pPar->p0 ==    NULL ) break ;
    } ;

    return left ;
}

//...................................................................... parseAssign [ := ]

// n.b.     allow := only 1    assignement -> a := b          [v]
//      not allow multiple     assignement -> a := b := c     [x]

node_t*        parseAssign( parser_t *pPar , node_t *pNode , scope_t* pScope )
{
    node_t *left=NULL;

    if ( pPar->p0==NULL )  return pNode; 

    left = parseAddSubOp( pPar ,  pNode , pScope ) ;

    if ( pPar->p0==NULL )  return left ; 

    if (    (pPar->p0->type == eToken_assign)    )  // ...................  left to right ( no multi assignment )
    {
        if ( left == NULL )             { $parserErrorExtra( parsing,expectedExpression ,":= LHS" ) ; return NULL ; }


        // blocca tutte le costanti in LHS ( emetti warning o info per ( const u8 / u16 ) C64.
        if ( left->nType     == nTypeTerm )    
            if ( left->baseKind == eTermKind_const )    
                { $parserWarningExtra( parsing,LValueRequired ,":= LHS" ) ; /*return NULL ;*/ }

        // Nel commodore 64 è legale fare poke 53280,1 cioè 53280 = 1 oppoure poke 1,0
        // tuttavia non sappiamo se doverr mettere byte / word; Consentito solo per u8 e u16

        if ( left->nType     == nTypeTerm )    
            if ( left->baseKind == eTermKind_const )
                if (    ( left->baseType != eTermType_u8 ) && ( left->baseType != eTermType_u16 ) )
                { 
                    $parserErrorExtra( parsing,LValueRequired ,":= LHS" ) ; 
                    return NULL ; 
                }
            
        token_t* pTokenSave = pPar->p0 ;
        
        if ( pPar->p0==NULL )  return left ;

        parserNextGetToken( pPar ) ;

        node_t* right =  parseAddSubOp( pPar ,  NULL  , pScope ) ;

        if ( pPar->p0 ) if (pPar->p0->type == eToken_assign) 
            { $parserErrorExtra( parsing,notAllowedMultipleAssign,":=" ) ; return NULL ; }

        if ( right == NULL ) { $parserErrorExtra( parsing,expectedExpression,":= RHS" ) ; return NULL ; }

        left = astMakeNodeBinOp    ( pPar , pTokenSave , left ,right , pScope ) ; // se come nel nostro caso 1 sola assegnazione

        return left ;
    } ;

    return left ;
}

//...................................................................... parseExpr

node_t*        parseExpr( parser_t *pPar , node_t *pNode , scope_t* pScope )
{
    if ( pPar->p0 == NULL ) return pNode ;

    pNode = parseAssign( pPar , pNode , pScope ) ;

    return    pNode;
}

