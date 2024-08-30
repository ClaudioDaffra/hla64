
#include "../lib/cxx.h"
#include "Cmos.h"


//...................................................................... lexerNew

lexer_t*    lexerNew(streamMBChar_t* pStream)
{
    lexer_t* pLex = (lexer_t*) new (lexer_t) ;

    pLex->pStream = pStream ;
    

    // ............................................................................. debug
    
    pLex->pFileDebugLexer   = NULL ;

    if ( pLex->pStream->pCompiler->fDebug )
    {
        pLex->pFileDebugLexer  = streamOpen( pLex->pStream->pCompiler->fileInputName.data , "w+" , sender_lexer  ) ;
    }

    // ............................................................................. lexema_t
    
    pLex->pLexema = (lexema_t*)  new ( lexema_t)  ;    
    
    pLex->pLexema->c0        = NULL     ;    
    pLex->pLexema->c1        = NULL     ;    
    pLex->pLexema->char0     = 0        ;    
    pLex->pLexema->char1     = 0        ;        
    pLex->pLexema->ndx       = 0        ;  
    
    // ............................................................................. start/end
    
    pLex->lexPosStart       =    0 ;
    pLex->lexPosEnd         =    0 ; 


    // ............................................................................. vector_token_t

    vector_token_t*    pToken = (vector_token_t*) gcMalloc( sizeof(vector_token_t) ) ;

    pLex->pvToken = pToken ;
    
    vectorAlloc( (*pLex->pvToken),128 );

    // ............................................................................. 
    
    return pLex ;
}

//...................................................................... lexerDelete

void    lexerDelete(lexer_t* pLex)
{
    vectorDealloc( (*pLex->pvToken) );
        
    gcFree(pLex->pLexema);  
    
    if ( pLex->pStream->pCompiler->fDebug )
    {
        gcFree( pLex->pFileDebugLexer ) ;
    }
    delete(pLex);
}

//...................................................................... lexerGetLexema

char* lexerGetSourceFromString(lexer_t *pLex,char* string)
{
    return pLex->pStream->pSource->data = string ; 
}

//...................................................................... lexerIsLastChar

int lexerIsLastChar( lexer_t *pLex )
{
    return ( pLex->pLexema->ndx+1 >= pLex->pStream->pvCharMB->size ) ;
}

//...................................................................... lexerGetLexema

uint32_t lexerGetLexema( lexer_t *pLex )
{
    if ( lexerIsLastChar(pLex) )
    {
        pLex->pLexema->c0 = NULL ;
        pLex->pLexema->c1 = NULL ;
        return pLex->pLexema->ndx ;
    }
    
    pLex->pLexema->c0         = pLex->pStream->pvCharMB->data[pLex->pLexema->ndx]->mbs    ;
    pLex->pLexema->char0      = pLex->pStream->pvCharMB->data[pLex->pLexema->ndx]->c32    ;
    
    if ( (pLex->pLexema->ndx + 1) < pLex->pStream->pvCharMB->size ) 
    { 
        pLex->pLexema->c1         = pLex->pStream->pvCharMB->data[pLex->pLexema->ndx+1]->mbs    ; 
        pLex->pLexema->char1      = pLex->pStream->pvCharMB->data[pLex->pLexema->ndx+1]->c32    ; 
    } 
    else 
    { 
        pLex->pLexema->c1         = NULL    ;
        pLex->pLexema->char1      = 0       ;
    } ;
 
    return pLex->pLexema->ndx ;
}

//...................................................................... lexerGetLexemaNext

uint32_t lexerGetLexemaNext( lexer_t *pLex )
{
    lexerGetLexema( pLex );

    return ++pLex->pLexema->ndx ;
}


//...................................................................... lexerunGetLexema

uint32_t lexerunGetLexema( lexer_t *pLex )
{
    if ( (pLex->pLexema->ndx - 1)  >= 1 ) --pLex->pLexema->ndx ;
    
    lexerGetLexema( pLex ) ; 
 
    return pLex->pLexema->ndx ;
}
 
//...................................................................... isLexerEOF

uint32_t isLexerEOF( lexer_t *pLex )
{
    return !(pLex->pLexema->ndx    <    pLex->pStream->pvCharMB->size) ; // skip NULL
}

//...................................................................... c0 , c1
/*
char* sc0( lexer_t *pLex )
{
    return pLex->pLexema->c0;
}
 */
uint32_t c0( lexer_t *pLex )
{
    return pLex->pLexema->char0;
}

uint32_t c1( lexer_t *pLex )
{
    return pLex->pLexema->char1;
}

//...................................................................... lexerPushToken

uint32_t lexerPushToken( lexer_t *pLex , token_t* pToken  )
{
    vectorPushBack( (*pLex->pvToken) , pToken ) ;

    return vectorSize( (*pLex->pvToken) ) ;
}

//...................................................................... lexerMakeDefaultToken

token_t* lexerMakeDefaultToken( lexer_t *pLex )
{
    token_t* pToken = (token_t*) gcCalloc( sizeof(token_t) ,1);

    stringAlloc ( pToken->buffer , 8 ) ; //  buffer temporaneo di salvataggio strcat mbs
    
    stringFromS8( (pToken->buffer),"" ); // 1234
    
    pToken->tPosStart   = 0;
    pToken->tPosEnd     = 0;
    pToken->type        = eToken_null;
    pToken->type_num    = eTermType_null;
    pToken->type_suffix = NULL;
    pToken->mbs         = NULL;
    pToken->f40s        = NULL;    
    pToken->row         = pLex->pStream->pvCharMB->data[ pLex->lexPosStart-1 ]->row ;
    pToken->col         = pLex->pStream->pvCharMB->data[ pLex->lexPosStart-1 ]->col ;
    
    return pToken;
}

//...................................................................... lexerDebugToken

