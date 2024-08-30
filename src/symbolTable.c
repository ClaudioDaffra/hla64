
#include "../lib/cxx.h"
#include "Cmos.h"

// ******************
// Symbol Table
// ******************

/*
    program
    
        pScope->     = program
        pScope->prev = NULL
    
        nTypeFunction
        
            pScope->     = nTypeFunction
            pScope->prev = program

            nTypeBlock1
            {
                pScope->     = nTypeBlock1
                pScope->prev = nTypeFunction
                    
                ntypeBlock2
                {
                    pScope->     = nTypeBlock2
                    pScope->prev = nTypeBlock1
                }

                ntypeBlock3
                {
                    pScope->     = nTypeBlock3
                    pScope->prev = nTypeBlock1
                }
            }
        
    end
*/


scope_t*        gScope;        
    
// gScope di interscambio, praticamente non serve a nulla
// in quanto li ho messi in locale

//...................................................................... stGetReduceTypeFromTermType

eTermType_t stGetReduceTypeFromTermType( token_t* pToken )
{
    if ( pToken->type == eToken_integer ) 
    {
        if ( (uint32_t)pToken->u64 < 256 ) 
        {
            if ( strcmp( pToken->type_suffix , "u8" ) == 0 ) return eTermType_u8 ;
            if ( strcmp( pToken->type_suffix , "s8" ) == 0 ) return eTermType_s8 ;
            if ( strcmp( pToken->type_suffix ,  ""  ) == 0 ) return eTermType_u8 ;
            
            return eTermType_u16 ;
        }
        else
        {
            return eTermType_u16 ;
        }
    }

    eTermType_t retRedType = pToken->type_num ;
    
    switch(retRedType)
    {
        case eTermType_null: 
            retRedType=eTermType_null; 
            break ;
        
        case eTermType_u8    :         //    1
            retRedType=eTermType_u8; 
            break ;
            
        case eTermType_s8    :         //    5    
            retRedType=eTermType_s8; 
            break ;
            
        case eTermType_u16    :        //    2
        case eTermType_u32    :        //    3
        case eTermType_u64    :        //    4
            retRedType=eTermType_u16; 
            break ;
        
        case eTermType_s16    :        //    6
        case eTermType_s32    :        //    7
        case eTermType_s64    :        //    8
            retRedType=eTermType_s16; 
            break ;
            
        case eTermType_r32    :        //    9
        case eTermType_r40    :        //    10
        case eTermType_r64    :        //    11
        case eTermType_r80    :        //    12
            retRedType=eTermType_r40; 
            break ;
            
        default:
            retRedType=eTermType_null; 
            break ;
    }
    return retRedType;
}

//...................................................................... stNewType

pType_t*    stNewType(char* name,eTermKind_t    kind,eTermType_t    type,uint8_t size,eScope_t scope )
{
    pType_t*    pType = (pType_t*) new (pType_t) ;
    
    pType->id       = gcStrDup( name ) ;
    pType->kind     = kind ;
    pType->type     = type ;
    pType->size     = size ;

    pType->kRecordField     = 0     ;
    pType->offset           = 0     ;
    pType->scope            = scope ;

    return pType;
}

//...................................................................... stInitDefaultType

// utile per definire i nuovi tipi record ( struct / union ) ;

vector_pTypeDef_t* stInitDefaultType(void)
{
    vector_pTypeDef_t* pstRecord = (vector_pTypeDef_t*) new (vector_pTypeDef_t) ;
    vectorAlloc( (*pstRecord) , 8 ) ;
    
    vectorPushBack( (*pstRecord) , stNewType("byte"    ,eTermKind_var    ,eTermType_u8     ,SIZEOF_BYTE , eScope_global ) );
    vectorPushBack( (*pstRecord) , stNewType("char"    ,eTermKind_var    ,eTermType_s8     ,SIZEOF_BYTE , eScope_global ) );
    vectorPushBack( (*pstRecord) , stNewType("word"    ,eTermKind_var    ,eTermType_u16    ,SIZEOF_WORD , eScope_global ) );
    vectorPushBack( (*pstRecord) , stNewType("int"     ,eTermKind_var    ,eTermType_s16    ,SIZEOF_WORD , eScope_global ) );
    vectorPushBack( (*pstRecord) , stNewType("real"    ,eTermKind_var    ,eTermType_r40    ,SIZEOF_REAL , eScope_global ) );
    
    return pstRecord;
}

