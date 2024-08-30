
#include "../lib/cxx.h"
#include "Cmos.h"

// ******************
// ASSEMBLER
// ******************

assembler_t* assemblerNew(parser_t *pPar)
{
    assembler_t*    pASM=(assembler_t*)new(assembler_t);

    pASM->pPar                = pPar;
    
    pASM->pFileASM = streamOpen( pASM->pPar->pLex->pStream->pCompiler->fileInputName.data, "w+",sender_asm ) ;

    pASM->pvLocalAsmReal = ( vector_LocalAsmReal_t* ) new ( vector_LocalAsmReal_t ) ;
    vectorAlloc( (*pASM->pvLocalAsmReal) ,8 ) ;

    pASM->kLocalAsmReal    =0;

    pASM->pvLocalAsmString = ( vector_LocalAsmString_t* )new ( vector_LocalAsmString_t ) ;
    vectorAlloc( (*pASM->pvLocalAsmString) ,8 ) ;
    
    pASM->kLocalAsmString=0;

    return pASM ;
}

void assemblerDelete(assembler_t *pASM)
{
    assert(pASM!=NULL);
    assert(pASM->pFileASM->ptr!=NULL);

    pASM->kLocalAsmString=0;
    
    vectorDealloc( (*pASM->pvLocalAsmString) ) ;
    gcFree (pASM->pvLocalAsmString) ;

    pASM->kLocalAsmReal    = 0;

    vectorDealloc( (*pASM->pvLocalAsmReal) ) ;
    gcFree (pASM->pvLocalAsmReal) ;

    gcFileClose(pASM->pFileASM->ptr); 

    delete(pASM);
}

// ******************
//     assembler Debug
// ******************

int assemblerInternalError( char*s , int t)
{
    fprintf ( stdout , "\n!! internal error : assemble ," ) ;
    fprintf ( stdout , " %s ",s ) ;
    fprintf ( stdout , " %d ",t ) ;
    fprintf ( stdout , " -> not recognize\n" ) ;
    exit(-1);
}

#define $ASMdefault(S,T)\
    default:\
    fprintf ( stdout , "\n?? internal error : assemble ," ) ;\
    fprintf ( stdout , " %s ",S ) ;\
    fprintf ( stdout , " %d ",T ) ;\
    fprintf ( stdout , " -> not recognize\n" ) ;\
    exit(-1);\
    break;

void assemblerTab( assembler_t *pASM,int level)
{
    fprintf ( pASM->pFileASM->ptr , "\n"  );
    for (int i=0;i<level;i++) fprintf ( pASM->pFileASM->ptr , "    "  ); // tab 8 
}

#define $ASMdebug(FORMAT,...)\
do{\
    if (pASM->pPar->pLex->pStream->pCompiler->fDebug)\
    {   assemblerTab(pASM,level);\
        fprintf(pASM->pFileASM->ptr,FORMAT,__VA_ARGS__);\
    }\
}while(0);

/* 
// FULL VERSIONE
#define $ASMdebugNode(FORMAT,...)\
do{\
    if (pNode)\
        if (pASM->pPar->pLex->pStream->pCompiler->fDebug)\
        {   assemblerTab(pASM,level);\
            fprintf(pASM->pFileASM->ptr,"; ""RC[%3d/%3d] B[%3d]K[%3d] "\
                    ,pNode->pData->row,pNode->pData->col,pNode->baseKind,pNode->baseType);\
            fprintf(pASM->pFileASM->ptr,FORMAT,__VA_ARGS__);\
        }\
}while(0);
*/
// COMPACT VERSION
#define $ASMdebugNode(FORMAT,...)\
do{\
    if (pNode)\
        if (pASM->pPar->pLex->pStream->pCompiler->fDebug)\
        {   assemblerTab(pASM,level);\
            fprintf(pASM->pFileASM->ptr,"; ""RC[%3d/%3d] "\
                    ,pNode->pData->row,pNode->pData->col);\
            fprintf(pASM->pFileASM->ptr,FORMAT,__VA_ARGS__);\
        }\
}while(0);

#define $ASMnl    fprintf(pASM->pFileASM->ptr,"\n");

#define $ASMdebugNodeRC \
do{\
    if (pNode)\
        if (pASM->pPar->pLex->pStream->pCompiler->fDebug)\
        {   assemblerTab(pASM,level);\
            fprintf(pASM->pFileASM->ptr,"; RC[%3d/%3d]",pNode->pData->row,pNode->pData->col);\
        }\
}while(0);

// ******************
// Code Generator
// ******************

// .............................................................................................. cdPreamble

void cdPreamble( assembler_t *pASM )
{
    fprintf ( pASM->pFileASM->ptr , "\n");
    //fprintf ( pASM->pFileASM->ptr , "\n*       = $0801"); // PROGRAM_ADDRESS
    fprintf ( pASM->pFileASM->ptr , "\n*       = PROGRAM_ADDRESS"); // PROGRAM_ADDRESS
    fprintf ( pASM->pFileASM->ptr , "\n        .word (+), 2024  ;pointer, line number");
    fprintf ( pASM->pFileASM->ptr , "\n        .null $9e, format(\"%%4d\", start)"); 
    fprintf ( pASM->pFileASM->ptr , "\n+       .word 0          ;basic line end");
    fprintf ( pASM->pFileASM->ptr , "\n");
    //fprintf ( pASM->pFileASM->ptr , "\n*       = $080d");
    fprintf ( pASM->pFileASM->ptr , "\n*       = PROGRAM_ADDRESS+12");
    fprintf ( pASM->pFileASM->ptr , "\n        start");
    fprintf ( pASM->pFileASM->ptr , "\n            jmp %s.main",pASM->pPar->pLex->pStream->pCompiler->pNameSpace);
    fprintf ( pASM->pFileASM->ptr , "\n");
    fprintf ( pASM->pFileASM->ptr , "\nfnStackSize .var 0");
    fprintf ( pASM->pFileASM->ptr , "\n");
    fprintf ( pASM->pFileASM->ptr , "\n%s",".include \"../nC64Lib/lib/libCD.asm\"");
    
    fprintf ( pASM->pFileASM->ptr , "\n");
    fprintf ( pASM->pFileASM->ptr , "\nPROGRAM_STACK_ADDRESS    =    $%s",pASM->pPar->pLex->pStream->pCompiler->stackAddress ) ;
    fprintf ( pASM->pFileASM->ptr , "\nPROGRAM_STACK_SIZE       =    $%s",pASM->pPar->pLex->pStream->pCompiler->stackSize    ) ;
    fprintf ( pASM->pFileASM->ptr , "\n");
    
    fprintf ( pASM->pFileASM->ptr , "\n%s",".include \"../nC64Lib/lib/libC64.asm\"");
    fprintf ( pASM->pFileASM->ptr , "\n");
}
        
// .............................................................................................. cgEpilogue

void cgEpilogue( assembler_t *pASM )
{
    fprintf ( pASM->pFileASM->ptr , "\n");
}

// .............................................................................................. cgBlockBegin

static int cd=0; // first block in program .proc 

void cgBlockBegin( assembler_t *pASM,int level )
{
    if (cd==0)
    {
        assemblerTab( pASM, level) ;
        fprintf ( pASM->pFileASM->ptr ,    "%s .namespace",pASM->pPar->pLex->pStream->pCompiler->pNameSpace);
    }
    else
    {
        assemblerTab( pASM, level) ;
        fprintf ( pASM->pFileASM->ptr ,    ".block"    );
        fprintf ( pASM->pFileASM->ptr , "\n");
    }
    ++cd;
}

// .............................................................................................. cgBlockEnd

void cgBlockEnd( assembler_t *pASM,int level )
{
    --cd ;    
    if (cd==0)
    {
        assemblerTab( pASM, level) ;
        fprintf ( pASM->pFileASM->ptr ,    ".endnamespace"    );
        fprintf ( pASM->pFileASM->ptr , "\n");
    }    
    else
    {
        fprintf ( pASM->pFileASM->ptr , "\n");
        assemblerTab( pASM, level ) ;
        fprintf ( pASM->pFileASM->ptr ,    ".endblock"    );
    }

}

// .............................................................................................. cgFunctionTop