void lexerDebugToken(  lexer_t *pLex , token_t* pToken )
{
    if ( pToken != NULL )
    {
        uint32_t posS    = pToken->tPosStart    ;
        uint32_t posE    = pToken->tPosEnd    ;  
                
        fprintf ( pLex->pFileDebugLexer->ptr , "\nT[%3d] P[%3d] RC[%3d,%3d]..[%3d,%3d] L[%2d] B[%2d] "
            ,(int)pLex->pStream->pvCharMB->data[posS]->type        
            ,(int)pLex->pStream->pvCharMB->data[posS]->pos
            ,(int)pLex->pStream->pvCharMB->data[posS]->row 
            ,(int)pLex->pStream->pvCharMB->data[posS]->col
            ,(int)pLex->pStream->pvCharMB->data[posE]->row
            ,(int)pLex->pStream->pvCharMB->data[posE]->col 
            ,(int)pLex->pStream->pvCharMB->data[posS]->lvl
            ,(int)pLex->pStream->pvCharMB->data[posS]->blk
        );

        switch ( pToken->type ) 
        {
            case eToken_null:
                fprintf ( pLex->pFileDebugLexer->ptr , "%-30s","pToken = eToken_null "    );
                fprintf ( pLex->pFileDebugLexer->ptr , "\n[%3d]", 0 ); 
                
                break;
                
            case eToken_mbs    :
                fprintf ( pLex->pFileDebugLexer->ptr , "%-30s","pToken = eToken_mbs"        );
                fprintf ( pLex->pFileDebugLexer->ptr , "         len(%3d)::\n[\n%20s\n]", (int)strlen(pToken->mbs),pToken->mbs  );    
                
                break;
                
            case eToken_integer    :
            
                fprintf ( pLex->pFileDebugLexer->ptr , "%-30s","pToken = eToken_integer"        );

                fprintf ( pLex->pFileDebugLexer->ptr , " [%3d]/[%3d]{%3s}"
                    , pToken->type
                    , pToken->type_num
                    , pToken->type_suffix==NULL?"":pToken->type_suffix
                );
                // ## tutti i numeri inter sono contenuti in : unsigned long int.
#ifdef _MSC_VER
                fprintf ( pLex->pFileDebugLexer->ptr , "::(%20llu)", pToken->u64  ); 
#else
                fprintf ( pLex->pFileDebugLexer->ptr , "::(%20zu)" , pToken->u64  ); 
#endif    
                if ( pToken->buffer.data != NULL) 
                    fprintf ( pLex->pFileDebugLexer->ptr , " := {%20s}." , pToken->buffer.data ); 
                
                break;
                
            case eToken_real           :
            
                fprintf ( pLex->pFileDebugLexer->ptr , "%-30s","pToken = eToken_real"        );

                fprintf ( pLex->pFileDebugLexer->ptr , " [%3d]/[%3d]{%3s}"
                    , pToken->type
                    , pToken->type_num
                    , pToken->type_suffix==NULL?"":pToken->type_suffix
                );
                
                // ## tutti i numeri reali sono contenuti in : long double.
                fprintf ( pLex->pFileDebugLexer->ptr , "::(%20.9Lg)"    , pToken->r80  );
                
                fprintf ( pLex->pFileDebugLexer->ptr , " := {%20s}."     , pToken->buffer.data ); 
                fprintf ( pLex->pFileDebugLexer->ptr , " -> f40s(%s)"    , pToken->f40s==NULL?"":pToken->f40s );  

                break;    

            case eToken_id    :
                fprintf ( pLex->pFileDebugLexer->ptr , "%-30s","pToken = eToken_id"        );
                fprintf ( pLex->pFileDebugLexer->ptr , "         len(%3d)::[%20s]", (int)strlen(pToken->mbs),pToken->mbs  );    
                break;  
                
            case eToken_kw_include      :        //    0
            case eToken_kw_asm          :        //    1            
            case eToken_kw_if           :        //    2    
            case eToken_kw_else         :        //    3
            case eToken_kw_loop         :        //    4
            case eToken_kw_function     :        //    5
            case eToken_kw_return       :        //    6

            case eToken_kw_char         :        //    7
            case eToken_kw_byte         :        //    8
            case eToken_kw_int          :        //    9
            case eToken_kw_word         :        //    10
            case eToken_kw_real         :        //    11  
            
            case eToken_kw_program      :        //    12 
            case eToken_kw_end          :        //    13 
            
                fprintf ( pLex->pFileDebugLexer->ptr , "%-29s", "pToken = eToken_kw"    );
                fprintf ( pLex->pFileDebugLexer->ptr , "    [%3d] len(%3d)::[%20s]",pToken->type,  (int)strlen(pToken->mbs),pToken->mbs  );    
                break;
                
            case eToken_ellipsis              :
            case eToken_shift_left_assign     :
            case eToken_shift_right_assign    :
    
            case eToken_inc                   :
            case eToken_dec                   :
            case eToken_le                    :
            case eToken_ge                    :
            case eToken_assign                :
            case eToken_ne                    :
            case eToken_eq                    :    
    
            case eToken_p0                    :
            case eToken_p1                    :
            
            case eToken_mul                   :
            case eToken_ptr                   :
            
            case eToken_div                   :
            case eToken_mod                   :
            
            case eToken_add                   :    // + plus
            case eToken_addu                  :    // - unary plus
            
            case eToken_sub                   :    // - minus
            case eToken_subu                  :    // - unary neg
            
            case eToken_lt                    :
            case eToken_gt                    :
            case eToken_not                   :
            case eToken_g0                    :
            case eToken_g1                    :
            case eToken_dp                    :    //    :
            
            case eToken_and                   :    // &&
            case eToken_addr                  :    // &

            case eToken_pv                    :
            case eToken_dot                   :
            case eToken_return                :    // ->

            case eToken_q0                    :    // [
            case eToken_q1                    :    // ]
            
                fprintf ( pLex->pFileDebugLexer->ptr  , "%-29s", "pToken = eToken_op"         );
                fprintf ( pLex->pFileDebugLexer->ptr , "    [%3d] len(%3d)::[%20s]",pToken->type, (int)strlen(pToken->mbs),pToken->mbs );  
                
                break;
                
            case eToken_char    :
                fprintf ( pLex->pFileDebugLexer->ptr  , "%-30s", "pToken = eToken_char"         );
                fprintf ( pLex->pFileDebugLexer->ptr , "         len(%3d)::[%20s]"
                        , (int)strlen(pToken->mbs),streamConvertSpecialCharinMBS(pToken->mbs)   );
                break;   

            case eToken_type    : // viene usato dal parser (cast) qui no.
                fprintf ( pLex->pFileDebugLexer->ptr  , "%-30s", "pToken = eToken_type"         );
                fprintf ( pLex->pFileDebugLexer->ptr , "         len(%3d)::[%20s]"
                        , (int)strlen(pToken->mbs),streamConvertSpecialCharinMBS(pToken->mbs)   );
                break;   
                
            default:
                fprintf ( stdout , "\n?? Internal :: lexerDebugToken : pToken = unknown [%d]. \n",pToken->type);
                exit(-1);
            break;
        }
        
        // tutti i token hanno come stringa : pToken->buffer.data
        // if ( pToken->buffer.data != NULL) fprintf ( pLex->pFileDebugLexer->ptr , " Buffer{%20s}." , pToken->buffer.data ) ;
            
    }
    else
    {
        fprintf (  stdout  , "\n?? lexerDebugToken : pToken = NULL");
        exit(-1);        
    }
}

//...................................................................... lexerDebugAllTokens

