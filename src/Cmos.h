#ifndef Cmos
#define Cmos

#include "error.h"

//...................................................................... Cmos

#define $PRG  "Cmos"
#define $DATE "(1/2024)"
#define $REV  "alpha 0.0"

//...................................................................... global

#define BUFFER_STRING_16           16
#define BUFFER_STRING_64           64
#define BUFFER_STRING_128         128
#define BUFFER_STRING_256         256
#define BUFFER_STRING_1024       1024
#define BUFFER_STRING_4096       4096

#define SIZEOF_BYTE    1
#define SIZEOF_WORD    2
#define SIZEOF_REAL    5

#define SIZEOF_STACK              256

//...................................................................... fileStream_s

typedef struct fileStream_s
{
    FILE*    ptr;
    char*    name;
} fileStream_t;

//...................................................................... compiler_s

stringTypeDef(char,fileInputName    ) ;
stringTypeDef(char,fileOutputName   ) ;
    
typedef struct symbolTable_s symbolTable_t ; // forward
    
typedef struct compiler_s
{
    int fDebug        ;
    int kHelp         ;
    int cFlags        ;
    int cReturn       ;
    int cTabSize      ;
    int fOptimise     ;
    int stackMaxSize  ;
    string_fileInputName_t fileInputName  ;
    string_fileInputName_t fileOutputName ;
    char*            pNameSpace           ;

    char*            stackAddress ;
    char*            stackSize    ;
    
    symbolTable_t *pScope;
  
} compiler_t ;

compiler_t* compilerNew    (void) ;  
void        compilerDelete (compiler_t* cmos)  ;

// ************
//    CHAR_MB
// ************

typedef enum eCharType_e
{
  charNull         ,
  charAlpha        ,
  charDigit        ,
  charGraph        ,
  charBlank        ,
  charCntrl        ,
  charPunct        ,
  charPrint        ,
  charOther        ,
  
} eCharType_t ;

typedef struct charMB_s
{
    int         type       ;
    char*       mbs        ;
    uint32_t    row        ;
    uint32_t    col        ;
    uint32_t    pos        ;  
    uint8_t     lvl        ;
    uint32_t    c32        ;
    uint8_t     blk        ;
    
} charMB_t;

vectorTypeDef(charMB_t*,charMB);  // vector_ID_t -> vector_charMB_t

stringTypeDef(char,source) ;

typedef struct streamMBChar_s
{
 fileStream_t*       pFileStream         ;
 string_source_t*    pSource             ;
 vector_charMB_t*    pvCharMB            ;
 compiler_t*         pCompiler           ;
 fileStream_t*       pFileDebugCharMB    ;
 
} streamMBChar_t;

streamMBChar_t*     streamNew                      ( compiler_t*      pCompiler ) ;
void                streamDelete                   ( streamMBChar_t* pStream ) ;
fileStream_t *      streamOpen                     ( char *fileName , char *mod ,e_sender_t sender ) ;
char*               streamRemoveBOM                ( streamMBChar_t* pStream ) ;
char*               streamGetSource                ( streamMBChar_t* pStream ) ;
void                streamDelete                   ( streamMBChar_t* pStream ) ;
void                streamMakeVectorCharMB         ( streamMBChar_t* pStream ) ;
int                 streamVectorCharMBDebug        ( streamMBChar_t* pStream ) ;
char*               streamConvertSpecialCharinMBS  ( char* mbs ) ;
uint32_t            streamConvertMBCtoUTF32        ( char mbs[] );

// ************
//    ENUM TOKEN
// ************
    
// keep in order !!