//...................................................................... stNewSymbol

pSymbol_t* stNewSymbol( char *id, eTermKind_t kind,eTermType_t type,eScope_t scope )
{
    //pSymbol_t* t = stNewDefaultSymbol();
    
    pSymbol_t* t = (pSymbol_t*) new (pSymbol_t) ;
    
    t->id       = gcStrDup ( id )     ;
    t->kind     = kind                ;
    t->type     = type                ;
    t->scope    = scope               ;
    t->size     = stGetSizeFromType(type);
    t->offset   = 0;

    t->pScope   = gScope ; // in quale scope si trova 
    t->pRecord  = NULL   ; // non appartiene a nessuna struttura per default

    return t ;
}

//...................................................................... is type integger

int stIsInteger( eTermType_t type )
{
    if (type==eTermType_u8 )    return 1 ;
    if (type==eTermType_s8 )    return 1 ;
    if (type==eTermType_u16)    return 1 ;
    if (type==eTermType_s16)    return 1 ;
    return 0 ;
}

//...................................................................... stNewScope

// alloca vettore simbolo strutture/unioni/tipi del linguaggio e definiti dall'utente

scope_t* stNewSymbolTypeDef( scope_t* stScope )
{
    // ID    ;    int *ptr    ,    point arr[]
    stScope->pSymbolDef =  (vector_pSymbolDef_t*) new (vector_pSymbolDef_t)  ;
    vectorAlloc( (*stScope->pSymbolDef) , 8 ) ;
    
    // ID    ;    [int]        ,    [point]
    stScope->pTypeDef  = (vector_pTypeDef_t*) new (vector_pTypeDef_t)  ;
    vectorAlloc( (*stScope->pTypeDef) , 8 ) ;

    return stScope ;
}

scope_t* stNewScope(void)
{
    scope_t* stScope = (scope_t*) new (scope_t) ;
    
    memset(stScope, 0, sizeof(scope_t)) ;
    
    stScope->prev = NULL ;

    // ID    ;    int *ptr    ,    point arr[]
    stScope->pSymbolDef =  (vector_pSymbolDef_t*) new (vector_pSymbolDef_t)  ;
    vectorAlloc( (*stScope->pSymbolDef) , 8 ) ;
    
    // ID    ;    [int]        ,    [point]
    stScope->pTypeDef  = (vector_pTypeDef_t*) new (vector_pTypeDef_t)  ;
    vectorAlloc( (*stScope->pTypeDef) , 8 ) ;
    
    return gScope = stScope ;
}

//...................................................................... is primitive type

// char byte int word real

int stIsPrimitiveType(eToken_t t)
{
    if ((t>=eToken_primitive_type_begin) && (t<=eToken_primitive_type_end))  return 1 ;
    return 0 ;
}

//...................................................................... find symbol record

// I tipi sono i tipi primitivi e le struct e union definite dall'utente

pType_t* stFind_pTypeDef    ( scope_t* pScope , char *typeName )
{
    // vai a ritroso a ricercare i tipi predefiniti, negli scope dei genitori
    scope_t* ps=pScope;
    
    if ( ps != NULL )
        for(;;)
        {
            if ( ps->pTypeDef->size ) // occorre controllare se corrisponde ad un nome già definito
            {
                for(size_t i=0;i<ps->pTypeDef->size;i++)
                {
                    if ( strcmp( ps->pTypeDef->data[i]->id , typeName )==0 ) 
                    {
                        return  ps->pTypeDef->data[i];
                    }
                }
            }    
            ps=ps->prev;    
            
            if ( ps == NULL ) break ;
        }
    return NULL ;
}

