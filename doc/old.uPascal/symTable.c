
#include "global.h"
#include "symTable.h"

static FILE* pFileOutputST      ; // symTable.c
static char* fileNameOutputST   ;  

extern global_t g ;

stScope_t stScope ;

//vectorType(wstring_t,stNameSpace) ;

struct vector_s_stNameSpace stNameSpace ;

typedef wstring_t vectorDataTypestNameSpace;

whmapType(mapST) ;


// init stNameSpace

#define $CD gcWcsDup((wchar_t*)L"\\")

// .............................................. stInitSymTable

void stInitSymTable(void)
{
    pFileOutputST         = NULL ;
    fileNameOutputST     = NULL ;

    if ( g.fDebug ) 
    {
        fileNameOutputST = g.makeFileWithNewExt( g.fileInputName , ".symTable" ) ; // .............file symTable
        stdFileWOpen ( &pFileOutputST , fileNameOutputST , "w+","ccs=UTF-8") ;
        
        fwprintf ( pFileOutputST , L"\n# symTable   -> initializing ...\n\n" );
    }

    stScope = stScopeGlobal             ;    // parti dalle dichiarzioni globali
    
    vectorNew(stNameSpace,16)           ;    // definisci un iniziale spazio di 16 nomi in cascata
    
    vectorPushBack(stNameSpace,$CD)     ;    // inserisce il primo spazio dei nomi
    
    // GNU GCC 9.2 : lib/hmap.c:299:39: warning: 
    // ISO C forbids conversion of object pointer to function pointer type [-Wpedantic] C11
    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    #endif    
    
    mapST = whmapDef ( gcComparepWStrC );     // inizializza la funzione di ricerca nomi per la tabella hash
    
    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif
    
}

// .............................................. stDeInitSymTable

void stDeInitSymTable(void)
{
    if ( g.fDebug ) 
    {
        fwprintf ( pFileOutputST , L"\n# symTable   -> denitialized . \n\n" );
        if (  pFileOutputST != NULL ) fclose ( pFileOutputST ) ;
    }
}

// .............................................. st Get NameSpace(level) + id

// Questa funzione genera un nome composto da : NAMESPACE + ID
// size_t     level    : livelli da utilizzare per comporre il name space
//                    : 0 tutti i livelli
// wchar_t*    id        : nome attuale identificatore
//
// ex NS \f1
//          stGet_nsid(0,id)
//        \id
//        \f1id

wchar_t* stGet_nsid(size_t level,wchar_t* id) // ns(l)+id
{
	if ( id == NULL )
	{
		if ( g.fDebug ) fwprintf ( pFileOutputST , L"\n# symTable   -> stGet_nsid -> [id::{nil}]\n" ) ;
		return NULL ;
	}
	
    if ( g.fDebug ) fwprintf ( pFileOutputST , L"\n# symTable   -> stGet_nsid looking for [%ls]",id==NULL?L"{nil}":id );
    
    size_t MAXBUFFER = 4096 ;
    wchar_t* temp = gcMalloc( sizeof(wchar_t) * MAXBUFFER ) ;
    temp[0] = 0 ;
    size_t k=0;
    
    // 0         :    from 0 to vectorSize(stNameSpace)
    // 1 - x     :    from 0 to level
    if ( level==0 ) level=vectorSize(stNameSpace) ;
    
    if ( level<=vectorSize(stNameSpace) )
        for (  size_t i = 0 ; i<level; i++ )
        {
            for (  size_t j = 0 ; vectorAt(stNameSpace,i)[j] ; j++ )
            {
                if ( k>=MAXBUFFER-1 ) break ;
                temp[k++]=vectorAt(stNameSpace,i)[j];
            }
        }
        
    if (id!=NULL)
        for (  size_t j = 0 ; wcslen(id) ; j++ )
        {
            if ( k>=MAXBUFFER-1 ) break ;
            temp[k++]=id[j];
        }
    
    temp[k]=0;
    
    //if ( g.fDebug ) fwprintf ( pFileOutputST , L" found [%ls].\n",temp );

    return gcWcsDup(temp) ;
}

// .............................................. show map

void stShowMap(void)
{
    if ( g.fDebug ) // visualizza anche se ci sono errori, meglio.
    {
        if ( !whmapSize(mapST) )
        {
            fwprintf ( pFileOutputST , L"symbol Table [%p] size[%d] -> empty.\n ",mapST,(int) whmapSize(mapST) );
        }
        else
        {
            fwprintf ( pFileOutputST , L"\n§§§ List symbol in table : \n" );
            
            for ( whmapIt_t* it=whmapBegin(mapST) ; it != whmapEnd(mapST) ; whmapNext(mapST,it) )  
            {
                fwprintf ( pFileOutputST , L"\n§§§ ((%-20ls,%p)) ", whmapData(it) , (void*)whmapFind(mapST, whmapData(it) ) );
            }
            fwprintf ( pFileOutputST,  L"\n");
        } ;
    } ;
}