typedef enum eToken_e
{
    eToken_null       ,         //    0
    eToken_mbs        ,         //    1
    eToken_integer    ,         //    2
    eToken_real       ,         //    3
    eToken_id         ,         //    4
    eToken_kw         ,         //    5

    eToken_op_begin_kw=10 ,

        eToken_kw_include       ,        //    11
        eToken_kw_asm           ,        //    12
        eToken_kw_if            ,        //    13
        eToken_kw_else          ,        //    14
        eToken_kw_loop          ,        //    15
        eToken_kw_function      ,        //    16
        eToken_kw_return        ,        //    17
        
        eToken_primitive_type_begin,
        
        eToken_kw_char          ,        //    19
        eToken_kw_byte          ,        //    20
        eToken_kw_int           ,        //    21
        eToken_kw_word          ,        //    22
        eToken_kw_real          ,        //    23

        eToken_primitive_type_end,
        
        eToken_kw_program       ,        //    23
        eToken_kw_end           ,        //    24
        
    eToken_op_end_kw ,
    
    eToken_op_begin,                

        eToken_op3_begin=100,    
        
            eToken_ellipsis             ,    //    ...
            eToken_shift_left_assign    ,    //    <<=
            eToken_shift_right_assign   ,    //    >>=

        eToken_op3_end,
        
        eToken_op2_begin=200,
        
            eToken_inc                   ,    //    ++    201
            eToken_dec                   ,    //    --    202    
            eToken_le                    ,    //    <=    203    
            eToken_ge                    ,    //    >=    204    
            eToken_assign                ,    //    :=    205    
            eToken_ne                    ,    //    !=    206
            eToken_eq                    ,    //    ?=    207
            eToken_return                ,    //    ->    208
            
        eToken_op2_end,
        
        eToken_op1_begin=300,
        
            eToken_p0                    ,    //    (  301
            eToken_p1                    ,    //    )

            eToken_mul                   ,    //    *  303
            eToken_ptr                   ,    //    *
            
            eToken_div                   ,    //    /  305
            eToken_mod                   ,    //    %

            eToken_add                   ,    //    +  307
            eToken_addu                  ,    //    +  308
            eToken_sub                   ,    //    -  309
            eToken_subu                  ,    //    -  310
            
            eToken_lt                    ,    //    <  311
            eToken_gt                    ,    //    >

            eToken_not                   ,    //    !  313

            eToken_g0                    ,    //    { 
            eToken_g1                    ,    //    }  315

            eToken_dp                    ,    //    :
            eToken_and                   ,    //    &  317
            eToken_addr                  ,    //    &

            eToken_pv                    ,    //    ;  319
            eToken_dot                   ,    //    .  320

            eToken_q0                    ,    //    [  321
            eToken_q1                    ,    //    ]  322
            
        eToken_op1_end,
    
    eToken_op_end                        ,
    
    eToken_char                          ,

    eToken_type                          ,    // $tcast    326    ( identifica il nome type del cast )
    
} eToken_t ;

// ***************
//    ENUM Term Type
// ***************

typedef enum eTermType_e
{
    eTermType_null   ,        //    0
    
    // integer 
    
    eTermType_u8     ,        //    1
    eTermType_u16    ,        //    2
    eTermType_u32    ,        //    3
    eTermType_u64    ,        //    4

    eTermType_s8     ,        //    5
    eTermType_s16    ,        //    6
    eTermType_s32    ,        //    7
    eTermType_s64    ,        //    8
    
    // real 
    
    eTermType_r32    ,        //    9
    eTermType_r40    ,        //    10
    eTermType_r64    ,        //    11    
    eTermType_r80    ,        //    12    

    eRecordType      ,        //    13    TODO ?? il record ha dei termini e si fa riferimento a kind

} eTermType_t ;

// ************
//    TOKEN
// ************

stringTypeDef(char,buffer);
            
typedef struct token_s
{
    uint32_t                    row             ;
    uint32_t                    col             ;
    uint32_t                    tPosStart       ;  
    uint32_t                    tPosEnd         ;  
    
    enum eToken_e               type            ;    // ... eToken_integer
    enum eTermType_e            type_num        ;    // ... ... eTermType_u8
    
    char *                      type_suffix     ;
    char *                      f40s            ;
    unsigned char               f40c[5]         ;

    string_buffer_t              buffer         ;
    
    union
    {
        char*           mbs     ;
        
        unsigned char   u8      ;
        uint16_t        u16     ;
        uint32_t        u32     ;
        uint64_t        u64     ;

        int64_t         s64     ;
        
        float           r32     ;
        double          r40     ;
        double          r64     ;
        long double     r80     ;
        
        void*           var     ;
    } ;

    int ptr;    // n.b. kludge per mettere cast* nei prefissi ( usato solo in ast.prefix )
    
} token_t ;
            
vectorTypeDef( token_t* , token ) ;    // vector_vToken_t

// ************
//    LEXEMA
// ************

typedef struct lexema_s
{
    char*       c0      ;
    char*       c1      ;    
    uint32_t    ndx     ;
    uint32_t    char0   ; // current character
    uint32_t    char1   ; // look     ahead
    
} lexema_t;