//...................................................................... find symbol id

// i simboli sono gli identificatori

pSymbol_t* stFind_pSymbolDef    ( scope_t* pScope , char *symbolName )
{
    // vai a ritroso a ricercare i simboli predefiniti, negli scope dei genitori
    scope_t* ps=pScope;
    
    if ( ps != NULL )
        for(;;)
        {
            if ( ps->pSymbolDef->size ) // occorre controllare se corrisponde ad un nome già definito
            {
                for(size_t i=0;i<ps->pSymbolDef->size;i++)
                {
                    if ( strcmp( ps->pSymbolDef->data[i]->id , symbolName )==0 ) 
                    {
                        return  ps->pSymbolDef->data[i];
                    }
                }
            }    
            ps=ps->prev;
            
            if ( ps == NULL ) break ;
        }
    return NULL ;
}

pSymbol_t* stFind_pSymbolDefPos    ( scope_t* pScope , char *symbolName ,  size_t pos )
{
    (void)symbolName ;

    scope_t* ps=pScope;
    
    if ( ps != NULL )
    {
        if (pos<ps->pSymbolDef->size)    return  ps->pSymbolDef->data[pos];
    }
    
    return NULL ;
}

//...................................................................... stGetTypeFromName

eTermType_t stGetTypeFromName( char *name )
{
    if ( strcmp( name,"byte")==0 ) return eTermType_u8  ;
    if ( strcmp( name,"char")==0 ) return eTermType_s8  ;
    if ( strcmp( name,"word")==0 ) return eTermType_u16 ;
    if ( strcmp( name,"int" )==0 ) return eTermType_s16 ;
    if ( strcmp( name,"real")==0 ) return eTermType_r40 ;
    
    return eTermType_null ;
}

//...................................................................... stGetNameFromType

char* stGetCastNameFromType( eTermType_t t )
{
    if ( t == eTermType_u8  ) return gcStrDup("$byte");
    if ( t == eTermType_s8  ) return gcStrDup("$char");
    if ( t == eTermType_u16 ) return gcStrDup("$word");
    if ( t == eTermType_s16 ) return gcStrDup("$int" );
    if ( t == eTermType_u32 ) return gcStrDup("$word");
    if ( t == eTermType_s32 ) return gcStrDup("$int" );
    if ( t == eTermType_u64 ) return gcStrDup("$word");
    if ( t == eTermType_s64 ) return gcStrDup("$int" );
    
    if ( t == eTermType_r32 ) return gcStrDup("$real");
    if ( t == eTermType_r40 ) return gcStrDup("$real");
    if ( t == eTermType_r64 ) return gcStrDup("$real");
    if ( t == eTermType_r80 ) return gcStrDup("$real");
    
    return gcStrDup("${NULL}");
}

//...................................................................... stGet64TassNameFromType

char* stGet64TassNameFromType( eTermType_t t )
{
    if ( t == eTermType_u8  ) return gcStrDup(".byte");
    if ( t == eTermType_s8  ) return gcStrDup(".char");
    if ( t == eTermType_u16 ) return gcStrDup(".word");
    if ( t == eTermType_s16 ) return gcStrDup(".sint");
    if ( t == eTermType_u32 ) return gcStrDup(".word");
    if ( t == eTermType_s32 ) return gcStrDup(".sint");
    if ( t == eTermType_u64 ) return gcStrDup(".word");
    if ( t == eTermType_s64 ) return gcStrDup(".int" );
    if ( t == eTermType_r32 ) return gcStrDup(".byte");
    if ( t == eTermType_r40 ) return gcStrDup(".byte");
    if ( t == eTermType_r64 ) return gcStrDup(".byte");
    if ( t == eTermType_r80 ) return gcStrDup(".byte");
    
    return gcStrDup(".null");
}
//...................................................................... stAddNewSymbol

int stAddNewSymbol ( scope_t* pScope , pSymbol_t* pNewSymbol )
{
    vectorPushBack( (*pScope->pSymbolDef) , pNewSymbol );
    
    return vectorSize( (*pScope->pSymbolDef)) ;
}