void lexerDebugAllTokens( lexer_t *pLex )
{
 fprintf ( pLex->pFileDebugLexer->ptr , "\nsize :: [[%d]]\n", (int)vectorSize( (*pLex->pvToken) ) );

 for ( uint32_t i=0;i<vectorSize( (*pLex->pvToken) ) ; i++ )
 {
     lexerDebugToken( pLex , pLex->pvToken->data[i] ) ;
 };

} 

//...................................................................... lexerMakeTokenChar

token_t* lexerMakeTokenChar( lexer_t *pLex , token_t* pToken , char* mbs )
{
    pToken->tPosStart    = pLex->lexPosStart     ;
    pToken->tPosEnd     = pLex->pLexema->ndx    ;
    pToken->mbs         = mbs;
    pToken->type        = eToken_char;
    pToken->ptr         = 0; 
    // questo è un kludge ( $word ) per permettere ai token prefissi ( meglio cast ) di diventare anche puntatori ( $word 0 ) 
    
    return pToken;
}

//...................................................................... ## 
//
//    lexerIsLastChar 
//    lexerIsSpace
//    lexerIsNumber
//    lexerIsAlpha
//    lexerIsIdent0
//    lexerIsIdent1
//    lexerIsHexNumber
//    lexerIsBinNumber
//    lexerIsOctNumber

int lexerIsSpace( lexer_t *pLex )
{
    //    https://en.cppreference.com/w/cpp/locale/isspace
    //    isspace(' ', locale("C"))             returned false
    //    isspace(' ', locale("en_US.UTF8"))     returned true

    uint32_t ch = c0(pLex) ;

    if ( ch==0 ) return 0 ;    
    if ( ch<32 ) return 1 ;
    
    if (c0(pLex)==(uint32_t)' ' ) return 1;
    if (c0(pLex)==(uint32_t)'\r') return 1;
    if (c0(pLex)==(uint32_t)'\n') return 1;
    if (c0(pLex)==(uint32_t)'\t') return 1;    
    if (c0(pLex)==(uint32_t)'\f') return 1;
    if (c0(pLex)==(uint32_t)'\v') return 1;
    
    if (c0(pLex)==(uint32_t)0x00003000UL ) return 1 ; // full block space

    return 0;
}

//...................................................................... lexerIsNumber

int lexerIsNumber( lexer_t *pLex )
{
    if  (     (c0(pLex)>=(uint32_t)'0' ) 
    &&        (c0(pLex)<=(uint32_t)'9' )    )    return 1;

    return 0;
}

//...................................................................... lexerTokenAheadIsNumber

int lexerTokenAheadIsNumber( lexer_t *pLex )
{
    if  (     (c1(pLex)>=(uint32_t)'0' ) 
    &&        (c1(pLex)<=(uint32_t)'9' )    )    return 1;

    return 0;
}

//...................................................................... lexerIsAlpha

int lexerIsAlpha( lexer_t *pLex )
{
    if  (     (c0(pLex)>=(uint32_t)'a' ) 
    &&        (c0(pLex)<=(uint32_t)'z' )    )    return 1;

    if  (     (c0(pLex)>=(uint32_t)'A' ) 
    &&        (c0(pLex)<=(uint32_t)'Z' )    )    return 1;

    return 0;
}

//...................................................................... lexerIsIdent0

int lexerIsIdent0( lexer_t *pLex )
{
    if ( lexerIsAlpha(pLex) )     return 1 ;
    if (c0(pLex)==(uint32_t)'_' ) return 1 ;
    if (c0(pLex)==(uint32_t)'$' ) return 1 ;
    
    return 0;
}

//...................................................................... lexerIsIdent1

int lexerIsIdent1( lexer_t *pLex )
{
    if ( lexerIsIdent0(pLex)    ) return 1 ;
    if ( lexerIsNumber(pLex)    ) return 1 ;

    return 0;
}

//...................................................................... lexerIsHexNumber

int lexerIsHexNumber( lexer_t *pLex )
{
    if ( lexerIsNumber(pLex)          )    return 1;    
    if ((c0(pLex)>=(uint32_t)'a' ) 
    &&  (c0(pLex)<=(uint32_t)'f' )    )    return 1;
    if ((c0(pLex)>=(uint32_t)'A' ) 
    &&  (c0(pLex)<=(uint32_t)'F' )    )    return 1;

    return 0;
}

//...................................................................... lexerIsBinNumber

int lexerIsBinNumber( lexer_t *pLex )
{
    if  (     (c0(pLex)>=(uint32_t)'0' ) 
    &&        (c0(pLex)<=(uint32_t)'1' )    )    return 1;

    return 0;
}

//...................................................................... lexerIsOctNumber

int lexerIsOctNumber( lexer_t *pLex )
{
    if  (     (c0(pLex)>=(uint32_t)'0' ) 
    &&        (c0(pLex)<=(uint32_t)'7' )    )    return 1;

    return 0;
}

//...................................................................... lexerSkipForBackSlashNewLine

int lexerSkipForBackSlashNewLine( lexer_t *pLex )
{
    if ( ( c0(pLex) == '\\' ) && ( c1(pLex) == '\n' ) )
    {
        lexerGetLexemaNext( pLex ) ;
        return 1;
    }
    return 0 ;
}    

//...................................................................... lexerScanForSingleLineComment

int lexerScanForSingleLineComment( lexer_t *pLex )
{
    if ( (c0(pLex)=='/') && (c1(pLex)=='/') )
    {
        while( (c0(pLex))!='\n' )
        {
            if ( lexerIsLastChar(pLex) ) break ;
            
            lexerGetLexemaNext( pLex ) ;
        }
        lexerunGetLexema(pLex);
        return 1;
    }        
    return  0;        
}

//...................................................................... lexerScanForMultiLineComment

int lexerScanForMultiLineComment( lexer_t *pLex )
{
    // check initial open comment
    if ( (c0(pLex)==(uint32_t)'*') && (c1(pLex)==(uint32_t)'/') )
    {
            lexerGetLexemaNext( pLex ) ; // /
            $lexerErrorExtra( tokening,multiLineComment," * / ... <char>" );
            return 1 ;
    }

    // multi comment
    if ( (c0(pLex)==(uint32_t)'/') && (c1(pLex)==(uint32_t)'*') )
    {
        lexerGetLexemaNext( pLex ) ; // /    ->    *
        lexerGetLexemaNext( pLex ) ; // *    ->    c0    c1
        
        while (   1   )   
        {
            if ( ((c0(pLex))==(uint32_t)'*') && ((c1(pLex))==(uint32_t)'/') ) break ;
                
            // check missing close comment
            if    ( pLex->pLexema->ndx+1 >=    pLex->pStream->pvCharMB->size )
            {
                $lexerErrorExtra( tokening,multiLineComment," / * ... <EOF> " );
                return 0 ;
            }    
            lexerGetLexemaNext( pLex ) ;
        }
        lexerGetLexemaNext( pLex ) ;  
        return 1;                        
    }        
    return  0;        
}