void cgFunctionTop( assembler_t *pASM,int level ,char *fName )
{
    fprintf ( pASM->pFileASM->ptr ,"\n"        );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"; .........................................    %s",fName );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"%s .proc",fName );
}

// .............................................................................................. cgFunctionBottom

void cgFunctionBottom( assembler_t *pASM,int level )
{
    fprintf ( pASM->pFileASM->ptr ,"\n"         );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"rts"        );
    fprintf ( pASM->pFileASM->ptr ,"\n"         );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,".endproc"   );
}

// .............................................................................................. cgAssembler

void cgAssembler( assembler_t *pASM,int level, char *assemblerRaw )
{
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"%s",assemblerRaw );
}

// .............................................................................................. cgStackAlloc

void cgStackAlloc( assembler_t *pASM , int stackSize, int level )
{
    if ( stackSize != 0 )
    {
        fprintf ( pASM->pFileASM->ptr ,"\n");
        assemblerTab( pASM, level ) ;
        fprintf ( pASM->pFileASM->ptr ,"stack_begin");
    }    
}

// .............................................................................................. cgStackDeAlloc

void cgStackDeAlloc( assembler_t *pASM , int stackSize , int level )
{
    if ( stackSize != 0 )
    {
        // non server deallocare lo stack in quanto, avviene il ripristino alla posizione
        // dello stack precedente con stack_end
        fprintf ( pASM->pFileASM->ptr ,"\n");
        assemblerTab( pASM, level ) ;
        fprintf ( pASM->pFileASM->ptr ,"fnStackSize .var fnStackSize - size(localStack)");
        assemblerTab( pASM, level ) ;
        fprintf ( pASM->pFileASM->ptr ,".cerror(fnStackSize<0)");
        assemblerTab( pASM, level ) ;
        fprintf ( pASM->pFileASM->ptr ,"stack_end");
    }
}

// .............................................................................................. cgEmitSymbolGlobal

void cgEmitSymbolGlobal( assembler_t *pASM, node_t *pNode)
{
    fprintf ( pASM->pFileASM->ptr ,"\n"        );
    for(size_t i=0;i<pNode->pnProgram->pvStringGlobal->size;i++)
    {
        assemblerTab( pASM, 0 ) ;
        fprintf ( pASM->pFileASM->ptr ,"%s", pNode->pnProgram->pvStringGlobal->data[i]);
    }
    fprintf ( pASM->pFileASM->ptr ,"\n"        );
}

// .............................................................................................. cgEmitSymbolLocal

void cgEmitSymbolLocal( assembler_t *pASM, node_t *pNode, int level)
{
    fprintf ( pASM->pFileASM->ptr ,"\n"        );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"localStack .struct"        ) ;

    for(size_t i=0;i<pNode->pnFunction->pvStringFunction->size;i++)
    { 
        assemblerTab( pASM, level ) ;
        fprintf ( pASM->pFileASM->ptr ,"%s", pNode->pnFunction->pvStringFunction->data[i]);
    }
    
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,".endstruct" ) ;

    fprintf ( pASM->pFileASM->ptr ,"\n"         );
    
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,".weak"      ) ;
    
    for(size_t i=0;i<pNode->pnFunction->pvWeakFunction->size;i++)
    { 
        assemblerTab( pASM, level ) ;
        fprintf ( pASM->pFileASM->ptr ,"%s", pNode->pnFunction->pvWeakFunction->data[i]);
    }
    
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,".endweak"  ) ;
    
    fprintf ( pASM->pFileASM->ptr ,"\n"        );

    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"stack_alloc localStack");
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"fnStackSize .var fnStackSize + size(localStack)");
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,".cerror(fnStackSize>stack.size)");
    fprintf ( pASM->pFileASM->ptr ,"\n"        );
    
}

// .............................................................................................. cgEmitValueConstInteger

void cgEmitValueConstInteger( assembler_t *pASM, node_t* pNode , int level )
{
    switch( pNode->pData->type_num ) 
    {
        case eTermType_u8     :
        case eTermType_s8     :
                            assemblerTab( pASM, level ) ;
                            fprintf ( pASM->pFileASM->ptr , "lda #%02d",(unsigned int)pNode->pData->u64) ;
        break;
        case eTermType_u16    :
        case eTermType_u32    :
        case eTermType_u64    :
                            assemblerTab( pASM, level ) ;
                            fprintf ( pASM->pFileASM->ptr , "lday #%02d",(unsigned int)pNode->pData->u64) ;
        break;
        case eTermType_s16    :
        case eTermType_s32    :
        case eTermType_s64    :
                            assemblerTab( pASM, level ) ;
                            fprintf ( pASM->pFileASM->ptr , "lday #%02d",(unsigned int)pNode->pData->s64) ;
        break;
        $ASMdefault("codegegen.c -> cgEmitValueConstInteger -> switch( pNode->pData->type_num )",pNode->pnTerm->pTerm->type );
    }
}

// .............................................................................................. codeGenWriteAsmConst

void codeGenWriteAsmConst( assembler_t *pASM )
{
    fprintf ( pASM->pFileASM->ptr , "\n");
    // ........................................................................ real 
    for(size_t i=0;i<vectorSize((*pASM->pvLocalAsmReal));i++)
    {
        fprintf ( pASM->pFileASM->ptr , "\n\t%s",pASM->pvLocalAsmReal->data[i] ) ;
    }

    // ........................................................................ string 
    for(size_t i=0;i<vectorSize((*pASM->pvLocalAsmString));i++)
    {
        fprintf ( pASM->pFileASM->ptr , "\n\t%s",pASM->pvLocalAsmString->data[i] ) ;
    }

    fprintf ( pASM->pFileASM->ptr , "\n");    
}

// .............................................................................................. cgEmitValueConstReal

void cgEmitValueConstReal( assembler_t *pASM, node_t* pNode , int level )
{
    switch( pNode->pData->type_num ) 
    {
        case eTermType_r32    :
        case eTermType_r40    :
        case eTermType_r64    :    
        case eTermType_r80    :
        {    // crea costanti reali in f40 da mettere a fine file
            char idReal[BUFFER_STRING_128]; 
            snprintf( idReal , BUFFER_STRING_128 ,"kLocalAsmReal%02d" ,pASM->kLocalAsmReal++ ) ;

            char localAsmReal[BUFFER_STRING_1024];
            snprintf( localAsmReal , BUFFER_STRING_1024 , "%-20s %20s %20s ; %9.9g", idReal , ".byte" ,pNode->pData->f40s,(float)pNode->pData->r80 );

            vectorPushBack( (*pASM->pvLocalAsmReal), gcStrDup(localAsmReal) ) ;

            assemblerTab( pASM, level ) ;        
            fprintf ( pASM->pFileASM->ptr , "%-18s %s","ldfac1",idReal      );
        }
        break;
        $ASMdefault("codegegen.c -> cgEmitValueConstInteger -> switch( pNode->pData->type_num )",pNode->pnTerm->pTerm->type );
    }
}

// .............................................................................................. cgEmitAddressString

void cgEmitAddressString( assembler_t *pASM, node_t* pNode , int level )
{
    // crea costanti stringa indirizzi da mettere a fine file
 
    char idString[BUFFER_STRING_1024];
    snprintf( idString , BUFFER_STRING_1024 ,"kLocalAsmString%02d", pASM->kLocalAsmString++);
 
    char LocalAsmString[BUFFER_STRING_4096];
    snprintf( LocalAsmString , BUFFER_STRING_4096 ,"%-20s %20s \"%s\"", idString,".text",pNode->pData->mbs);
    
    vectorPushBack( (*pASM->pvLocalAsmString ), gcStrDup(LocalAsmString) ) ;    

    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr , "%-18s %s","load_address_ay",idString  ); 
}

// .............................................................................................. cgLoad_ID_Address

void cgLoad_ID_Address( assembler_t *pASM , node_t* pNode , int level    )
{
    assemblerTab( pASM, level ) ;    

    if (  pNode->pnTerm->pSymbol==NULL  )
    {
        $asmErrorExtraLast( coding,undeclared,pNode->pData->mbs ) ;
    }
    else
    {
        if (  pNode->pnTerm->pSymbol->scope == eScope_global )
        {
            // GLOBAL
            fprintf ( pASM->pFileASM->ptr , "load_address_ay %s"    ,pNode->pData->mbs ) ;
        }        
        else 
        {
            // LOCAL
            fprintf ( pASM->pFileASM->ptr , "stack_load_address %s",pNode->pData->mbs ) ;
        }
    }
}

