
#ifndef cdSymTable
#define cdSymTable

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#endif

#include "../lib/mvector.h"
#include "../lib/gc.h"
#include "../lib/hmap.h"
#include "error.h"
#include "ast.h"

// stScope in ast.h

// .............................................. kind

enum stKind_e
{
    stKindNull        ,
    stKindConst       ,    // 1
    stKindVar         ,    // 2
    stKindArray       ,    // 3
    stKindStruct      ,    // 4
    stKindFunction    ,    // 5
} ;

typedef enum stKind_e stKind_t ;

// .............................................. id

typedef wchar_t * wstring_t ;

// .............................................. nameSpace
struct vector_s_stNameSpace  
{                          
    wstring_t*   data 		;        
    size_t  	size        ;         
    size_t  	capacity    ;        
}  ;
//vectorType(wstring_t,stNameSpace) ;
extern struct vector_s_stNameSpace   stNameSpace ;

// .............................................. type
// to see : ast.h -> eNodeType_t

enum stType_e
{
    stTypeNull         ,
    stTypeInteger      ,
    stTypeReal         ,
    stTypeChar         ,    // il tipo string è un array di char
    stTypeByte         ,
    stTypeStruct       ,
    // ...
    stTypeConstString  ,    // compare solo con le costanti
} ;

typedef enum stType_e stType_t ;

// .............................................. sym Table

struct symTable_s ; // forward declaration

typedef struct symTable_s      symTable_t ;

typedef struct symTable_s *    psymTable_t ; // puntatore alla tabella dei simboli

typedef wchar_t*    pwchar_t ;

struct symTable_s
{
    stScope_t                scope         ; // Global / Local
    wstring_t                ns            ; // path
    wstring_t                id            ; // name
    stKind_t                 kind          ; // categoria          : Const/Var/Array/Type/Function
    stType_t                 type          ; // tipo    atomico    : Integer/Real/String/Char/Byte/Struct
    wstring_t                typeID        ; // struct name
    
    pnode_t                  array         ; // dimensione e nodi ;

    vectorStruct(pwchar_t,  member)        ; // membri della struttura.
    vectorStruct(size_t  ,  offset)        ; // offset dei membri della struttura
    
    size_t                    size         ; // dimesioni dell'oggetto

    union // mantenuto solo per le const
    {
      int64_t   integer     ;
      double    real        ;
      wchar_t*  wstring     ;
      wchar_t   wchar       ;
      uint8_t   byte        ;
    } value ;
    
} ;

// .............................................. tabella HASH stTable

typedef struct mapST_s
{
    
  wchar_t*       fullName     	;    // nome completo da ricercare
  psymTable_t    pST         	;   // puntatore alla tabella dei simboli
  
} mapST_t ;


extern whmapType(mapST) ;


// prototype


void         stInitSymTable       (void) ;
void         stDeInitSymTable     (void) ;

wchar_t*     stGet_nsid           (size_t level,wchar_t* id) ;
psymTable_t  stMakeSymTable       (void) ;
void         stDebugSymTableNode  (psymTable_t pst) ;
void         stShowMap            (void) ;

#define      stGetNameSpace(LEVEL)  stGet_nsid(LEVEL,NULL)
#define      stGetFullName(ID)      stGet_nsid(0,ID)

psymTable_t  stFindIDinMap        (wchar_t* id) ;
size_t       stGetSize            (pnode_t node) ;

#endif



/**/