//...................................................................... lexerScanForSpace

int lexerScanForSpace( lexer_t *pLex )
{
    if ( lexerIsSpace(pLex) )
    {
        while( lexerIsSpace(pLex) )
        {
            if ( lexerIsLastChar(pLex) )  return 1 ;
            lexerGetLexemaNext( pLex ) ;
        }
        lexerunGetLexema( pLex );
        return 1;
    }        
    return  0;        
}

//...................................................................... lexePushMBSinBufferString

void lexePushMBSinBufferString( token_t* pToken , char *mbs ) 
{
    for (uint32_t i=0;i<strlen(mbs);i++)
    {
        stringPushBack ( pToken->buffer , mbs[i] ) ;
    }
}

//...................................................................... lexerPushC0andGetNext

void lexerPushC0andGetNext( lexer_t *pLex , token_t* pToken )
{
    lexePushMBSinBufferString ( pToken ,    pLex->pStream->pvCharMB->data[ pLex->pLexema->ndx-1 ]->mbs ) ;
    lexerGetLexemaNext( pLex ) ;
}

//...................................................................... lexerScanForIntegerOrReal
//
//  lexerScanForNumberNext
//  lexerScanForIntegerOrReal
//

int lexerScanForNumberNext( lexer_t *pLex , token_t* pToken )
{
    if ( lexerIsNumber(pLex) )
    {
        while( lexerIsNumber(pLex) )
        {
            lexerPushC0andGetNext( pLex , pToken ) ;
            if ( lexerIsLastChar(pLex) )  break ;
        }
        return 1 ;
    }
    return 0 ;
}

//...................................................................... lexerIsSuffix

int lexerIsSuffix( lexer_t *pLex )
{
    uint32_t suffix = c0(pLex);
    
    switch ( tolower(suffix) ) 
    {
        case 'u':
        case 's':
        case 'r':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '6':
        case '8':
            return 1;
            break;
        default:
            return 0;
            break;
    }

    return 0;
}

//...................................................................... lexerScanForSuffix

int lexerScanForSuffix( lexer_t *pLex , token_t* pToken )
{
    if ( lexerIsSuffix(pLex) )
    {
        while( lexerIsSuffix(pLex) )
        {
            lexerPushC0andGetNext( pLex , pToken ) ;
            if ( lexerIsLastChar(pLex) )  break ;
        }
        return 1 ;
    }
    return 0 ;
}

//...................................................................... suffix[]

char *suffix[]    =
{ 
    "u8"    ,
    "u16"    ,
    "u32"    ,
    "u64"    ,
    
    "s8"    ,
    "s16"    ,
    "s32"    ,
    "s64"    ,
    
    "r32"    ,
    "r40"    ,
    "r64"    ,
    "r80"    ,
    
    NULL 
} ;

//...................................................................... lexerIsValidSuffix

int lexerIsValidSuffix(char *bufferSuffixStr)
{
    if (strlen(bufferSuffixStr)==0) return 1 ;
    
    int i=0;
    
    do    {
        
        if ( strcmp( bufferSuffixStr , suffix[i] ) == 0 ) return eTermType_null+i+1 ;
        
    } while ( suffix[++i] != NULL ) ;

    return eTermType_null ;
}

//...................................................................... convert_dieeetomsbin

int convert_dieeetomsbin(const double src8, double *dest8) 
{
    unsigned char ieee[8];

    unsigned char *msbin = (unsigned char *)dest8;
    unsigned char sign;
    unsigned char any_on = 0;
    short msbin_exp;
    int i;

    // Make a clobberable copy of the source number
    memcpy(ieee, &src8, 8);

    for (i = 0; i < 8; i++)
        msbin[i] = 0;

    // If all are zero in src8, the msbin should be zero
    for (i = 0; i < 8; i++)
        any_on |= ieee[i];
    
    if (any_on == 0) {
        return 0;
    }

    sign = ieee[7] & 0x80;
    
    msbin[6] |= sign;
    
    msbin_exp = (ieee[7] & 0x7f) << 4;
    msbin_exp += (ieee[6] >> 4);

    if ((msbin_exp - 0x3ff) > 0x80) 
    {
        return 1;
    }

    msbin[7] = msbin_exp - 0x3ff + 0x80 + 1;

    // The ieee mantissa must be shifted up 3 bits
    ieee[6] &= 0x0f; // mask out the exponent in the second byte
    for (i = 6; i >= 1; i--) {
        msbin[i] |= (ieee[i] << 3);
        msbin[i] |= (ieee[i - 1] >> 5);
    }
    msbin[0] |= (ieee[0] << 3);

    return 0;
}

//...................................................................... convertDoubleToMBF

char* convertDoubleToMBF( double fs  )
{
    double fd = 0 ;
    
    convert_dieeetomsbin( fs, &fd )  ;

    unsigned char *mbf = (unsigned char *)&fd;

    char f40s[128] ;
    snprintf ( f40s , 128 , "$%02x,$%02x,$%02x,$%02x,$%02x" , mbf[7],mbf[6],mbf[5],mbf[4],mbf[3] ) ;

    return gcStrDup((char*)f40s);
}

//...................................................................... convertDoubleToMBF_5

void convertDoubleToMBF_5( double fs  , unsigned char* f40c  )
{
    double fd = 0                 ;
    
    convert_dieeetomsbin( fs, &fd )      ;

    unsigned char *mbf = (unsigned char *)&fd;

    //char f40s[128] ;
    //snprintf ( f40s , 128 , "$%02x,$%02x,$%02x,$%02x,$%02x" , mbf[7],mbf[6],mbf[5],mbf[4],mbf[3] ) ;
    
    f40c[0] =  mbf[7] ;
    f40c[1] =  mbf[6] ;
    f40c[2] =  mbf[5] ;
    f40c[3] =  mbf[4] ;
    f40c[4] =  mbf[3] ;
    
}

//...................................................................... convert_fieeetomsbin

int convert_fieeetomsbin(const float src4, float *dest4) 
{
    uint8_t *ieee  = (uint8_t *)&src4;
    uint8_t *msbin = (uint8_t *)dest4;
    uint8_t sign;
    uint8_t msbin_exp;
    int i;

    msbin_exp = 0;
    // See _fmsbintoieee() for details of formats
    sign = ieee[3] & 0x80;
    msbin_exp |= (ieee[3] << 1);
    msbin_exp |= (ieee[2] >> 7);
    // An ieee exponent of 0xfe overflows in MBF
    if (msbin_exp == 0xfe) {
        return 1;
    }
    msbin_exp += 2; // actually, -127 + 128 + 1
    for (i = 0; i < 4; i++) {
        msbin[i] = 0;
    }
    msbin[3] = msbin_exp;
    msbin[2] |= sign;
    msbin[2] |= (ieee[2] & 0x7f);
    msbin[1] = ieee[1];
    msbin[0] = ieee[0];
    
    return 0;
}