// ************
//    LEXER
// ************

typedef struct lexer_s
{
    streamMBChar_t*     pStream                 ;
    fileStream_t*       pFileDebugLexer         ;
    lexema_t*           pLexema                 ;
    vector_token_t*     pvToken                 ;
    uint32_t            lexPosStart             ;
    uint32_t            lexPosEnd               ;
 
} lexer_t ;

lexer_t*    lexerNew                    ( streamMBChar_t* pStream ) ;
void        lexerDelete                 ( lexer_t* pLex ) ;
uint32_t    lexerMakeVectorTokens       ( lexer_t *pLex ) ;
char*       lexerGetSourceFromString    ( lexer_t *pLex,char* string ) ;
char*       convertDoubleToMBF          ( double fs ) ;

// ************
//    NODE
// ************

//...................................................................... node term kind

typedef enum eTermKind_e
{
    eTermKind_null    ,        //    0
    eTermKind_var     ,        //    1         a
    eTermKind_ptr     ,        //    2        *a
    eTermKind_const   ,        //    3         1
    eTermKind_arr     ,        //    4         a[]
    eTermKind_fn      ,        //    5         a()
    eTermKind_addr    ,        //    6        &a
    eTermKind_record           //    7         a.
        
} eTermKind_t ;

//...................................................................... node type

typedef struct node_s node_t ;

typedef struct scope_s         scope_t     ; // forward
typedef struct pSymbolDef_s             pSymbol_t        ;

typedef enum nodeType_e
{
    nTypeUndefined      ,   //  0
    nTypeTerm           ,   //  1
    nTypeBinOp          ,   //  2
    nTypePrefix         ,   //  3
    nTypePostfix        ,   //  4
    nTypeASM            ,   //  5
    nTypeBlock          ,   //  6
    nTypeVar            ,   //  7
    nTypeFunction       ,   //  8
    nTypeParamFunction  ,   //  9
    nTypeProgram        ,   // 10
    nTypeDot            ,   // 11
    
} nodeType_t;

//...................................................................... node(s)

// tutte le informazioni relativi al token sono accessibili
// dal nodo radice al livello pNode->pData->(token)
typedef struct nodeTerm_s
{
    token_t*        pTerm       ;
    pSymbol_t*      pSymbol     ;
} nodeTerm_t;    

typedef struct nodePrefix_s
{
    node_t*        pLeft        ;
} nodePrefix_t;

typedef struct nodeBinOp_s
{
    node_t*        pLeft        ;
    node_t*        pRight       ;
} nodeBinOp_t;

vectorTypeDef( node_t* , node ) ;    // vector_node_t

typedef struct nodeBlock_s
{
    vector_node_t*    pvNode    ;
    scope_t*         bScope     ;    // local symbol table
    
} nodeBlock_t;

typedef struct nodeASM_s
{
    token_t*    pASM        ;
} nodeASM_t;

vectorTypeDef( char* , stringFunction );

typedef struct nodeFunction_s
{
    char*                       fName               ;
    node_t*                     fBlock              ;
    scope_t*                    fScope              ;
    char*                       retType             ;
    vector_stringFunction_t*    pvStringFunction    ;
    vector_stringFunction_t*    pvWeakFunction      ;
    int                         stackSize           ;
} nodeFunction_t;

typedef struct nodeParamFunction_s
{
    char*            fName      ;
    node_t*          fBlock     ;
    char*            retType    ;
} nodeParamFunction_t;

vectorTypeDef( char* , stringGlobal );
    
typedef struct nodeProgram_s
{
    node_t*                    body             ;
    vector_stringGlobal_t*     pvStringGlobal   ;
    
} nodeProgram_t;


typedef struct nodeDot_s
{
    node_t*     pLeft   ;
    char*       member  ;
} nodeDot_t;

//...................................................................... node

typedef struct node_s
{
    nodeType_t             nType        ;
    eTermType_t            baseType     ;
    eTermKind_t            baseKind     ;
    int                    baseSize     ;
    union 
    {
        nodeTerm_t*             pnTerm          ;    // integer real string 
        nodeBinOp_t*            pnBinOp         ;    // pOp LHS/RHS   
        nodePrefix_t*           pnPrefix        ;    // unary prefix
        nodeASM_t*              pnASM           ;    // assembler Code
        nodeBlock_t*            pnBlock         ;    // BEGIN                // code gen
        //nodeIfThenElse_t*     pnIfThenElse    ;    // if then else
        //nodeLoop_t*           pnLoop          ;    // node loop 
        nodeFunction_t*         pnFunction      ;    // function f1 ()
        nodeParamFunction_t*    pnParamFunction ;    // f1 ( 1,2,3 )
        nodeProgram_t*          pnProgram       ;    // main node 
        nodeDot_t*              pnDot           ;    // struct
    } ;

    token_t* pData ;            // info generali del nodo (row,col,buffer)


} node_t ;