// .............................................................................................. cgLoad_ID_value

void cgLoad_ID_Value( assembler_t *pASM , node_t* pNode , int level    )
{
   if ( pNode->pnTerm->pSymbol != NULL )
   {
        if (  pNode->pnTerm->pSymbol->scope == eScope_global ) // ## GLOBAL
        {
            assemblerTab( pASM, level ) ;
            if((pNode->pnTerm->pSymbol->type==eTermType_u8)||(pNode->pnTerm->pSymbol->type==eTermType_s8))      // BYTE
            {
                fprintf ( pASM->pFileASM->ptr , "lda %s",pNode->pData->mbs) ;
            }
            else 
            if((pNode->pnTerm->pSymbol->type==eTermType_u16)||(pNode->pnTerm->pSymbol->type==eTermType_s16))    // WORD
            {
                fprintf ( pASM->pFileASM->ptr , "lda %s+0",pNode->pData->mbs ) ;
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr , "ldy %s+1",pNode->pData->mbs ) ;
            } 
            else 
            if((pNode->pnTerm->pSymbol->type==eTermType_r40))
            {
                fprintf ( pASM->pFileASM->ptr , "load_address_ay %s",pNode->pData->mbs ) ;                      // REAL
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr , "jsr basic.load5_fac1");
            }
            else
            if((pNode->pnTerm->pSymbol->type==eRecordType))
            {
                //puts("eRecordType struct found");
            }
            else { printf("!! INTERNAL GLOBAL : cgLoad_ID_Value][%d]",pNode->pnTerm->pSymbol->type); exit(-1); }
        }        
        else                                                      // ## LOCAL
        {
            assemblerTab( pASM, level ) ;
            fprintf ( pASM->pFileASM->ptr , "stack_load_address %s",pNode->pData->mbs ) ;
            
            
            if (  pNode->pnTerm->pSymbol->kind == eTermKind_ptr )
            {
                //assemblerTab( pASM, level ) ;
                //fprintf ( pASM->pFileASM->ptr , "stack_load_word") ;                                            
            }
            else 
            if((pNode->pnTerm->pSymbol->type==eTermType_u8)||(pNode->pnTerm->pSymbol->type==eTermType_s8))
            {
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr , "stack_load_byte") ;                                            // BYTE
            }
            else 
            if((pNode->pnTerm->pSymbol->type==eTermType_u16)||(pNode->pnTerm->pSymbol->type==eTermType_s16))
            {
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr , "stack_load_word") ;                                            // WORD
            } 
            else
            if((pNode->pnTerm->pSymbol->type==eTermType_r40))    
            {
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr , "jsr basic.load5_fac1");                                        // REAL
            }
            else
            if((pNode->pnTerm->pSymbol->type==eRecordType))
            {
                //puts("eRecordType struct found");
            }
            else { printf("!! INTERNAL LOCAL : cgLoad_ID_Value][%d]",pNode->pnTerm->pSymbol->type); exit(-1); }
        }
   }
}

//.................................................. CAST

eTermType_t cgCastToBaseType( assembler_t *pASM  , eTermType_t sourceType , eTermType_t destType , int level ,int ptr , int ROW,int COL)
{
    (void)ptr;
    (void)ROW;
    (void)COL;
    
    char castOutput[BUFFER_STRING_256];
    
    strcpy(castOutput,"cast_from_");

    if ( sourceType == destType) return destType;
    
    if ( sourceType == eTermType_u8     ) strcat( castOutput , "u8_to" ) ;
	
    else if ( sourceType == eTermType_u16     ) strcat( castOutput , "u16_to"     ) ;
    else if ( sourceType == eTermType_s8      ) strcat( castOutput , "s8_to"      ) ;
    else if ( sourceType == eTermType_s16     ) strcat( castOutput , "s16_to"     ) ;
    else if ( sourceType == eTermType_r32     ) strcat( castOutput , "real_to"    ) ;
    else if ( sourceType == eTermType_r40     ) strcat( castOutput , "real_to"    ) ;
    else if ( sourceType == eTermType_r64     ) strcat( castOutput , "real_to"    ) ;
    else if ( sourceType == eTermType_r80     ) strcat( castOutput , "real_to"    ) ;
    
    //else { puts(" cast source NULL ??");  } //  TODO CHECK ELSE
/*    
    if ( ptr ) 
    {
        $asmWarningExtraLast        ( coding,ptrArithmetic,"$cast *",ROW,COL)    ; 
        strcat( castOutput , "_u16"        ) ;
        assemblerTab( pASM, level ) ;
        fprintf( pASM->pFileASM->ptr , "%s",castOutput);

        return destType ;
    }
*/
    if ( destType     == eTermType_u8     ) strcat( castOutput , "_u8" ) ;
	
    else if ( destType     == eTermType_u16     ) strcat( castOutput , "_u16"       ) ;
    else if ( destType     == eTermType_s8      ) strcat( castOutput , "_s8"        ) ;
    else if ( destType     == eTermType_s16     ) strcat( castOutput , "_s16"       ) ;
    else if ( destType     == eTermType_r32     ) strcat( castOutput , "_real"      ) ;
    else if ( destType     == eTermType_r40     ) strcat( castOutput , "_real"      ) ;
    else if ( destType     == eTermType_r64     ) strcat( castOutput , "_real"      ) ;
    else if ( destType     == eTermType_r80     ) strcat( castOutput , "_real"      ) ;
    else { puts(" cast dest NULL ??");  }
    
    assemblerTab( pASM, level ) ;
    fprintf( pASM->pFileASM->ptr , "%s",castOutput);
    
    return destType ;
}

//.................................................. cgnTypeTermFunctionPushBaseType

void  cgnTypeTermFunctionPushBaseType( assembler_t *pASM, eTermType_t baseType , int level )
{
    // Qui non c'è conteggio dello stack in quanto
    // l'indirizzo locale dello stack viene calcolato nella funzione
    // allocandolo, per tanto con .param .endparam viene creato uno psuedo stack
    // solo per inserire i parametri e riportarlo quindi al punto di partenza
    
    assemblerTab( pASM, level ) ;
    
    switch( baseType )
    {
        case eTermType_u8     :
        case eTermType_s8     :    
                            fprintf ( pASM->pFileASM->ptr , "jsr stack.push.byte" );
        break;
        case eTermType_u16    :
        case eTermType_u32    :
        case eTermType_u64    :
        case eTermType_s16    :
        case eTermType_s32    :
        case eTermType_s64    :
                            fprintf ( pASM->pFileASM->ptr , "jsr stack.push.word" );
        break;
        case eTermType_r32    :
        case eTermType_r40    :
        case eTermType_r64    :    
        case eTermType_r80    :    
                            fprintf ( pASM->pFileASM->ptr , "jsr stack.push.real" );
        break;
        $ASMdefault("switch( cgnTypeTermFunctionPushBaseType )",baseType);
    }
}

//.................................................. stack

#define emitValue    0
#define emitAddr    1
#define emitMax        SIZEOF_STACK
static int      aStackAddrValue[emitMax];
static int      kaStackAddrValue=0;

// Dimensione di partenza dello stack locale delle funzione, questo andrà incrementato con push e pop

static int      fn_stackSize = 0 ; 

//.................................................. c64 hardware stack

void cgPushAtoC64Stack( assembler_t *pASM , int level )
{
    (void)level;
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"pha" );
}
void cgPopAfromC64Stack( assembler_t *pASM , int level )
{
    (void)level;
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"pla" );
}

void cgPushAYtoC64Stack( assembler_t *pASM , int level )
{
    (void)level;
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"pha" );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"tya" );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"pha" );
}
void cgPopAYfromC64Stack( assembler_t *pASM , int level )
{
    assemblerTab( pASM, level ) ;    
    fprintf ( pASM->pFileASM->ptr ,"pla" );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"tay" );
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"pla" );
}
void cgPushFAC1fromC64Stack( assembler_t *pASM , int level )
{
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"jsr stack.push.real_c64" );
}
void cgPopFAC1fromC64Stack( assembler_t *pASM , int level )
{
    assemblerTab( pASM, level ) ;
    fprintf ( pASM->pFileASM->ptr ,"jsr stack.pop.real_c64" );
}