//...................................................................... onvertFloatToMBF

char* convertFloatToMBF( float fs  )
{
    float fd = 0 ;
    
    convert_fieeetomsbin( fs, &fd )  ;

    unsigned char *mbf = (unsigned char *)&fd;

    char f40s[128] ;
    snprintf ( f40s , 128 , "%02x,%02x,%02x,%02x,%02x" , mbf[7],mbf[6],mbf[5],mbf[4],mbf[3] ) ;
    
    return gcStrDup((char*)f40s);
}

//...................................................................... lexerScanForIntegerOrReal

int lexerScanForIntegerOrReal( lexer_t *pLex , token_t* pToken )
{
    eToken_t numType = eToken_null ;

    if ( lexerIsNumber(pLex) )    //    ........................................ [0-9]
    {
        numType = eToken_integer ;
        lexerScanForNumberNext( pLex, pToken ) ;
    }

    if ( c0(pLex)==(uint32_t)'.' )    //    .................................... [.]
    {
        if ( !lexerTokenAheadIsNumber(pLex) )
        {
            return 0 ; // [.] è un operatore
        }
        
        numType = eToken_real ;

        lexerPushC0andGetNext( pLex , pToken ) ;
        
        if ( lexerIsNumber(pLex) )    //    .................................... [0-9]
        {
            lexerScanForNumberNext( pLex, pToken ) ;
        }
    }

    if ( numType != eToken_null )
    {
        if ( (c0(pLex)==(uint32_t)'e') //    ................................... [eE]
        ||   (c0(pLex)==(uint32_t)'E')  )
        {
            lexerPushC0andGetNext( pLex , pToken ) ;

            if ( (c0(pLex)==(uint32_t)'+') //    ............................... [+-]
            ||   (c0(pLex)==(uint32_t)'-')  )
            {
                lexerPushC0andGetNext( pLex , pToken ) ;
            }
            
            if ( lexerIsNumber(pLex) )    //    ................................ [0-9]
            {
                lexerScanForNumberNext( pLex, pToken ) ;
            }            
            else
            {
                $lexerErrorExtra( tokening,expWithoutDigits,pToken->buffer.data ) ;
            }
        }
    }

    // scan for suffix get str and pos.
    
    int bufferSuffixPos = strlen( pToken->buffer.data );

    // u8-16-32-64
    // s8-16-32-64
    // r -32-40-80
    int fSuffix = lexerScanForSuffix(pLex,pToken) ;
    
    char* bufferSuffixStr = &pToken->buffer.data[bufferSuffixPos];

    eTermType_t isSuffix = 0 ;
    
    if ( fSuffix ) isSuffix = lexerIsValidSuffix(bufferSuffixStr) ;

    if ( fSuffix && !isSuffix )
    {
        $lexerErrorExtra( tokening,invalidSuffix,bufferSuffixStr ) ;
    }

    //
    
    if ( numType == eToken_null ) return 0 ;

    //
    
    char*    end         = NULL    ;
    
    pToken->tPosStart   = pLex->lexPosStart     -1  ;
    pToken->tPosEnd     = pLex->pLexema->ndx    -1  ;
    pToken->mbs         = pToken->buffer.data       ;
    pToken->type        = numType                    ;

    pToken->type_suffix = bufferSuffixStr            ;
    pToken->f40s         = NULL                        ;
    
    if ( numType==eToken_integer    ) 
    {
        uint64_t         number     = (uint64_t) strtod(pToken->buffer.data, &end ) ;
        pToken->u64             = number ;
        pToken->type_num        = eTermType_u64 ;
    }
    
    if ( numType==eToken_real    )
    {
        long double number     = strtold (pToken->buffer.data, &end) ;
        pToken->r80         = number ;
        pToken->type_num    = eTermType_r80 ;
 
        double         numberReal64     = strtod (pToken->buffer.data, &end) ;

        pToken->f40s        = convertDoubleToMBF( numberReal64 ) ;
        
        convertDoubleToMBF_5( numberReal64,&pToken->f40c[0] ) ;
    }
    
    if ( isSuffix )
    {
        pToken->type_num    = isSuffix                    ;
    }
    else
    {
        if ( (pToken->type==eToken_integer)) pToken->type_num    = eTermType_u64 ;
        if ( (pToken->type==eToken_real   )) pToken->type_num    = eTermType_r80 ;
    }

    if (errno == ERANGE)
    {
        $lexerErrorExtra( tokeningNumber,outOfRange,pToken->buffer.data ) ;
    }

    // check suffix overflow
    
    if ( isSuffix )
    {
        char extra[128] ;
        extra[0]=0;
        if ( pToken->type == eToken_integer) //pToken->u64
        {    
            if  ( (pToken->u64 > UCHAR_MAX    ) && (strcmp(pToken->type_suffix,"u8" )==0) ) { strcpy(extra,"u8");        } ;
            if  ( (pToken->u64 > SCHAR_MAX    ) && (strcmp(pToken->type_suffix,"s8" )==0) ) { strcpy(extra,"s8");        } ;
            if  ( (pToken->u64 > USHRT_MAX  ) && (strcmp(pToken->type_suffix,"u16")==0) ) { strcpy(extra,"u16");    } ;
            if  ( (pToken->u64 > SHRT_MAX    ) && (strcmp(pToken->type_suffix,"s16")==0) ) { strcpy(extra,"s16");    } ;
            if  ( (pToken->u64 > UINT_MAX   ) && (strcmp(pToken->type_suffix,"u32")==0) ) { strcpy(extra,"u32");    } ;
            if  ( (pToken->u64 > INT_MAX    ) && (strcmp(pToken->type_suffix,"s32")==0) ) { strcpy(extra,"s32");    } ;
        }
        
        if ( pToken->type == eToken_integer)
        {
            int err = 0;
            if  ( (strcmp(pToken->type_suffix,"r32")==0) ) err=1;
            if  ( (strcmp(pToken->type_suffix,"r40")==0) ) err=1;
            if  ( (strcmp(pToken->type_suffix,"r64")==0) ) err=1;
            if ( err ) $lexerErrorExtra( tokeningNumber,invalidSuffix,pToken->buffer.data ) ;
        }
        
        if ( pToken->type == eToken_real) //pToken->r80
        {    
            if  ( (pToken->r80 > FLT_MAX    ) && (strcmp(pToken->type_suffix,"r32")==0) ) { strcpy(extra,"r32");    } ;
            if  ( (pToken->r80 > FLT_MAX    ) && (strcmp(pToken->type_suffix,"r40")==0) ) { strcpy(extra,"r40");    } ;
            if  ( (pToken->r80 > DBL_MAX    ) && (strcmp(pToken->type_suffix,"r64")==0) ) { strcpy(extra,"r64");    } ;
        }

        if ( pToken->type == eToken_real)
        {
            int err = 0;
            if  ( (strcmp(pToken->type_suffix,"u8" )==0) ) err=1;
            if  ( (strcmp(pToken->type_suffix,"s8" )==0) ) err=1;
            if  ( (strcmp(pToken->type_suffix,"u16")==0) ) err=1;
            if  ( (strcmp(pToken->type_suffix,"s16")==0) ) err=1;
            if  ( (strcmp(pToken->type_suffix,"u32")==0) ) err=1;
            if  ( (strcmp(pToken->type_suffix,"s32")==0) ) err=1;
            if ( err ) $lexerErrorExtra( tokeningNumber,invalidSuffix,pToken->buffer.data ) ;
        }
        
        if ( extra[0] != 0 )
        {
            $lexerErrorExtra( tokeningNumber,overflow,pToken->buffer.data ) ;
        }
    }
    
    //
    
    lexerPushToken( pLex , pToken  ) ;    

    //
    
    lexerunGetLexema(pLex);
    return 1 ;
}