// ************
//    PARSER
// ************


// ...............................................................    eScope_t

typedef enum eScope_e
{
    eScope_null     ,        //    0
    eScope_local    ,        //    1
    eScope_param    ,        //    2
    eScope_global   ,        //    3
    
} eScope_t ;

typedef struct parser_s
{
    lexer_t*            pLex                ;
    uint32_t            lexPosStart         ;
    uint32_t            lexPosEnd           ;
    fileStream_t*       pFileDebugParser    ;
    fileStream_t*       pFileDebugNode      ;
    token_t*            p0                  ;
    token_t*            p1                  ;
    uint32_t            ndx                 ;
    token_t*            pLastToken          ;
    node_t*             pAST                ;

} parser_t ;

// parser.c

parser_t*   parserNew           ( lexer_t*  pLex ) ;
void        parserDelete        ( parser_t* pPar ) ;
token_t*    parserGetToken      ( parser_t* pPar ) ;
token_t*    parserNextGetToken  ( parser_t* pPar ) ;
int         parserMatchToken    ( parser_t* pPar , eToken_t tokenType , const char *tok ) ;
node_t*     parseFunction       ( parser_t* pPar , node_t* pNode , scope_t* parentScope , eScope_t prevScope )  ;
node_t*     parseDeclaration    ( parser_t* pPar , node_t* pNode , eScope_t scope , scope_t* parentScope )  ;
node_t*     parseBlockBeginEnd  ( parser_t* pPar , scope_t* parentScope )  ;
node_t*     parseProgram        ( parser_t* pPar ) ;
node_t*     parserRun           ( parser_t* pPar ) ;

// expr.c

node_t*        parseTerm        ( parser_t *pPar , node_t *pNode , scope_t* pScope ) ;
node_t*        parsePrefix      ( parser_t *pPar , node_t *pNode , scope_t* pScope ) ;
node_t*        parseAssign      ( parser_t *pPar , node_t *pNode , scope_t* pScope ) ;
node_t*        parseExpr        ( parser_t *pPar , node_t *pNode , scope_t* pScope ) ;


// ************
//    AST
// ************

node_t*        astMakeNodeTerm          ( parser_t *pPar , token_t *pToken ) ;
node_t*        astMakeNodeBlock         ( parser_t *pPar , token_t *pToken ) ;
node_t*        astMakeNodeASM           ( parser_t *pPar , token_t *pToken ) ;
node_t*        astMakeNodePrefix        ( parser_t *pPar , token_t *pToken , node_t* pLeft ) ;
node_t*        astMakeNodeBinOp         ( parser_t *pPar , token_t *pToken , node_t *left , node_t *right  , scope_t* pScope )  ;
node_t*        astMakeNodeFunction      ( parser_t *pPar , token_t *pToken ) ;
node_t*        astMakeProgram           ( parser_t *pPar , token_t *pToken ) ;
node_t*        astMakeNodeNDX           ( parser_t *pPar , token_t *pToken , node_t* pNode , node_t *left , node_t *right  , scope_t* pScope ) ;
node_t*        astMakeNodeParamFunction ( parser_t *pPar , token_t *pToken ) ;
node_t*        astMakeNodePrefix        ( parser_t *pPar , token_t *pToken , node_t* pLeft ) ;
token_t*       astNewTokenCopyFrom      ( parser_t *pPar , token_t *pCopyToken ) ;
node_t*        astMakeNodeDot           ( parser_t *pPar , token_t *pToken , node_t *left , node_t *right  , scope_t* pScope , char* member ) ;

// ************
//    scanner
// ************

node_t*        ast( parser_t *pPar ,node_t* pNode , scope_t* pScope ) ;

// ******************
// Symbol Table
// ******************

eTermType_t stGetReduceTypeFromTermType( token_t* pToken ) ;

// ...............................................................    pType_t

// informazioni sul tipo 