//.................................................. cgPushValueType

void cgPushValueType( assembler_t *pASM   , node_t* pNode , int* kStack , int level , eToken_t tToken )
{
    // per := è consentito una sola assegnazione
    // diversamente + - * ci sono multiple opBinari
    eTermType_t cast = pNode->baseType ;
    
    if ( pNode->baseKind == eTermKind_addr )
    {
        cast = eTermType_u16;
    }
    if ( pNode->baseKind == eTermKind_ptr )
    {
        cast = eTermType_u16;
    }
	
    switch(cast) 
    {
        case eTermType_u8    :
        case eTermType_s8    :
            if ( tToken == eToken_assign )
            {
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr ,"jsr stack.push.byte" );
                *kStack+=SIZEOF_BYTE;
            }
            else
            {
                cgPushAtoC64Stack( pASM , level ) ;
            }
        break;
        case eTermType_u16    :    
        case eTermType_u32    :    
        case eTermType_u64    :    
        case eTermType_s16    :    
        case eTermType_s32    :    
        case eTermType_s64    :
            if ( tToken == eToken_assign )
            {
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr ,"jsr stack.push.word" );
                *kStack+=SIZEOF_WORD;
            }
            else
            {
                cgPushAYtoC64Stack( pASM , level ) ;
            }
        break;
        case eTermType_r32    :    
        case eTermType_r40    :    
        case eTermType_r64    :    
        case eTermType_r80    :
            if ( tToken == eToken_assign )
            {
                assemblerTab( pASM, level ) ;                
                fprintf ( pASM->pFileASM->ptr ,"jsr stack.push.real" );
                *kStack+=SIZEOF_REAL;
            }
            else
            {
                cgPushFAC1fromC64Stack( pASM , level ) ;
            }
        break;
        $ASMdefault("cgPushValueType :: switch(pNode->baseType)",pNode->baseType);
        break;
    }

    if ( *kStack >= pASM->pPar->pLex->pStream->pCompiler->stackMaxSize )             
        $asmErrorExtraLast( coding,overflow,"stack >= stackMaxSize" ) ;

}

//..................................................  cgStoreAddress

void cgStoreAddress( assembler_t *pASM  , int level )
{
    assemblerTab( pASM, level ) ;
    fprintf( pASM->pFileASM->ptr , "sta zpWord0+0");
    assemblerTab( pASM, level ) ;
    fprintf( pASM->pFileASM->ptr , "sty zpWord0+1");
}

//.................................................. cgPopValueType

void cgPopValueType( assembler_t *pASM   , node_t* pNode , int* kStack , int level , eToken_t tToken )
{
    eTermType_t cast = pNode->baseType ;
    
    if ( pNode->baseKind == eTermKind_addr )
    {
        cast = eTermType_u16;
    }

    switch(cast) 
    {
        case eTermType_u8    :
        case eTermType_s8    :
            if ( tToken == eToken_assign )
            {
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr ,"jsr stack.pop.byte" );
                *kStack-=SIZEOF_BYTE;
            }
            else
            {
                cgPopAfromC64Stack( pASM , level ) ;
            }
        break;
        case eTermType_u16    :    
        case eTermType_u32    :    
        case eTermType_u64    :    
        case eTermType_s16    :    
        case eTermType_s32    :    
        case eTermType_s64    :
            if ( tToken == eToken_assign )
            {
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr ,"jsr stack.pop.word" );
                *kStack-=SIZEOF_WORD;
            }
            else
            {
                cgPopAYfromC64Stack( pASM , level ) ;
            }
        break;
        case eTermType_r32    :    
        case eTermType_r40    :    
        case eTermType_r64    :    
        case eTermType_r80    :
            if ( tToken == eToken_assign )
            {
                assemblerTab( pASM, level ) ;                
                fprintf ( pASM->pFileASM->ptr ,"jsr stack.pop.real" );
                *kStack-=SIZEOF_REAL;
            }
            else
            {
                cgPopFAC1fromC64Stack( pASM , level ) ;
            }
        break;
        $ASMdefault("cgPopValueType :: switch(pNode->baseType)",pNode->baseType);
        break;
    }
    if ( *kStack < 0 )         
        $asmErrorExtraLast( coding,overflow,"stack < 0" ) ;    
}

//.................................................. cgCopyAYto_zpWord0

void cgCopyAYto_zpWord0( assembler_t *pASM, int level )
{
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sta zpWord0+0");
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sty zpWord0+1");
}
void cgStoreAYto_zpAY( assembler_t *pASM, int level )
{
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sta zpa");
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sty zpy");
}
void cgLoadAYto_zpAY( assembler_t *pASM, int level )
{
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "lda zpa");
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "ldy zpy");
}
void cgCopyAYto_zpWord1( assembler_t *pASM, int level )
{
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sta zpWord1+0");
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sty zpWord1+1");
}
void cgTAY_POPA( assembler_t *pASM, int level )
{
    // tay
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tay");
    // pla
    cgPopAfromC64Stack    ( pASM , level ) ;    
}
    
//.................................................. cgBinOp_eToken_add_u8s8

void cgBinOp_eToken_add_u8s8( assembler_t *pASM, int level )
{
    // sta zpa
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sta zpa");
    // pla
    cgPopAfromC64Stack    ( pASM , level ) ;
    // clc
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "clc");
    // adc zpa
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "adc zpa");
}

//.................................................. cgBinOp_eToken_add_u16s16

void cgBinOp_eToken_add_u16s16( assembler_t *pASM, int level )
{
    // zpWord0 := a,y
    cgCopyAYto_zpWord0    ( pASM , level ) ;
    // pla,y
    cgPopAYfromC64Stack    ( pASM , level ) ;
    // add_u16s16
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.add_u16s16");
}

//.................................................. cgBinOp_eToken_add_f40

void cgBinOp_eToken_add_f40( assembler_t *pASM, int level )
{
    // pop fac1 to fac2
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr stack.pop.real_c64_to_fac2");
    // fac1 += fac2
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.add_f40");
}

//.................................................. cgBinOp_eToken_mul_u8s8

void cgBinOp_eToken_mul_u8s8( assembler_t *pASM, int level )
{
    // tay
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tay");    // right to y
    // pla
    cgPopAfromC64Stack    ( pASM , level ) ;                                   // left  to a
    // mul8
    assemblerTab( pASM, level ) ;
    // ### OPTIMIZE
    if ( pASM->pPar->pLex->pStream->pCompiler->fOptimise==false) 
        fprintf( pASM->pFileASM->ptr , "jsr math.mul_u8s8");
    else
        fprintf( pASM->pFileASM->ptr , "jsr math.mul_u8s8_fast");
}

//.................................................. cgBinOp_eToken_mul_u16s16

void cgBinOp_eToken_mul_u16s16( assembler_t *pASM, int level )
{
    //           A/Y             = first  16-bit number, 
    //           zpWord0 in ZP   = second 16-bit number

    // zpWord0 := a,y
    cgCopyAYto_zpWord0    ( pASM , level ) ;                // left in zpWord0
    // pla,y
    cgPopAYfromC64Stack    ( pASM , level ) ;                // right in zpWord1 (do routine)
    
    assemblerTab( pASM, level ) ;
    // ### OPTIMIZE
    if ( pASM->pPar->pLex->pStream->pCompiler->fOptimise==false) 
        fprintf( pASM->pFileASM->ptr , "jsr math.mul_u16s16");
    else
        fprintf( pASM->pFileASM->ptr , "jsr math.mul_u16s16_fast");
    
}

//.................................................. cgBinOp_eToken_mul_f40

void cgBinOp_eToken_mul_f40( assembler_t *pASM, int level )    // fac := fac1 * fac2
{
    // pop fac1 to fac2
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr stack.pop.real_c64_to_fac2");    // left fac2
    // fac1 += fac2
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.mul_f40");                  // right fac1
}

//.................................................. cgBinOp_eToken_div_u8