//...................................................................... lexerMakeTokenIdent
//
//    lexerMakeTokenIdent
//    lexerIsKeyword
//    lexerScanForIdentOrKeyword
//

token_t* lexerMakeTokenIdent( lexer_t *pLex , token_t* pToken )
{
    pToken->tPosStart   = pLex->lexPosStart     -1  ;    //  l'attuale è il carattere di uscita
    pToken->tPosEnd     = pLex->pLexema->ndx    -1  ;    // -1 è l'ultimo carattere del token
    pToken->mbs         = pToken->buffer.data       ;
    pToken->type        = eToken_id;
    pToken->ptr         = 0; 
    
    return pToken;
}

//...................................................................... kw[]

char *kw[]    = // keep in order
{ 
    "include"     ,        //    0
    "asm"         ,        //    1
    "if"         ,        //    2
    "else"         ,        //    3
    "loop"         ,        //    4
    "function"     ,        //    5
    "return"     ,        //    6
    
    "primitive_type_begin"    ,
    "char"         ,        //    7
    "byte"         ,        //    8
    "int"         ,        //    9
    "word"         ,        //    10
    "real"         ,        //    11
    "primitive_type_end"    ,
    
    "program"     ,        //    12
    "end"         ,        //    13
    
    NULL 
} ;

//...................................................................... lexerIsKeyword

int lexerIsKeyword( char* mbs )
{
    int i=0;
    
    do    {
        
        if ( strcmp( mbs , kw[i] ) == 0 ) return eToken_op_begin_kw+1+i ;
        
    } while ( kw[++i] != NULL ) ;

    return 0 ;
}

//...................................................................... lexerScanForIdentOrKeyword

int lexerScanForIdentOrKeyword( lexer_t *pLex , token_t* pToken )
{
    if ( lexerIsIdent0(pLex) )
    {
        // push ident0 che abbiamo già letto (-1)
        lexerPushC0andGetNext( pLex , pToken ) ;
        
        while( lexerIsIdent1(pLex) )
        {
            lexerPushC0andGetNext( pLex , pToken ) ;
        }
        
        // makeIdentTken
        
        pToken = lexerMakeTokenIdent( pLex , pToken ) ;
        
        eToken_t isKW=eToken_null;
        
        isKW = lexerIsKeyword( pToken->mbs ) ;
        if ( isKW ) pToken->type=isKW ;
        
        lexerPushToken( pLex , pToken  ) ;

        lexerunGetLexema( pLex );
        return 1 ;
    }
    return  0;
}

//...................................................................... lexerScanForMBS/MBC
//
//    lexerMakeTokenMBS
//    lexerScanForSpecialChar
//    lexerScanForMBS
//    lexerScanForMBC
//

token_t* lexerMakeTokenMBS( lexer_t *pLex , token_t* pToken )
{
    pToken->tPosStart   = pLex->lexPosStart     -1  ;
    pToken->tPosEnd     = pLex->pLexema->ndx    -1  ;
    pToken->mbs         = pToken->buffer.data       ;
    pToken->type        = eToken_mbs;
    pToken->ptr         = 0; 
    
    return pToken;
}

//...................................................................... lexerScanForSpecialChar

int lexerScanForSpecialChar( lexer_t *pLex , token_t* pToken )
{
     if ( c0(pLex) == (uint32_t)'\\' )
    {
        lexerGetLexemaNext( pLex ) ; // skip slash(\)

        if ( isLexerEOF(pLex) )
        {
            $lexerErrorExtra( tokening,missingTerminating,pToken->buffer.data ) ;
            return 1 ;
        }
        else
        {
            char temp[2];            
            switch    ( (uint32_t)c0(pLex) )
            {
                case    'n':
                            stringPushBack ( pToken->buffer , '\n' ) ;
                break;
                case    '\"':
                            stringPushBack ( pToken->buffer , '\"' ) ;
                break;
                case    '\'':
                            stringPushBack ( pToken->buffer , '\'' ) ;
                break;
                case    '0':
                            stringPushBack ( pToken->buffer , '\0' ) ;
                break;
                case    't':
                            stringPushBack ( pToken->buffer , '\t' ) ;
                break;
                case    'r':
                            stringPushBack ( pToken->buffer , '\r' ) ;
                break;
                case    'f':
                            stringPushBack ( pToken->buffer , '\f' ) ;
                break;
                case    'v':
                            stringPushBack ( pToken->buffer , '\v' ) ;
                break;
                case    'a':
                            stringPushBack ( pToken->buffer , '\a' ) ;
                break;
                case    'b':
                            stringPushBack ( pToken->buffer , '\b' ) ;
                break;

                default:
                    temp[0]=c0(pLex);
                    temp[1]=0;
                    $lexerErrorExtraNow( tokening,unexpectedToken,temp ) ;
                break;
            }
            
        }
        
        lexerGetLexemaNext( pLex ) ;
        
        return 1;
    }
    return 0 ;
}

//...................................................................... lexerScanForMBS

int lexerScanForMBS( lexer_t *pLex , token_t* pToken )
{
    if ( c0(pLex)==(uint32_t)'\"' )    //
    {
        lexerGetLexemaNext( pLex ) ;
 
        while( c0(pLex)!=(uint32_t)'\"' )
        {
            if ( lexerIsLastChar(pLex) )  
            {
                $lexerErrorExtra( tokening,missingTerminating,pToken->buffer.data ) ;
                break;
            }        
            
            if ( lexerScanForSpecialChar( pLex , pToken ) ) continue ;
            
            lexerPushC0andGetNext( pLex , pToken ) ;
        }

        pToken = lexerMakeTokenMBS( pLex , pToken ) ;
        
        lexerPushToken( pLex , pToken  ) ;

        // No unget in quanto esce con lexerunGetLexema( pLex );

        return 1 ;
    }
    return 0 ;
}