//...................................................................... stGetSizeFromType

uint8_t stGetSizeFromType(eTermType_t t)
{
    uint8_t size=0;
    switch(t)
    {
        case eTermType_null: 
            size=0; break ;
        
        case eTermType_u8    :         //    1
        case eTermType_s8    :         //    5
            size=SIZEOF_BYTE; break ;
            
        case eTermType_u16    :        //    2
        case eTermType_u32    :        //    3
        case eTermType_u64    :        //    4
        case eTermType_s16    :        //    6
        case eTermType_s32    :        //    7
        case eTermType_s64    :        //    8
            size=SIZEOF_WORD; break ;
            
        case eTermType_r32    :        //    9
        case eTermType_r40    :        //    10
        case eTermType_r64    :        //    11
        case eTermType_r80    :        //    12
            size=SIZEOF_REAL; break ;
            
        default:
            size=0; break ;
    }
    return size;
}


// ...................................... stPromote

eTermType_t stPromote(eTermType_t lhs,eTermType_t rhs)
{
//                    eTermType_u8 eTermType_u16 eTermType_s8 eTermType_s16 eTermType_r40
//    eTermType_u8    eTermType_u8 ............. ............ ............. .............
//    eTermType_u16    ............ eTermType_u16 ............ ............. .............
//    eTermType_s8    ............ ............. eTermType_s8 ............. .............
//    eTermType_s16    ............ ............. ............ eTermType_s16 .............
//    eTermType_r40    ............ ............. ............ ............. eTermType_r40

    if ( lhs==rhs ) return lhs ;

    //if ( rhs == eTermType_null)  { puts("!! stPromote rhs == eTermType_null"); exit(-1); }
    //if ( lhs == eTermType_null) { puts("!! stPromote lhs == eTermType_null"); exit(-1); }

    if (  lhs == eTermType_null ) return rhs ;
    if (  rhs == eTermType_null ) return lhs ;
    
    if ( lhs == eTermType_u8 )
    {
             if ( rhs == eTermType_u8  )  return eTermType_u8  ;
        else if ( rhs == eTermType_u16 )  return eTermType_u16 ;
        else if ( rhs == eTermType_s8  )  return eTermType_s16 ;
        else if ( rhs == eTermType_s16 )  return eTermType_s16 ;        
        else if ( rhs == eTermType_r40 )  return eTermType_r40 ;
    }
        
    if ( lhs == eTermType_u16 )
    {
             if ( rhs == eTermType_u8  )  return eTermType_u16 ;
        else if ( rhs == eTermType_u16 )  return eTermType_u16 ;
        else if ( rhs == eTermType_s8  )  return eTermType_s16 ;
        else if ( rhs == eTermType_s16 )  return eTermType_s16 ;        
        else if ( rhs == eTermType_r40 )  return eTermType_r40 ;
    }        

    if ( lhs == eTermType_s8 )
    {
             if ( rhs == eTermType_u8  )  return eTermType_s16 ;
        else if ( rhs == eTermType_u16 )  return eTermType_s16 ;
        else if ( rhs == eTermType_s8  )  return eTermType_s8  ;
        else if ( rhs == eTermType_s16 )  return eTermType_s16 ;        
        else if ( rhs == eTermType_r40 )  return eTermType_r40 ;
    }        
        
    if ( lhs == eTermType_s16 )
    {
             if ( rhs == eTermType_u8  )  return eTermType_s16 ;
        else if ( rhs == eTermType_u16 )  return eTermType_s16 ;
        else if ( rhs == eTermType_s8  )  return eTermType_s16 ;
        else if ( rhs == eTermType_s16 )  return eTermType_s16 ;        
        else if ( rhs == eTermType_r40 )  return eTermType_r40 ;
    }

    // TODO puts("!! stPromote incompatible types when assigning to type 'record' from 'type' "); exit(-1); 
    
    return eTermType_r40 ;
}