void cgBinOp_eToken_div_u8( assembler_t *pASM, int level )
{
    // tay
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tay");    // right to y
    // pla
    cgPopAfromC64Stack    ( pASM , level ) ;                                    // left  to a
    // divu8
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.div_u8");
}

//.................................................. cgBinOp_eToken_div_s8

void cgBinOp_eToken_div_s8( assembler_t *pASM, int level )
{
    // tay
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tay");    // right to y
    // pla
    cgPopAfromC64Stack    ( pASM , level ) ;            // left  to a
    // divs8
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.div_s8");
}

//.................................................. cgBinOp_eToken_div_u16

void cgBinOp_eToken_div_u16( assembler_t *pASM, int level )
{
    // occorre invertire ay zpWord0 -> zpWord0 / ay
    cgStoreAYto_zpAY    ( pASM , level ) ;
    cgPopAYfromC64Stack ( pASM , level ) ;
    cgCopyAYto_zpWord0  ( pASM , level ) ;
    cgLoadAYto_zpAY     ( pASM , level ) ;
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.div_u16");
}

//.................................................. cgBinOp_eToken_div_s16

void cgBinOp_eToken_div_s16( assembler_t *pASM, int level )
{
    // occorre invertire ay zpWord0 -> zpWord0 / ay
    cgStoreAYto_zpAY    ( pASM , level ) ;
    cgPopAYfromC64Stack ( pASM , level ) ;
    cgCopyAYto_zpWord0  ( pASM , level ) ;
    cgLoadAYto_zpAY     ( pASM , level ) ;
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.div_s16");
}

//.................................................. cgBinOp_eToken_div_f40

void cgBinOp_eToken_div_f40( assembler_t *pASM, int level )    // fac1 := fac2 / fac1
{
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr stack.pop.real_c64_to_fac2");
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.div_f40.fac2_fac1");
}

//.................................................. cgBinOp_eToken_mod_u8

void cgBinOp_eToken_mod_u8( assembler_t *pASM, int level )
{
    // tay
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tay");     // right to y
    // pla
    cgPopAfromC64Stack    ( pASM , level ) ;                                    // left  to a
    // divu8
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.mod_u8");
}

//.................................................. cgBinOp_eToken_mod_s8

void cgBinOp_eToken_mod_s8( assembler_t *pASM, int level )
{
    // tay
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tay");     // right to y
    // pla
    cgPopAfromC64Stack    ( pASM , level ) ;                                    // left  to a
    // divs8
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.mod_s8");
}

//.................................................. cgBinOp_eToken_mod_u16

void cgBinOp_eToken_mod_u16( assembler_t *pASM, int level )
{
    // occorre invertire ay zpWord0 -> zpWord0 / ay
    cgStoreAYto_zpAY    ( pASM , level ) ;
    cgPopAYfromC64Stack    ( pASM , level ) ;
    cgCopyAYto_zpWord0    ( pASM , level ) ;            
    cgLoadAYto_zpAY        ( pASM , level ) ;
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.mod_u16");
}

//.................................................. cgBinOp_eToken_mod_f40

void cgBinOp_eToken_mod_f40( assembler_t *pASM, int level )    // fac1 := fac2 % fac1
{
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr stack.pop.real_c64_to_fac2");
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.mod_f40.fac2_fac1");
}

//.................................................. cgBinOp_eToken_mod_s16

void cgBinOp_eToken_mod_s16( assembler_t *pASM, int level )
{
    // occorre invertire ay zpWord0 -> zpWord0 / ay
    cgStoreAYto_zpAY    ( pASM , level ) ;
    cgPopAYfromC64Stack ( pASM , level ) ;
    cgCopyAYto_zpWord0  ( pASM , level ) ;
    cgLoadAYto_zpAY     ( pASM , level ) ;
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.mod_s16");
}

//.................................................. cgBinOp_eToken_sub_u8s8

void cgBinOp_eToken_sub_u8s8( assembler_t *pASM, int level )
{
    // sta zpa
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sta zpa");
    // pla
    cgPopAfromC64Stack    ( pASM , level ) ;
    // clc
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sec");
    // sbc zpa
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sbc zpa");
}

//.................................................. cgBinOp_eToken_sub_u16s16

void cgBinOp_eToken_sub_u16s16( assembler_t *pASM, int level )
{
    // left e right devono essere invertiti
    // ex left 513 - right 257
    cgCopyAYto_zpWord1    ( pASM , level ) ;        // right 257

    cgPopAYfromC64Stack    ( pASM , level ) ;        // left 513
    cgCopyAYto_zpWord0    ( pASM , level ) ;        // zpWord0 - zpWord1

    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.sub_u16s16.start");
}

//.................................................. cgBinOp_eToken_sub_f40

void cgBinOp_eToken_sub_f40( assembler_t *pASM, int level )
{
    // pop fac1 to fac2
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr stack.pop.real_c64_to_fac2");
    // fac1 -= fac2
    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.sub_f40");
}


//-------------
//        cgBinOp
//-------------

void cgBinOp( assembler_t *pASM, node_t* pNode , node_t* pLeft ,  node_t* pRight, int* kStack , int level , eToken_t tokenType )
{
    (void)pLeft;
    (void)pRight;
    (void)kStack;
    
    // left     :    a ay fac1
    // push     :    a ay fac1    stack+ logical
    // asm(right)
    // right    :    a ay fac1
    // push     :    a ay fac1    stack+ hardware
    // pop      :    a ay fac1    stack- logical        zpa    zpWord0 fac1
    // pop      :    a ay fac1    stack- hardware        zpa    zpWord0 fac1
    
    switch( pNode->baseType )
    {
        case eTermType_u8:
                if  ( tokenType==eToken_add ) cgBinOp_eToken_add_u8s8(pASM,level);
                if  ( tokenType==eToken_mul ) cgBinOp_eToken_mul_u8s8(pASM,level);
                if  ( tokenType==eToken_div ) cgBinOp_eToken_div_u8  (pASM,level);
                if  ( tokenType==eToken_mod ) cgBinOp_eToken_mod_u8  (pASM,level);
                if  ( tokenType==eToken_sub ) cgBinOp_eToken_sub_u8s8(pASM,level);
        break;
        
        case eTermType_s8:
                if  ( tokenType==eToken_add ) cgBinOp_eToken_add_u8s8(pASM,level);
                if  ( tokenType==eToken_mul ) cgBinOp_eToken_mul_u8s8(pASM,level);
                if  ( tokenType==eToken_div ) cgBinOp_eToken_div_s8  (pASM,level);
                if  ( tokenType==eToken_mod ) cgBinOp_eToken_mod_s8  (pASM,level);
                if  ( tokenType==eToken_sub ) cgBinOp_eToken_sub_u8s8(pASM,level);
        break;
        
        case eTermType_u16:
                if  ( tokenType==eToken_add ) cgBinOp_eToken_add_u16s16(pASM,level);
                if  ( tokenType==eToken_mul ) cgBinOp_eToken_mul_u16s16(pASM,level);
                if  ( tokenType==eToken_div ) cgBinOp_eToken_div_u16   (pASM,level);
                if  ( tokenType==eToken_mod ) cgBinOp_eToken_mod_u16   (pASM,level);
                if  ( tokenType==eToken_sub ) cgBinOp_eToken_sub_u16s16(pASM,level);
        break;
        
        case eTermType_s16:
                if  ( tokenType==eToken_add ) cgBinOp_eToken_add_u16s16(pASM,level);
                if  ( tokenType==eToken_mul ) cgBinOp_eToken_mul_u16s16(pASM,level);
                if  ( tokenType==eToken_div ) cgBinOp_eToken_div_s16   (pASM,level);
                if  ( tokenType==eToken_mod ) cgBinOp_eToken_mod_s16   (pASM,level);
                if  ( tokenType==eToken_sub ) cgBinOp_eToken_sub_u16s16(pASM,level);
        break;

        case eTermType_r32:
        case eTermType_r40:
        case eTermType_r64:
        case eTermType_r80:
                if  ( tokenType==eToken_add ) cgBinOp_eToken_add_f40(pASM,level);
                if  ( tokenType==eToken_mul ) cgBinOp_eToken_mul_f40(pASM,level);
                if  ( tokenType==eToken_div ) cgBinOp_eToken_div_f40(pASM,level);
                if  ( tokenType==eToken_mod ) cgBinOp_eToken_mod_f40(pASM,level);
                if  ( tokenType==eToken_sub ) cgBinOp_eToken_sub_f40(pASM,level);
        break;
        
        $ASMdefault("cgBinOp :: switch( pNode->baseType )",pNode->baseType);
    }
}