//...................................................................... lexerMakeTokenMBC

token_t* lexerMakeTokenMBC( lexer_t *pLex , token_t* pToken )
{
    pToken->tPosStart   = pLex->lexPosStart     -1  ;
    pToken->tPosEnd     = pLex->pLexema->ndx    -1  ;
    pToken->type        = eToken_integer;
    pToken->type_num    = eTermType_u32;    
    pToken->ptr         = 0; 
    
    return pToken;
}

//...................................................................... lexerScanForMBC

int lexerScanForMBC( lexer_t *pLex , token_t* pToken )
{
    if ( c0(pLex)==(uint32_t)'\'' ) 
    {
        lexerGetLexemaNext( pLex ) ;
        
        if ( lexerScanForSpecialChar( pLex , pToken ) )
        {

        }
        else
        {
            lexerPushC0andGetNext( pLex , pToken ) ;
        }
        
        if ( c0(pLex)==(uint32_t)'\'' )
        {
            uint32_t numberInteger         = streamConvertMBCtoUTF32(pToken->buffer.data) ;

            pToken->u32                 = numberInteger         ;
            pToken->type                = eToken_integer         ;
            pToken->type_num            = eTermType_u32         ;
            
            lexerPushToken( pLex , pToken  ) ;    
        }
        else
        {
            char temp[2];
            temp[0]=c0(pLex);
            temp[1]=0;
            $lexerErrorExtraNow( tokening,missingTerminating,temp ) ;
        }    
        return 1;
    }
    return 0 ;
}

//...................................................................... lexerScanForInclude

int lexerScanForInclude( lexer_t *pLex , token_t* pToken )
{
    if ( vectorSize( (*pLex->pvToken) ) < 1    ) return 0;
    
    if ( (          vectorBack( (*pLex->pvToken) )->type              != eToken_kw_include    ) 
    ||   ( strcmp ( vectorBack( (*pLex->pvToken) )->mbs , "include" ) != 0                    ) ) return 0 ;

    if ( c0(pLex)==(uint32_t)'<' ) 
    {
        lexerGetLexemaNext( pLex ) ;
 
        while( c0(pLex)!=(uint32_t)'>' )
        {
            if ( lexerIsLastChar(pLex)   )  // ex <stdio.h>
            {
                $lexerErrorExtra( tokening,missingTerminating,pToken->buffer.data ) ;
                break;
            }        

            lexerPushC0andGetNext( pLex , pToken ) ;
        }

        pToken = lexerMakeTokenMBS( pLex , pToken ) ;
        
        lexerPushToken( pLex , pToken  ) ;

        // No unget in quanto esce con lexerunGetLexema( pLex );

        return 1 ;
    }
    return 0 ;
}    

//...................................................................... lexerScanForAsm

int lexerScanForAsm( lexer_t *pLex , token_t* pToken )
{
    if ( vectorSize( (*pLex->pvToken) ) < 1    ) return 0;
    
    if ( (          vectorBack( (*pLex->pvToken) )->type            != eToken_kw_asm    ) 
    ||   ( strcmp ( vectorBack( (*pLex->pvToken) )->mbs , "asm" )   != 0                ) ) return 0 ;

    if ( c0(pLex)==(uint32_t)'{' ) 
    {
        lexerGetLexemaNext( pLex ) ;
 
        while( c0(pLex)!=(uint32_t)'}' )
        {
            if ( lexerIsLastChar(pLex)   )  // ex asm { ... }
            {
                $lexerErrorExtra( tokening,missingTerminating,pToken->buffer.data ) ;
                break;
            }        

            lexerPushC0andGetNext( pLex , pToken ) ;
        }

        pToken = lexerMakeTokenMBS( pLex , pToken ) ;
        
        lexerPushToken( pLex , pToken  ) ;

        // No unget in quanto esce con lexerunGetLexema( pLex );

        return 1 ;
    }
    return 0 ;
}    

//...................................................................... lexerMakeTokenOperator

// keep in order enum and array[]

token_t* lexerMakeTokenOperator( lexer_t *pLex , token_t* pToken , char* pOp , int pos )
{
    pToken->tPosStart   = pLex->lexPosStart     -1  ;
    pToken->tPosEnd     = pLex->pLexema->ndx    -1  ;

    pToken->mbs         = pOp                       ;
    pToken->type        = (eToken_t) (pos);

    pToken->buffer.data    = pToken->mbs;
    pToken->ptr         = 0; 
    
    
    return pToken;
}

//...................................................................... op3[]

char *op3[]    =
{
    //eToken_ellipsis               ,    //    ...    
    "..."    ,     
    //eToken_shift_left_assign      ,    //    <<=    
    "<<="     ,    
    //eToken_shift_right_assign     ,    //    >>=    
    ">>="    ,
    
    NULL 
} ;

//...................................................................... lexerScanForoperator3

int lexerScanForoperator3( lexer_t *pLex , token_t* pToken )
{
    uint32_t ch0 = (uint32_t)c0(pLex);
    uint32_t ch1 = (uint32_t)c1(pLex);
    int         fOperator=0;

    int i=0;
    
    // ## ............................................. operator 3 
    
    do    {
        if ( ( ch0 == (uint32_t)op3[i][0]) && (ch1 == (uint32_t)op3[i][1]) )
        {
            lexerGetLexemaNext( pLex ) ;
            
            if ( c1(pLex) == (uint32_t)op3[i][2] )
            {
                fOperator=3;

                lexerPushToken( pLex , lexerMakeTokenOperator( pLex , pToken , op3[i] , eToken_op3_begin+i+1 ) ) ;

                lexerGetLexemaNext( pLex ) ;
                return fOperator;
            }
            else
            {
                // se <<? e non è <<= controlla per i 2 operatori
                lexerunGetLexema( pLex ) ;
                return fOperator=0;
            }
        }
    } while ( op3[++i] != NULL ) ;

    return fOperator ;
}

//...................................................................... lexerScanForOp2notAllowed

int lexerScanForOp2notAllowed( lexer_t *pLex )
{
    uint32_t ch0 = (uint32_t)c0(pLex);
    uint32_t ch1 = (uint32_t)c1(pLex);
    
    if ( ch0=='=' && ch1=='=' )
    {
        $lexerErrorExtra( tokening,opNotAllowed,"==" ) ;
        lexerGetLexemaNext( pLex ) ;
        return 1 ;
    }
    return 0;
}