// .............................................. find simbol in map

psymTable_t stFindIDinMap(wchar_t* id) 
{
    psymTable_t pret=NULL;
    
    // #1 per tutti i livelli da 0 a n del namespace + ID
    // cerca nella mappa
 
    size_t i=vectorSize(stNameSpace);
    
    do
    {
        i--;
        //fwprintf ( stdout , L"\nNAME SPACE::[%ls]",vectorAt(stNameSpace,i) ) ;
        //fwprintf ( stdout , L"\nFULL NAME ::[%ls]",stGet_nsid(i,id)     ) ;
        
        pret = whmapFind( mapST , stGet_nsid(i,id) ) ; // cerca il id nella mappa 
        
        if ( pret != NULL ) 
        {
			fwprintf ( pFileOutputST , L" found::[%ls-%018p] \n", pret->id,pret );
			return pret ; // ritorna indice se l'ha trovato
		}
        
    } while(i!=0);
 
    return pret ;
}

// .............................................. Make Sym Table and push in hasmap

psymTable_t stMakeSymTable(void)
{
    if ( g.fDebug ) fwprintf ( pFileOutputST , L"# symTable   -> stMakeSymTable ...\n" );
    
    psymTable_t pstNew   = NULL ; // new node Symbol Table
    
    pstNew = gcMalloc ( sizeof(struct symTable_s) ) ;
    if ( pstNew==NULL ) $scannerInternal ( malloc , outOfMemory , L"symTable.c" , L"stMakeSymTable") ;

    pstNew->scope    = stScope ;
     pstNew->ns        = stGet_nsid(0,NULL) ;
    pstNew->id         = NULL ;
    
    pstNew->kind     = stKindNull ;
    
    pstNew->type     = stTypeNull ;
    pstNew->typeID  = NULL ;
    
    pstNew->array    = NULL ;

    vectorNew( pstNew->member     , 8 ) ;
    vectorNew( pstNew->offset     , 8 ) ;
    
    pstNew->size     = 0 ;

    pstNew->value.integer  = 0 ; // default value
    
    return pstNew ;
}

// .............................................. Debug Sym Table (node)