//.................................................. cgBinOpAssign

void cgBinOpAssign( assembler_t *pASM, node_t* pLeft ,  node_t* pRight, int* kStack , int level )
{
    // left  : $+ := ay         :    address
    // right : a? ay? fac1?     :    value

    switch( pRight->baseType )
    {
        case eTermType_u8:
        case eTermType_s8:
                cgPushAtoC64Stack     ( pASM , level ) ;     // a value
                cgPopValueType        ( pASM , pLeft , kStack ,  level , eToken_assign ) ; // ay address
                cgStoreAddress        ( pASM , level ) ;    // zpWord0
                cgPopAfromC64Stack    ( pASM , level ) ;    // a value
                assemblerTab( pASM, level ) ;    
                fprintf( pASM->pFileASM->ptr , "jsr store_pByte");
        break;
        
        case eTermType_u16:
        case eTermType_s16:
                cgPushAYtoC64Stack    ( pASM , level ) ;     // ay value
                cgPopValueType        ( pASM , pLeft , kStack ,  level , eToken_assign ) ; // ay address
                cgStoreAddress        ( pASM , level ) ;     // zpWord0
                cgPopAYfromC64Stack   ( pASM , level ) ;    // ay value
                assemblerTab( pASM, level ) ;    
                fprintf( pASM->pFileASM->ptr , "jsr store_pWord");
        break;

        case eTermType_r32:
        case eTermType_r40:
        case eTermType_r64:
        case eTermType_r80:
                // no push fac1 ( fac1 non viene sovrascritto )
                // cgPushFAC1fromC64Stack( pASM , level ) ;     // FAC1 value
                cgPopValueType( pASM ,  pLeft , kStack ,  level , eToken_assign ) ;
                cgStoreAddress( pASM , level ) ;
                // cgPopFAC1fromC64Stack( pASM , level ) ;      // FAC1 value
                assemblerTab( pASM, level ) ;    
                fprintf( pASM->pFileASM->ptr , "jsr store_pReal");
        break;
        $ASMdefault("cgBinOpAssign :: switch( pRight->baseType )",pRight->baseType);
    }
}

// ******************
//     emit type
// ******************

void cgEmitType( assembler_t *pASM,int level,scope_t *ps) 
{
    //if (0)
    if ( ps != NULL )
        for(;;)
        {
            if ( ps->pTypeDef->size )
            {
                for(size_t i=0;i<ps->pTypeDef->size;i++)
                {
                    pType_t* pt = ps->pTypeDef->data[i] ;
                    
                    if ( pt->type == eRecordType )
                    {
                        assemblerTab( pASM, level ) ; 
                        assemblerTab( pASM, level ) ;  
                        fprintf( pASM->pFileASM->ptr , "%s .struct",pt->id);

                        ++level;

                            int kRecordMax = ps->pTypeDef->data[i]->kRecordField ;

                            for(size_t j=0;j< (size_t)kRecordMax ;j++)
                            {
                               assemblerTab( pASM, level ) ;
                               
                               fprintf( pASM->pFileASM->ptr ,"%-12s %-7s ?"
                                    ,   ps->pTypeDef->data[i]->aRecordField_name[j]
                                    ,stGet64TassNameFromType(
                                        ps->pTypeDef->data[i]->aRecordField_type[j])
                               );
                            }
                            
                        --level;
                        
                        assemblerTab( pASM, level ) ;  
                        fprintf( pASM->pFileASM->ptr , ".endstruct");
                    }
                }
            }    
            break;
        }    
}

// ******************
//     info optimizer
// ******************

void oEmitInfo( assembler_t *pASM , int level,char *fmt, ...) 
{
    if ( pASM->pPar->pLex->pStream->pCompiler->fOptimise == true )
    {
        assemblerTab( pASM, level ) ;
        va_list ap;
        va_start(ap, fmt);
        vfprintf(pASM->pFileASM->ptr, fmt, ap);
    }
}

// ******************
//     assembler AST
// ******************

#define left  1
#define right 2