typedef struct pType_s
{
    char*           id      ;    //    int        real           point
    eTermKind_t     kind    ;    //    var        var            record
    eTermType_t     type    ;    //    u16        f40            record
    uint8_t         size    ;    //    $(u16)     $(f40)        $(record)
    
    char*           aRecordField_name[16];
    eTermType_t     aRecordField_type[16];
    uint8_t         aRecordField_offset[16];
    int             kRecordField ;
    int             offset  ;
    
    eScope_t        scope;
    
} pType_t ;

vectorTypeDef( pType_t* , pTypeDef ) ;    // vector_node_t

// ...............................................................    pSymbol_t

// informazioni sul simbolo (id)
        
typedef struct pSymbolDef_s 
{
    char*               id          ; // ID name
    eTermKind_t         kind        ; // genere     :   null,var,ptr,const,arr,fn,addr
    eTermType_t         type        ; // tipo       :   null,byte,char,int,word,real,(string=*byte,byte[]),record
    eScope_t            scope       ; // scope      :   null,local,param,global  
    int                 offset      ; // stack      :   0    ,  offset
    uint8_t             size        ; // ID         :   0    ,  size
    scope_t*            pScope;

    // array lda (array),x    al momento 256 elelementi essendo HLA
    
    pType_t*            pRecord     ; // punta alla struttura di appartenenza

} pSymbol_t ;

vectorTypeDef( pSymbol_t* , pSymbolDef ) ; // vector_pSymbolDef_t

// ...............................................................    scope_t

typedef struct scope_s 
{
    scope_t*                prev            ;    // parent
    vector_pSymbolDef_t*    pSymbolDef      ;    // ID    ;    int *ptr     ,    point arr[]
    vector_pTypeDef_t*      pTypeDef        ;    // KW    ;    [int]        ,    [point]
    
} scope_t;

// ...............................................................    symbolTable_s

extern scope_t*            gScope;        // scope attuale program/function/block

pType_t*                stNewType               (char* name,eTermKind_t    kind,eTermType_t    type,uint8_t size,eScope_t scope );
vector_pTypeDef_t*      stInitDefaultType        (void);
scope_t*                stNewSymbolTypeDef       (scope_t* stScope ) ;
pSymbol_t*              stNewDefaultSymbol       (void);
pSymbol_t*              stNewSymbol              (char *id, eTermKind_t kind,eTermType_t type,eScope_t scope);
scope_t*                stNewScope               (void);
int                     stIsPrimitiveType        (eToken_t t);
eTermType_t             stGetTypeFromName        (char *name) ;
char*                   stGetCastNameFromType    (eTermType_t t ) ;
uint8_t                 stGetSizeFromType        (eTermType_t t) ;
int                     stAddNewSymbol           (scope_t* pScope , pSymbol_t* pNewSymbol ) ;
pType_t*                stFind_pTypeDef          (scope_t* last     , char *typeID ) ;
pSymbol_t*              stFind_pSymbolDef        (scope_t* pScope, char *symbolName ) ;
eTermType_t             stPromote                (eTermType_t lhs,eTermType_t rhs);
char*                   stGet64TassNameFromType  (eTermType_t t ) ;
pSymbol_t*              stFind_pSymbolDefPos     (scope_t* pScope , char *symbolName ,  size_t pos );
int                     stIsInteger              (eTermType_t type ) ;

// ************
//    assembler
// ************
 
vectorTypeDef( char* , LocalAsmReal     );
vectorTypeDef( char* , LocalAsmString     );
vectorTypeDef( char* , GlobalAsmDef     );

typedef struct assembler_s
{
    parser_t*                   pPar                ;
    fileStream_t*               pFileASM            ;
    
    uint32_t                    kLocalAsmReal       ;
    vector_LocalAsmReal_t*      pvLocalAsmReal      ;
    
    uint32_t                    kLocalAsmString     ;
    vector_LocalAsmString_t*    pvLocalAsmString    ;
    
} assembler_t ;

assembler_t*    assemblerNew    ( parser_t *pPar ) ;
void            assemblerDelete ( assembler_t *pASM ) ;
node_t*         assemble        ( assembler_t *pASM ,node_t* pNode , scope_t* pScope , int left_right ) ;
node_t*         assembleGO      ( assembler_t *pASM ) ;

// DELETEME extern vector_pTypeDef_t* gvTypeDef ;

//
#endif