void stDebugSymTableNode(psymTable_t pst)
{
    if ( g.fDebug ) 
    {
        // ................................................................................................. POINTER PST
        
        fwprintf ( pFileOutputST , L"\n# DEBUG ST [%p] ->\n",pst );
        
        // ................................................................................................. SCOPE
        
        fwprintf ( pFileOutputST , L"# symTable->scope   [%d] :: "                ,pst->scope );
        switch ( pst->scope )
        {
            case stScopeGlobal :  fwprintf ( pFileOutputST , L"[Global]"); break ;
            case stScopeLocal  :  fwprintf ( pFileOutputST , L"[Local]"); break ;
            default : fwprintf ( pFileOutputST , L"[??]"); break ;
        }
        fwprintf ( pFileOutputST , L"\n");
        
        // ................................................................................................. NS
        
        fwprintf ( pFileOutputST , L"# symTable->ns      [%ls]\n"                ,pst->ns      );
        
        // ................................................................................................. ID
        
        fwprintf ( pFileOutputST , L"# symTable->id      [%ls]\n"                ,pst->id      );
        
        // ................................................................................................. KIND
        
        fwprintf ( pFileOutputST , L"# symTable->kind    [%d] :: "                 ,pst->kind   );
        switch ( pst->kind )
        {
            case stKindNull        : fwprintf ( pFileOutputST , L"[Null]");       break ;
            case stKindConst       : fwprintf ( pFileOutputST , L"[Const]");      break ;
            case stKindVar         : fwprintf ( pFileOutputST , L"[Var]");        break ;
            case stKindArray       : fwprintf ( pFileOutputST , L"[Array]");      break ;
            case stKindStruct      : fwprintf ( pFileOutputST , L"[Struct]");     break ;
            case stKindFunction    : fwprintf ( pFileOutputST , L"[function]");   break ;
            default : fwprintf ( pFileOutputST , L"[??]"); break ;
        }
        fwprintf ( pFileOutputST , L"\n");

        // ................................................................................................. TYPE
        
        fwprintf ( pFileOutputST , L"# symTable->type    [%d] :: "                 ,pst->type  );
        switch ( pst->type )
        {
            case stTypeNull          : fwprintf ( pFileOutputST , L"[stTypeNull]"          ); break ;
            case stTypeInteger       : fwprintf ( pFileOutputST , L"[stTypeInteger]"       ); break ;
            case stTypeReal          : fwprintf ( pFileOutputST , L"[stTypeReal]"          ); break ;
            case stTypeChar          : fwprintf ( pFileOutputST , L"[stTypeChar]"          ); break ;
            case stTypeByte          : fwprintf ( pFileOutputST , L"[stTypeByte]"          ); break ;
            case stTypeStruct        : fwprintf ( pFileOutputST , L"[stTypeStruct]"        ); break ; 
            case stTypeConstString   : fwprintf ( pFileOutputST , L"[stTypeConstString]"   ); break ;

            default: fwprintf ( pFileOutputST , L"[??]"    );     break ;
        } ;
        
        // ................................................................................................. ARRAY

        if ( pst->array != NULL )
        {
            size_t nDim = pst->array->arrayDim.ndx.size ;
            fwprintf ( pFileOutputST , L"\n# symTable->nDim    [%d] / "                 ,(int) nDim  );
            for(size_t i=0;i<nDim;i++) fwprintf ( pFileOutputST , L"[%d]" ,pst->array->arrayDim.ndx.data[i]->term.integer  );
        }
        else
         {
            fwprintf ( pFileOutputST , L"\n# symTable->nDim    [0] / "  );
        }

        // ................................................................................................. STRUCT
        
        fwprintf ( pFileOutputST , L"\n# symTable->member  [%d] / " ,(int)vectorSize(pst->member)  );
        for(size_t i=0;i<vectorSize(pst->member);i++) fwprintf ( pFileOutputST , L"[%p]" ,((void*)vectorAt(pst->member,i))  );
        
        // ................................................................................................. SIZE
        
        fwprintf ( pFileOutputST , L"\n# symTable->size    [%d]\n"                 ,(int)pst->size  );
        
        //fwprintf ( pFileOutputST , L"# symTable->address [%p]\n"                 ,pst->address  );
        fwprintf ( pFileOutputST , L"# symTable->typeID  [%ls]\n"                 ,(pst->typeID == NULL) ? L"{NULL}" : pst->typeID );
        
        // ................................................................................................. CONST VALUE
        
        fwprintf ( pFileOutputST , L"# symTable->value   "  );
        switch ( pst->type )
        {
            case stTypeNull         : fwprintf ( pFileOutputST , L"[stTypeNull]"                        ); break ;
            case stTypeInteger      : fwprintf ( pFileOutputST , L"[%ld]"        , pst->value.integer   ); break ;
            case stTypeReal         : fwprintf ( pFileOutputST , L"[%lf]"        , pst->value.real      ); break ;
            case stTypeStruct       : 
            {
            fwprintf ( pFileOutputST , L"[MEMBER]::"    );
            const size_t kVectorSize = vectorSize ( pst->member ) ;

            for ( uint32_t i = 0 ; i < kVectorSize ; i++ ) // ........................................... member & offset
            {
                fwprintf ( pFileOutputST , L" {%ls/%d} " , pst->member.data[i],(int)pst->offset.data[i] ) ;
            }
            break ;  
            }
            case stTypeConstString   : fwprintf ( pFileOutputST , L"[%ls]"       , g.outputSpecialCharInString(pst->value.wstring)    ); break ;
            case stTypeChar          : fwprintf ( pFileOutputST , L"[%lc]"       , g.outputSpecialCharInChar(pst->value.wchar)        ); break ;
            case stTypeByte          : fwprintf ( pFileOutputST , L"[%u]"        , pst->value.byte                                    ); break ;
            default:
                fwprintf ( pFileOutputST , L"[??]"    ); 
                break ;
        } ;

        fwprintf ( pFileOutputST , L"\n\n");
    }
}

// .............................................. st get size of symbol 

size_t stGetSize( pnode_t node )
{
    psymTable_t    pstTemp = NULL ;

    size_t size = 0 ;
    
    switch ( node->type )
    {
        case nTypeDeclVar :
            pstTemp = stFindIDinMap(node->declVar.id) ;
            break;
            
        case nTypeDeclArray :
            pstTemp = stFindIDinMap(node->declArray.id) ;
            break;
            
        default:
            fwprintf(stderr, L"\n?? internal error :: stGetSize -> switch ( node->type ) = unknown.\n");
            exit(-1);
            break;
    }
    
    if ( pstTemp != NULL ) size = pstTemp->size ;    
    return size ;
}

#undef $CD



/**/