node_t*        assemble( assembler_t *pASM ,node_t* pNode , scope_t* pScope , int left_right )
{
    static int level=1;

    if ( pNode==NULL ) 
    {
        return NULL ;
    }

    switch( pNode->nType )
    {
        // ________________________________________________________________ nTypeUndefined
        case    nTypeUndefined :
        {
            break ;
        }
        // ________________________________________________________________ nTypeTerm
        case    nTypeTerm :
        {
            $ASMdebugNodeRC

            assert (( aStackAddrValue[kaStackAddrValue]==0 ) || ( aStackAddrValue[kaStackAddrValue]==1)) ;

            if  ( aStackAddrValue[kaStackAddrValue] == emitValue ) // ## VALUE
            {
                switch( pNode->pData->type ) 
                {
                    case eToken_integer:    cgEmitValueConstInteger    ( pASM , pNode , level ) ;
                    break;
                    case eToken_real:       cgEmitValueConstReal    ( pASM , pNode , level ) ;
                    break;
                    case eToken_mbs:        cgEmitAddressString        ( pASM , pNode , level ) ;
                    break;
                    case eToken_id:            // [v]var [v]ptr [@]arr [@]fn
								 if ( pNode->baseKind  == eTermKind_arr ) cgLoad_ID_Address ( pASM , pNode , level ) ;
							else if ( pNode->baseKind  == eTermKind_fn  ) cgLoad_ID_Address ( pASM , pNode , level ) ;
							else if ( pNode->baseKind  == eTermKind_ptr ) cgLoad_ID_Address ( pASM , pNode , level ) ;
                            else
																		  cgLoad_ID_Value	( pASM , pNode , level ) ;
                    break;
                    $ASMdefault("codegegen.c -> case nTypeTerm -> pNode->pData->type",pNode->pnTerm->pTerm->type );
                }
            }

            if  ( aStackAddrValue[kaStackAddrValue] == emitAddr )  // ## ADDRESS
            {
                switch( pNode->pData->type ) 
                {
                    case eToken_integer:    cgEmitValueConstInteger    ( pASM , pNode , level )     ;    // C64 EXT
                                            if ((pNode->baseType == eTermType_u8)) 
                                            {
                                                assemblerTab( pASM, level ) ;
                                                fprintf ( pASM->pFileASM->ptr , "ldy #00");
                                            }
                                            if ((pNode->baseType == eTermType_u16)) 
                                            {
                                                assemblerTab( pASM, level ) ;
                                                fprintf ( pASM->pFileASM->ptr , "ldy #00");
                                            }
                    break;
                    case eToken_real:       $asmErrorExtraLast        ( coding,LValueRequired,pNode->pData->buffer.data )    ;    
                         return NULL ;
                    break;
                    case eToken_mbs:        $asmErrorExtraLast        ( coding,LValueRequired,pNode->pData->buffer.data )    ;
                         return NULL ;
                    break;
                    case eToken_id:          cgLoad_ID_Address        ( pASM , pNode , level )     ;
                                            // [v]var [v]ptr [v]arr [v]fn
                    break;
                    $ASMdefault("codegegen.c -> case nTypeTerm -> pNode->pData->type",pNode->pnTerm->pTerm->type);
                }
            }
        }            
        break ;

        // ________________________________________________________________ nTypeDot
        case    nTypeDot:
        {
            $ASMdebugNodeRC
            
            assert( ++kaStackAddrValue < emitMax    ) ;

            aStackAddrValue[kaStackAddrValue] = emitAddr  ;
            
            assemble( pASM , pNode->pnDot->pLeft , pScope , left ) ;
            
            // emette indirizzo AY : salva in zpWord0
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sta zpWord0+0");
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sty zpWord0+1");

            char *rightID = pNode->pnDot->member ;
            
            char *leftID  = pNode->pnDot->pLeft->pData->buffer.data ;

            pSymbol_t*  pSymbol = stFind_pSymbolDef    ( pScope , leftID ) ; // ottieni symbolo
            
            uint8_t     offsetPos  = 0 ;
            for ( int i=0; i< pSymbol->pRecord->kRecordField; i++)
            {
                if ( strcmp(pSymbol->pRecord->aRecordField_name[i], rightID ) == 0 )
                {
                    offsetPos  = pSymbol->pRecord->aRecordField_offset[i] ;
                }
            }
            
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "clc"                   );
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "lda #%02d" ,offsetPos  );
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "adc zpWord0+0"         );
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sta zpWord0+0"         );
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "lda #00"               );
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tay"                   );
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "adc zpWord0+1"         );
            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "sta zpWord0+1"         );
            
            // RIGHT
            
          if (left_right == right ) // prende il valore
          {
                // char byte
                if ( ( pNode->baseType == eTermType_u8) || ( pNode->baseType == eTermType_s8) )
                {
                    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "lda (zpWord0),y");
                }
                // int word
                if ( ( pNode->baseType == eTermType_u16) || ( pNode->baseType == eTermType_s16) )
                {
                    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "lda (zpWord0),y");
                    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tax");
                    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "iny");
                    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "lda (zpWord0),y");
                    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "tay");
                    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "txa"); 
                }
                // real
                if ( ( pNode->baseType == eTermType_u8) || ( pNode->baseType == eTermType_s8) )
                {
                    assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "ldfac1 zpWord0");
                }
           }
               
            // LEFT
               
          if (left_right == left ) 
          {
                // RIMANE INDIRIZZO !
          }
            
            
            assert( --kaStackAddrValue >=       0   ) ;   
        }
        break ;
        
        // ________________________________________________________________ nTypeBinOp
        case    nTypeBinOp:
        {
            oEmitInfo(pASM,level,"; {{{ nTypeBinOp eToken_t %d",pNode->pData->type);
            
            $ASMdebugNodeRC
            
            assert( ++kaStackAddrValue < emitMax ) ;

            // ................................................................. LEFT
            
            if (  pNode->pData->type == eToken_assign )
			{				
                    aStackAddrValue[kaStackAddrValue] = emitAddr  ;
					if ( pNode->pnBinOp->pLeft->baseKind == eTermKind_ptr )
					{
						pNode->baseType = pNode->pnBinOp->pLeft->baseType ;
					}
			}		 
            else
			{				
                    aStackAddrValue[kaStackAddrValue] = emitValue ;
			}

            assemble ( pASM , pNode->pnBinOp->pLeft , pScope , left ) ;

            assert(    --kaStackAddrValue >=   0 ) ;    

            // ................................................................. RIGHT
            
            cgPushValueType(pASM,pNode->pnBinOp->pLeft, &fn_stackSize , level , pNode->pData->type ); // a ay fac1
            
            assemble ( pASM , pNode->pnBinOp->pRight , pScope , right ) ;    

            switch(pNode->pData->type)
            {
                case   eToken_mul:
                        cgBinOp( pASM , pNode , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight , &fn_stackSize , level , eToken_mul ) ;
                break;
                case   eToken_div:
                        cgBinOp( pASM , pNode , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight , &fn_stackSize , level , eToken_div ) ;
                break;
                case   eToken_mod:
                        cgBinOp( pASM , pNode , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight , &fn_stackSize , level , eToken_mod ) ;
                break;
                case   eToken_sub:
                        cgBinOp( pASM , pNode , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight , &fn_stackSize , level , eToken_sub ) ;
                break;
                case   eToken_add:
                        cgBinOp( pASM , pNode , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight , &fn_stackSize , level , eToken_add ) ;
                break;
                break;
                case   eToken_assign:
                        cgBinOpAssign( pASM , pNode->pnBinOp->pLeft , pNode->pnBinOp->pRight , &fn_stackSize , level ) ; 
                break;
                $ASMdefault("case    nTypeBinOp :: switch(pNode->baseType)",pNode->pData->type);
                break;
            }

            oEmitInfo(pASM,level,"; }}}");
        }
        break ;
        
        // ________________________________________________________________ nTypePrefix
        case nTypePrefix :
        {
            // Ho messo in assemble ( ... , left/right ), in quanto per una evil optimisation
            // relativo ai puntatori, ho saltato un passaggio , per recupaare la l'indizirro del contenuto del puntatore
            // ma mettendo solo il valore, ora right e left si comportano diversamente :
            // if (left_right == right ) fprintf( pASM->pFileASM->ptr , "jsr mem.load.byte");
            
            $ASMdebugNodeRC

            assert( ++kaStackAddrValue < SIZEOF_STACK ) ;

            if  ( pNode->pData->type == eToken_addr )  
            {
                aStackAddrValue[kaStackAddrValue] = emitAddr ;
            }                    
            else 
            {
                aStackAddrValue[kaStackAddrValue] = emitValue ;
            }

            assemble ( pASM , pNode->pnPrefix->pLeft , pScope , left_right ) ;    

            switch ( pNode->pData->type )
            {
                // .......................................................... eToken_addu    +
                case eToken_null:
                    // riferito a prefisso annullato, vedesi neg real, ...
                break;
                // .......................................................... eToken_addr    &
                case eToken_addr:
                    // [v] :: Emesso in top nTypePrefix
                    if (( pNode->pnPrefix->pLeft->baseType == eTermType_s8  ) // check invalid pointer Real and Signed
                    ||     ( pNode->pnPrefix->pLeft->baseType == eTermType_s16 ) 
                    ||     ( pNode->pnPrefix->pLeft->baseType == eTermType_r40 ))
                    {  $asmErrorExtraLast( parsing,invTypeArgUnaryAddrOf,pNode->pnPrefix->pLeft->pData->buffer.data ) ; } 
                break;
                // .......................................................... eToken_ptr    *
                case eToken_ptr:
                {
                    // [v] :: eToken_ptr
                    if (   ( pNode->pnPrefix->pLeft->baseType == eTermType_s8  ) // check invalid pointer Real and Signed
                    ||     ( pNode->pnPrefix->pLeft->baseType == eTermType_s16 ) 
                    ||     ( pNode->pnPrefix->pLeft->baseType == eTermType_r40 ))
                    {  $asmErrorExtraLast( parsing,invTypeArgUnaryPtr,pNode->pnPrefix->pLeft->pData->buffer.data ) ; } 

                    if (  ( pNode->baseType == eTermType_u8 ) 
                    ||    ( pNode->baseType == eTermType_s8 )) 
                    {
                        assemblerTab( pASM, level ) ;
                        if (left_right == right ) fprintf( pASM->pFileASM->ptr , "jsr mem.load.byte_ptr");
                    } 
                    if (  ( pNode->baseType == eTermType_u16 ) 
                    ||    ( pNode->baseType == eTermType_s16 )) 
                    {
                        assemblerTab( pASM, level ) ;
                        if (left_right == right ) fprintf( pASM->pFileASM->ptr , "jsr mem.load.word_ptr");
                    } 
                    if (( pNode->baseType == eTermType_r32 ) ||    ( pNode->baseType == eTermType_r40 ) 
                    ||  ( pNode->baseType == eTermType_r64 ) ||    ( pNode->baseType == eTermType_r80 ) ) 
                    {
                        assemblerTab( pASM, level ) ;
                        if (left_right == right ) fprintf( pASM->pFileASM->ptr , "jsr mem.load.real_ptr");
                    } 
					
                    if ( left_right == left ) // evil optimisation ...
                    {
                        assemblerTab( pASM, level ) ;
                        fprintf ( pASM->pFileASM->ptr , "stack_load_word ;");
                    }
                    //$asmErrorExtraLast( coding,invTypeArgUnaryPtr,pNode->pData->buffer.data ) ;
                }
                break;
                // .......................................................... eToken_addu    +
                case eToken_addu:
                    // [v] :: eToken_addu
                break;
                // .......................................................... eToken_subu    -
                case eToken_subu:
                    if ( pNode->pnPrefix->pLeft->baseType == eTermType_u8  )
                    {
                        assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "math_neg_u8s8");
                    }
                    if ( pNode->pnPrefix->pLeft->baseType == eTermType_s8  )
                    {
                        assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "math_neg_u8s8");
                    }
                    if ( pNode->pnPrefix->pLeft->baseType == eTermType_u16  )
                    {
                        assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "math_neg_u16s16");
                    }
                    if ( pNode->pnPrefix->pLeft->baseType == eTermType_s16  )
                    {
                        assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "math_neg_u16s16");
                    }
                    if ( pNode->pnPrefix->pLeft->baseType == eTermType_r40  )
                    {
                        assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "math_neg_f40");
                    }
                break;
                // .......................................................... eToken_not    !
                case eToken_not:
                
                    if (( pNode->pnPrefix->pLeft->baseType == eTermType_u8  )
                    ||    ( pNode->pnPrefix->pLeft->baseType == eTermType_s8  ))
                    {
                        if ( pASM->pPar->pLex->pStream->pCompiler->fOptimise==false) {
                            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.not_u8s8");
                        } else {
                            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "math_not_u8s8_fast");
                        }
                    }
                    if (( pNode->pnPrefix->pLeft->baseType == eTermType_u16  )
                    ||    ( pNode->pnPrefix->pLeft->baseType == eTermType_s16  ))
                    {
                        if ( pASM->pPar->pLex->pStream->pCompiler->fOptimise==false) { 
                            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.not_u16s16");
                        } else {
                            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "math_not_u16s16_fast");
                        }                            
                    }
                    if ( pNode->pnPrefix->pLeft->baseType == eTermType_r40  )
                    {
                        if ( pASM->pPar->pLex->pStream->pCompiler->fOptimise==false) {
                            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "jsr math.not_f40");
                        } else {
                            assemblerTab( pASM, level ) ;    fprintf( pASM->pFileASM->ptr , "math_not_f40_fast");
                        }
                    }
                    pNode->baseType = eTermType_u8 ;
                break;
                // .......................................................... eToken_type    ( CAST ESPLICITO )
                case eToken_type:
                {
                    // [v] :: eToken_type
                    pType_t* pType = stFind_pTypeDef( pScope,&pNode->pData->mbs[1] ) ;
                    assert(pType!=NULL);
                
                    eTermType_t castTo = stGetTypeFromName( pType->id ) ;
                     
                    // se è indicato la presenza di un pointer cast to u16
                    cgCastToBaseType( 
                        pASM      , pNode->pnPrefix->pLeft->baseType , 
                        castTo     , level ,    
                        pNode->pData->ptr ,pNode->pData->row,pNode->pData->col 
                    ) ;
                }
                break;
                $ASMdefault("nTypePrefix :: switch ( pNode->type )",( pNode->pData->type ));
            }
            
            assert(--kaStackAddrValue >=   0 ) ;

        }
        break ;

        // ________________________________________________________________ nTypeBlock [scope]
        case nTypeBlock :
        {
            $ASMdebugNodeRC
            
            cgBlockBegin(pASM,level);    
            ++level;
            
            cgEmitType(pASM,level,pNode->pnBlock->bScope) ;
            
            for(uint32_t i=0;i<pNode->pnBlock->pvNode->size;i++)
            {
                
                assemble ( pASM , pNode->pnBlock->pvNode->data[i]  , pNode->pnBlock->bScope , left_right ) ;
            }
            --level;
            cgBlockEnd(pASM,level);    
        }
        break ;

        // ________________________________________________________________ nodeASM_t
        case    nTypeASM:
        {
            $ASMdebugNodeRC

            cgAssembler( pASM,level , pNode->pData->mbs ) ;
        }
        break ;

        // ________________________________________________________________ nTypeProgram [scope]
        case    nTypeProgram:
        {
            $ASMdebugNodeRC

            cgEmitSymbolGlobal(pASM,pNode);    
            
            assemble ( pASM , pNode->pnProgram->body , pNode->pnProgram->body->pnBlock->bScope  , left_right ) ;
        }
        break ;

        // ________________________________________________________________ nTypeFunction [scope]
        case    nTypeFunction:
        {
            fprintf ( pASM->pFileASM->ptr , "\n");
            $ASMdebugNodeRC

            ++level ;

            cgFunctionTop(pASM,level,pNode->pnFunction->fName);

            cgEmitType(pASM,level,pNode->pnFunction->fScope) ;

            cgStackAlloc( pASM , pNode->pnFunction->stackSize , level ) ;

            fn_stackSize = pNode->pnFunction->stackSize ;
            
            cgEmitSymbolLocal( pASM, pNode, level ) ;

            assemble ( pASM , pNode->pnFunction->fBlock , pNode->pnFunction->fScope  , left_right ) ;

            cgStackDeAlloc( pASM , pNode->pnFunction->stackSize , level ) ;
            
            cgFunctionBottom(pASM,level);    

            --level;
        }
        break ;
        
        // ________________________________________________________________  nTypeParamFunction
        case nTypeParamFunction :
        {
            // occorre recuperare i tipi eParam nello scope prev ( eParam )
            pSymbol_t* pSymbolFunctionParam = stFind_pSymbolDef    ( pScope , pNode->pnParamFunction->fName ) ; // ex fff

            //printf ( "nTypeParamFunction , fName[%s] pScopeParamFunction[%lx] scope param [0x%lx] size[%d]\n"
            //    ,pNode->pnParamFunction->fName
            //    ,(size_t)pSymbolFunctionParam
            //    ,(size_t)pSymbolFunctionParam->pScope->prev
            //    ,(int   )pSymbolFunctionParam->pScope->prev->pSymbolDef->size    // ex lw 1
            //) ;
            
            $ASMdebugNodeRC

            if ( pNode->pnParamFunction->fBlock!=NULL)
            {
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr ,"param_begin");
                ++level ;
                
                if ( pNode->pnParamFunction->fBlock->pnBlock->pvNode->size > pSymbolFunctionParam->pScope->prev->pSymbolDef->size )
                {
                    $asmErrorExtraLast( coding,tooManyArgument,pNode->pnParamFunction->fName ) ;
                    return NULL ;
                }
                if ( pNode->pnParamFunction->fBlock->pnBlock->pvNode->size < pSymbolFunctionParam->pScope->prev->pSymbolDef->size )
                {
                    $asmErrorExtraLast( coding,tooFewArgument,pNode->pnParamFunction->fName ) ;
                    return NULL ;
                }
                
                    size_t dim = pNode->pnParamFunction->fBlock->pnBlock->pvNode->size ;
                    int    keParam=0;// ePram non sono reverse
                    for (unsigned int i=dim;i>=1;i--) // reverse
                    {
                        node_t* pNodeTemp = pNode->pnParamFunction->fBlock->pnBlock->pvNode->data[i-1] ;

                        assemble( pASM , pNodeTemp , pScope  , left_right ) ;

                        //printf ( " cast param %d to %d \n" 
                        //, pNodeTemp->baseType , pSymbolFunctionParam->pScope->prev->pSymbolDef->data[i-1]->type ) ;

                        cgCastToBaseType( 
                            pASM      , 
                            pNodeTemp->baseType ,                                                       // cast from 
                            pSymbolFunctionParam->pScope->prev->pSymbolDef->data[ keParam ]->type     , // cast to
                            level ,    pNode->pData->ptr ,pNode->pData->row,pNode->pData->col 
                        ) ;
                        
                        cgnTypeTermFunctionPushBaseType( pASM , pSymbolFunctionParam->pScope->prev->pSymbolDef->data[keParam]->type , level );    

                        ++keParam ; // passa al prossimo parametro
                    }
                    
                --level;
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr ,"param_end");
                assemblerTab( pASM, level ) ;
                fprintf ( pASM->pFileASM->ptr ,"jsr %s",pNode->pnParamFunction->fName );
            }
                
            return pNode;
        }
        break ;
        
        // ________________________________________________________________ END switch pNode
        $ASMdefault("switch( pNode->nType )",pNode->nType);
    }

    return pNode ;    
}

// ******************
//     assemble GO !
// ******************

node_t*      assembleGO    (assembler_t *pASM )
{
    cdPreamble( pASM ) ;
    
    assemble( pASM, pASM->pPar->pAST , pASM->pPar->pAST->pnProgram->body->pnBlock->bScope , 1 /*left*/) ;

    codeGenWriteAsmConst( pASM ) ;

    cgEpilogue( pASM ) ;

    return pASM->pPar->pAST ;
}