//...................................................................... lexerScanForoperator2

char *op2[]    =
{ 
    // eToken_inc                   ,    //    ++    
    "++"     ,    
    // eToken_dec                   ,    //    --    
    "--"    ,    
    // eToken_le                    ,    //    <=    
    "<="     ,    
    // eToken_ge                    ,    //    >=    
    ">="    ,    
    // eToken_assign                ,    //    :=    
    ":="    ,    
    // eToken_ne                    ,    //    !=    
    "!="    ,    
    // eToken_eq                    ,    //    ?=    
    "?="     ,
    // eToken_return                ,    //    ->    
    "->"     ,

    NULL 
} ;

//...................................................................... lexerScanForoperator2

int lexerScanForoperator2( lexer_t *pLex , token_t* pToken )
{
    uint32_t ch0 = (uint32_t)c0(pLex);
    uint32_t ch1 = (uint32_t)c1(pLex);
    int         fOperator=0;

    int i=0;
    
    // ## ............................................. operator 2 
        
    do    {
        if ( (ch0 == (uint32_t)op2[i][0]) && (ch1 == (uint32_t)op2[i][1]) )
        {
                fOperator=2;

                lexerPushToken( pLex , lexerMakeTokenOperator( pLex , pToken , op2[i] ,  eToken_op2_begin+i+1 ) ) ;

                lexerGetLexemaNext( pLex ) ;
                return fOperator;
        }
    } while ( op2[++i] != NULL ) ;

    return fOperator ;
}

int lexerScanForOp1notAllowed( lexer_t *pLex )
{
    uint32_t ch0 = (uint32_t)c0(pLex);
    
    if ( ch0=='=' )
    {
        $lexerErrorExtra( tokening,opNotAllowed,"=" ) ;
        return 1 ;
    }
    return 0;
}

//...................................................................... lexerScanForoperator1

char *op1[]    =
{
    //eToken_p0                 ,    //    (
    "("     ,    
    //eToken_p1                 ,    //    )
    ")"     ,
    //eToken_mul                ,    //    *
    "*"     ,
    //eToken_ptr                ,    //    *
    "^"     ,        
    //eToken_div                ,    //    /
    "/"        ,    
    //eToken_mod                ,    //    %
    "%"     ,
    //eToken_add                ,    //    +
    "+"        ,    
    //eToken_addu               ,    //    +
    "+"        ,    
    //eToken_sub                ,    //    -
    "-"        ,
    //eToken_subu               ,    //    -
    "-"        ,    
    //eToken_lt                 ,    //    <
    "<"     ,    
    //eToken_gt                 ,    //    >
    ">"        ,
    //eToken_not                ,    //    !
    "!"        ,
    //eToken_g0                 ,    //    {
    "{"        ,
    //eToken_g1                 ,    //    }
    "}"        ,    
    //eToken_dp                 ,    //    :
    ":"        ,
    //eToken_and                ,    //    &
    "&"        ,
    //eToken_addr               ,    //    &
    "&"        ,
    //eToken_pv                 ,    //    ;
    ";"        ,
    //eToken_dot                ,    //    .
    "."        ,
    //eToken_q0                 ,    //    [
    "["        ,
    //eToken_q1                 ,    //    ]
    "]"        ,
    NULL 
} ;

//...................................................................... lexerScanForoperator1

int lexerScanForoperator1( lexer_t *pLex , token_t* pToken )
{
    uint32_t ch0 = (uint32_t)c0(pLex);

    int         fOperator=0;

    int i=0;
    
    // ## ............................................. operator 1 
    
    do    {
        if ( ch0 == (uint32_t)op1[i][0] ) 
        {
                fOperator=1;

                lexerPushToken( pLex , lexerMakeTokenOperator( pLex , pToken , op1[i] ,  eToken_op1_begin+i+1 ) ) ;

                return fOperator;
        }
    } while ( op1[++i] != NULL ) ;

    return fOperator ;
}

//...................................................................... lexerMakeVectorTokens

uint32_t lexerMakeVectorTokens( lexer_t *pLex )
{
    pLex->pStream->pSource->data = streamGetSource(pLex->pStream) ;
    
    streamMakeVectorCharMB(pLex->pStream);
 
    if ( pLex->pStream->pCompiler->fDebug ) streamVectorCharMBDebug(pLex->pStream);

    // LEXER MAIN LOOP

    lexerGetLexema( pLex ) ; 

    while( lexerGetLexemaNext( pLex ), !isLexerEOF(pLex) )
    {
        pLex->lexPosStart   =    pLex->pLexema->ndx;    

        token_t* pToken     =    lexerMakeDefaultToken(pLex);
        
        stringClear( (pToken->buffer) ); 

        stringFromS8( (pToken->buffer) ,"\0" ); 
        
        //

        if ( lexerSkipForBackSlashNewLine(pLex)         ) continue ;        //    \ \n

        if ( lexerScanForSingleLineComment(pLex)        ) continue ;        //    // ...
        
        if ( lexerScanForMultiLineComment(pLex)         ) continue ;        //    / * ... * /

        if ( lexerScanForSpace(pLex)                    ) continue ;        //    ' ' \ t
    
        if ( lexerScanForIdentOrKeyword(pLex,pToken)    ) continue ;        //    id kw
    
        if ( lexerScanForIntegerOrReal(pLex,pToken)     ) continue ;        //    int real
            
        if ( lexerScanForMBS(pLex,pToken)               ) continue ;        //    " ... "
        
        if ( lexerScanForMBC(pLex,pToken)               ) continue ;        //    ' . '    ->    int
        
        if ( lexerScanForInclude(pLex,pToken)           ) continue ;        //    # include

        if ( lexerScanForAsm(pLex,pToken)               ) continue ;        //    asm { }
        
        if ( lexerScanForoperator3(pLex,pToken)         ) continue ;        //    <<= ...

        if ( lexerScanForOp2notAllowed(pLex)            ) continue ;        //    ==
        
        if ( lexerScanForoperator2(pLex,pToken)         ) continue ;        //    !=

        if ( lexerScanForOp1notAllowed(pLex)            ) continue ;        //    =
        
        if ( lexerScanForoperator1(pLex,pToken)         ) continue ;        //    +

        // ..... default

        if ( lexerIsLastChar(pLex) )  break ;

        pToken=lexerMakeTokenChar(pLex, pToken , pLex->pLexema->c0 );        // char

        $lexerErrorExtra( tokening,unexpectedToken,pLex->pLexema->c0 ) ;

        pToken->ptr = 0 ;
        
        lexerPushToken(pLex,pToken);

    }

    if ( pLex->pStream->pCompiler->fDebug==true )
    {
        lexerDebugAllTokens( pLex ) ;
    }

    return 0;
}