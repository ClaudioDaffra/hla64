#include "cxx.h"


//*******************  
//
// CD GARBAGE COLLECTOR
//
//*******************

gc_t *gc ;

//******************* gcc Constructor / Destructor

#ifdef __GNUC__
void gcCTOR() 
{
	gcStart();
}
void gcDTOR() 
{
	gcStop();
}
#endif

//******************* vc Constructor / Destructor
#ifdef _MSC_VER
INITIALIZER( initialize)
{
	gcStart();
    atexit( finalize);
}

static void finalize(void)
{
	gcStop();
}
#endif


//*******************

// initialize a hash gc structure 

short int gcMapInit    (
    gcMap_t* gc                                              , 
    size_t              (* gcHashFN)   (void* ptr)           , 
    unsigned char       (* gcPtrCMP)   (void* p1, void* p2)  , 
    size_t    start_len                                      , 
    float                load_factor
)
{
    gc->table = calloc(start_len, sizeof(gcMapNode_t*));
    
    if(!gc->table) return GC_ERR_ALLOC;
    
    gc->gcHashFN        = gcHashFN;
    gc->gcPtrCMP        = gcPtrCMP;
    gc->size            = start_len;
    gc->load_factor     = load_factor;
    gc->count           = 0;

    return 0;
}

// add a ptr-dtor pair to a hash gc, resizing and rehashing if necessary 

short int gcMapPut( gcMap_t* gc, void* ptr, gcDtorCallBack_t dtor, unsigned short int flags)
{
    gcMapNode_t *n_node,* s_node;
    size_t      node_idx    ,
                node_hash   ;

    // perform resize and rehash if necessary 
    if(((float)(++ gc->count)) / gc->size > gc->load_factor)
    {
        size_t i;

        size_t n_len = gc->size << 1;
        gcMapNode_t** temp = calloc(n_len, sizeof(gcMapNode_t*));
        if(!temp)
        {
            gc->count --;
            return GC_ERR_ALLOC;
        }

        // for each element in the table 
        for(i = 0; i < gc->size; i ++)
        {
            // traverse down the linked list 
            gcMapNode_t     * current   ,
                            * next      ;

            // guard against empty elements 
            current = gc->table[i];
            while(current)
            {
            size_t npos;

            // prepare lookahead pointer 
			
            next = current->next;

            // rehash and copy each item 


            npos = (gc->gcHashFN(current->ptr)) % n_len;

            current->next = temp[npos];
            
            temp[npos] = current;

            // advance to next list element 
            current = next;
            }
        }

        free(gc->table);
        gc->table = temp;
        gc->size = n_len;
    }

    // check whether item is already in gc
    node_hash = (*gc->gcHashFN)(ptr);
    node_idx = node_hash % gc->size;
    s_node = gc->table[node_idx];
    while(s_node)
    {
        if(flags & GC_FAST ? ((gc->gcHashFN)(s_node->ptr) == node_hash) : ((gc->gcPtrCMP)(s_node->ptr, ptr)))
        {
            gc->count --;

            // deallocate existing dtor if necessary
            // update dtor and return if found
			
            s_node->dtor = dtor;
            return 0;
        }
        
        s_node = s_node->next;
    }

    // create a new gcMapNode_t to hold data 
    n_node = malloc(sizeof(gcMapNode_t));
    if(!n_node)
    {
        gc->count --;
        return GC_ERR_ALLOC;
    }
    n_node->ptr = ptr;
    n_node->dtor = dtor;
    n_node->next = gc->table[node_idx];

    // add new gcMapNode_t to table 
    gc->table[node_idx] = n_node;

    return 0;
}

// get the dtor associated with a ptr returns NULL if ptr does not exist in gc
 
gcDtorCallBack_t* gcMapGet( gcMap_t* gc, void* ptr, unsigned short int flags)
{
    gcMapNode_t* current;
    size_t k_hash;

    k_hash = (gc->gcHashFN)(ptr);
    current = gc->table[k_hash % gc->size];
    while(current)
    {
        if(flags & GC_FAST ? (k_hash == (gc->gcHashFN)(current->ptr)) : ((gc->gcPtrCMP(ptr, current->ptr))))
        {
            return &current->dtor;
        }

        current = current->next;
    }

    return NULL;
}

// .............................................. hash gc at

void* gcMapAtAddress( gcMap_t* gc, void* ptr, unsigned short int flags)
{
    gcMapNode_t*    current ;
    size_t          k_hash  ;

    k_hash = (gc->gcHashFN)(ptr);
    current = gc->table[k_hash % gc->size];
    while(current)
    {
        if(flags & GC_FAST ? (k_hash == (gc->gcHashFN)(current->ptr)) : ((gc->gcPtrCMP(ptr, current->ptr))))
        {
        return &(current->dtor);
        }
        current = current->next;
    }

    return NULL;
}

// removes an item from the hash gc if present (else no-op)

short int gcMapDrop( gcMap_t* gc, void* ptr, unsigned short int flags)
{
    gcMapNode_t     * current       ,
                    * parent = NULL ;
    
    size_t  k_hash  ,
            idx     ;

    k_hash = (gc->gcHashFN)(ptr);
    idx = k_hash % gc->size;

    current = gc->table[idx];
    while(current)
    {
        if(flags & GC_FAST ? ((gc->gcHashFN)(current->ptr) == k_hash) : ((gc->gcPtrCMP)(ptr, current->ptr)))
        {
            // redirect the gcMapNode_t chain around it, then destroy 
            if(parent)
                parent->next = current->next;
            else
                gc->table[idx] = current->next;
            
			// ptr can be freed from calling code; would be too messy to add that here  
            
            free(current);             // nodes are always malloc'ed  

            // perform resize and rehash if necessary  
            if((gc->size > 10) && (-- gc->count) / (gc->size << 1) < gc->load_factor)
            {
                size_t i;

                size_t n_len = gc->size >> 1;
                gcMapNode_t** temp = calloc(n_len, sizeof(gcMapNode_t*));
                if(!temp)
                {
                    gc->count --;
                    return GC_ERR_ALLOC;
                }

                // for each element in the table 
                for(i = 0; i < gc->size; i ++)
                {
                    // traverse down the linked list 
                    gcMapNode_t * current   ,
                                * next      ;

                    // guard against empty elements  
                    current = gc->table[i];
                    while(current)
                    {
                        size_t npos;

                        // prepare lookahead pointer 
                        next = current->next;

                        // rehash and copy each item  
                        npos = (gc->gcHashFN(current->ptr)) % n_len;
                        current->next = temp[npos];
                        temp[npos] = current;

                        // advance to next list element  
                        current = next;
                    }
                }

                free(gc->table) ;
                gc->table = temp;
                gc->size = n_len;
            }

            // stop looking since gcMapNode_t was found  
            return 0;
        }

        parent = current;
        current = current->next;
    }

    return GC_NODE_NOTFOUND;
}

// destroys a hash gc (does not touch pointed data) 

void gcMapDestroy( gcMap_t* gc, unsigned short int flags1,unsigned short int flags2 )
{
    (void)flags1;
    (void)flags2;
    
    size_t i;
    gcMapNode_t* current,* next;

    // goes down each list, deleting all nodes  
    for(i = 0; i < gc->size; i++)
    {
        current = gc->table[i];
        while(current)
        {
            next = current->next;

            //if(flags1==GC_DESTROY) //free(current->ptr);
            //if(flags2==GC_DESTROY) //free(current->dtor);

            free(current);
            
            current = next;
        }
    }

    // deletes the table  
    free(gc->table);
}


// .............................................. gc gc hash clear

void gcLocalDestroy( gcMap_t* gc, unsigned short int flags)
{
    size_t i;
    gcMapNode_t    * current,* next;

    // goes down each list, deleting all nodes
    
    for(i = 0; i < gc->size; i++)
    {
        current = gc->table[i];
        while(current)
        {
            next = current->next;
            if(flags & GC_DESTROY)
            {

                gcDtorCallBack_t *cb = (gcDtorCallBack_t*) &current->dtor ;
/*
                gcMap_DEBUG_PRINTF ( "gcLocalDestroy free ptr %p dtor %p %p\n"
                                    ,current->ptr,current->dtor , *cb );
*/
                (*cb)(current->ptr);
            }
            free(current);
            current = next;
        }
    }

    // deletes the table 
    free(gc->table);
}

// simple "hash" function (uses pointer as hash code) 

size_t gcDefaultHashFN(void* ptr)
{
    uintptr_t ad = (uintptr_t) ptr;
    return (size_t) ((13*ad) ^ (ad >> 15));
}

// ......................................................    gcMap simple equality checker hash gc

unsigned char  gcDefaultPtrCMP(void* p1, void* p2)
{
    return p1 == p2;
}
/*
unsigned char  gcMap_string_hash_key_comp(void* p1, void* p2)
{
    return !strcmp((char*)p1, (char*)p2);
}

unsigned char  gcMap_stringw_key_comp(void* p1, void* p2)
{
    return !wcscmp((wchar_t*)p1, (wchar_t*)p2);
}
*/
// ......................................................    GC simple equality checker  qsort

int gcCompareStrC ( const void * a, const void * b ) 
{
    const char **pa = (const char **)a;
    const char **pb = (const char **)b;
    return strcmp(*pa, *pb);   
    
}
int gcCompareWStrC ( const void * a, const void * b ) 
{
    const wchar_t **pa = (const wchar_t **)a;
    const wchar_t **pb = (const wchar_t **)b;
    return wcscmp(*pa, *pb);  
}
int gcComparepStrC ( const void * a, const void * b ) 
{
    const char *pa = (const char *)a;
    const char *pb = (const char *)b;
    return strcmp(pa, pb);   
    
}
int gcComparepWStrC ( const void * a, const void * b ) 
{
    const wchar_t *pa = (const wchar_t *)a;
    const wchar_t *pb = (const wchar_t *)b;
    return wcscmp(pa, pb);  
}
/*
int gc_int_key_comp (const void * a, const void * b)
{
  if ( *(int*)a <  *(int*)b ) return -1;
  if ( *(int*)a == *(int*)b ) return 0;
  return 1;
}
*/
int gcCompareInt(const void* a, const void* b)
{
  int va = *(const int*) a;
  int vb = *(const int*) b;
  return (va > vb) - (va < vb);
}
/*
int gcCompare_uint64_t(const uint64_t* a, const uint64_t* b)
{
  return (a > b) - (a < b);
}
*/
int gcCompareFloat (const void * a, const void * b)
{
  float fa = *(const float*) a;
  float fb = *(const float*) b;
  return (fa > fb) - (fa < fb);
}

int gcCompareDouble (const void * a, const void * b)
{
  double fa = *(const double*) a;
  double fb = *(const double*) b;
  return (fa > fb) - (fa < fb);
}

int gcCompareFloatAsInt (const void * a, const void * b)
{
  float _fa = *(const float*) a;
  float _fb = *(const float*) b;
  int fa = (int)_fa;
  int fb = (int)_fb;
  return (fa > fb) - (fa < fb);
}
int gcCompareDoubleAsInt (const void * a, const void * b)
{
  double _fa = *(const double*) a;
  double _fb = *(const double*) b;
  int fa = (int)_fa;
  int fb = (int)_fb;  
  return (fa > fb) - (fa < fb);
}
// ....................................................................... gcallback gcFreeDtor -> FREE

void gcFreeDtor( void * ptr )
{
    if (ptr!=NULL) free(ptr) ;
}

// ....................................................................... gcallback gcFCloseDtor

void gcFCloseDtor( void * ptr )
{
    FILE* fptr = (FILE*)ptr;
    
    if (ptr!=NULL) fclose(fptr);
}

// ....................................................................... gcLocalStart

void* gcLocalStart(void)
{
    gc_t *gc = ( gcMap_t*)malloc ( sizeof( gcMap_t) ) ;
    
    gcMapInit ( gc , gcDefaultHashFN , gcDefaultPtrCMP , GC_DEFAULT_LEN , GC_DEFAULT_LOAD_FACTOR ) ;
    
    return gc ;
}

// ....................................................................... gcLocalMalloc

void* gcLocalMalloc(gc_t *gc , size_t SIZE )
{
    if ( SIZE==0 ) return NULL ;

    assert(SIZE>0);
    
    void* ptr= (void*) malloc ( SIZE );
    
    gcMapPut(   gc, ptr , gcFreeDtor, GC_FAST );
    
   // gcMap_DEBUG_PRINTF ( "gcMalloc ptr==%p , dtor==%p \n",ptr,gcFreeDtor ) ;
    
    return ptr ;
}

// ....................................................................... gcLocalPush

void* gcLocalPush(gc_t *gc , void* ptr, size_t SIZE )
{
    (void)SIZE;
    
    gcMapPut( gc, ptr , gcFreeDtor, GC_FAST );
    
    //gcMap_DEBUG_PRINTF ( "gcPush ptr==%p , dtor==%p \n",ptr,gcFreeDtor ) ;
    
    return ptr ;
}

// ....................................................................... gcLocalOpen

FILE* gcLocalFileOpen(gc_t *gc , const char* fileName , const char* flag)
{
    FILE* ptr= fopen ( fileName , flag ) ;
    
    gcMapPut(   gc, (void*)ptr , gcFCloseDtor, GC_FAST );
    
    //gcMap_DEBUG_PRINTF ( "gcOpen ptr==%p , dtor==%p \n",ptr,gcFCloseDtor ) ;
    
    return ptr ;
}

// ....................................................................... gcLocalFree

void* gcLocalFree(gc_t *gc , void *ptr)
{
    if (ptr==NULL)
    {
		//gcMap_DEBUG_PRINTF ( "gcFree ptr==NULL \n") ;
		
		return NULL ;
    } ;
    // recupera il distruttore
   gcDtorCallBack_t *dtor = gcMapGet ( gc , ptr , GC_FAST );
   
   // chiama il distrutore
   if (dtor!=NULL)
   {
        //gcDtorCallBack_t *cb = (gcDtorCallBack_t*) &dtor ;

        (*dtor)(ptr);

        //gcMap_DEBUG_PRINTF ( "gcFree ptr==%p , dtor==%p %p \n",ptr,dtor,*cb ) ;
   }
   else
   {
        //gcMap_DEBUG_PRINTF ( "gcFree dtor==NULL \n") ;
        
        return NULL ;  
   }
   
   // libera lo spazio
   gcMapDrop(   gc, ptr ,  GC_FAST );
   
   return ptr;
}

// ....................................................................... gcLocalPop

void* gcLocalPop(gc_t *gc , void *ptr)
{
    if (ptr==NULL)
    {
        //gcMap_DEBUG_PRINTF ( "gcPop ptr==NULL \n") ;
        return NULL ;
    } ;
    
    // recupera il distruttore
   void *dtor = gcMapGet ( gc , ptr , GC_FAST );
   
   // chiama il distrutore
   if (dtor!=NULL)
   {
       gcDtorCallBack_t *cb = (gcDtorCallBack_t*) &dtor ;

        *cb=*cb;
        
       //gcMap_DEBUG_PRINTF ( "gcPop ptr==%p , dtor==%p %p \n",ptr,dtor,*cb ) ;
   }
   else
   {
        //gcMap_DEBUG_PRINTF ( "gcPop dtor==NULL \n") ;
        
        return NULL ;  
   }
   // libera lo spazio
   gcMapDrop( gc, ptr ,  GC_FAST );
   
   return ptr ;
}

// ....................................................................... gcLocalStop

void* gcLocalStop(gc_t *gc )
{
    gcLocalDestroy(gc,GC_DESTROY);
    
    if ( gc!=NULL) free(gc);
    
    return gc=NULL;
}

// ....................................................................... gcLocalRealloc

void* gcLocalRealloc( gc_t *gc, void* ptr , size_t SIZE )
{
    //assert(SIZE!=0); //  realloc(array, 0) is not equivalent to free(array). 
    if (SIZE==0)
    {
        gcFree(ptr);
        return ptr=NULL;
    }
    
    assert(SIZE>0);
    
    if (ptr==NULL) return gcLocalMalloc(gc,SIZE);

    void* old=ptr;
    
    ptr = (void*) realloc ( ptr,SIZE ) ;

    if (ptr!=NULL)
    {
        gcLocalPop ( gc,old ) ;

        gcLocalPush ( gc,ptr,SIZE ) ;
       
        assert ( ptr != NULL ) ;
    }
    
    return ptr ;
}

// ....................................................................... gcMapNew

void* gcMapNew(void)
{
    gc_t *gc = ( gcMap_t*)malloc ( sizeof( gcMap_t) ) ;
    
    assert(gc!=NULL);
    
    gcMapInit ( gc , gcDefaultHashFN , gcDefaultPtrCMP , GC_DEFAULT_LEN , GC_DEFAULT_LOAD_FACTOR ) ;
    
    return gc ;
}

// ....................................................................... gcMapDelete

void gcMapDelete( gcMap_t *gcMap )
{
    gcMapDestroy(gcMap,GC_NODESTROY,GC_NODESTROY);
    
    if (gcMap!=NULL) free( gcMap );
}

void* gcMapFind( gcMap_t *gcMap , void* ptr )
{
    return gcMapGet( gcMap , ptr , GC_FAST );
}

// ....................................................................... gc Dup

char* gcLocalStrDup( gc_t* gc, char* str)
{
    char* strnew = strdup(str);

    gcMapPut(  gc, (void*)strnew , gcFreeDtor, GC_FAST );
    
    return strnew ;
}

wchar_t* gcLocalWcsDup( gc_t* gc,wchar_t* str)
{
    wchar_t* strnew=wcsdup(str);

    gcMapPut(   gc, (void*)strnew , gcFreeDtor, GC_FAST );
    
    return strnew ;
}

// ................................................................... intDup

int* gcIntLocalDup( gc_t* gc,int val)
{
    int* p=(int*)  gcMalloc ( sizeof(int) );
    
    *p=val;
    
    return p ;
}

// ................................................................... doubleDup

double* gcLocalDoubleDup( gc_t* gc, double val )  
{
    double* p = (double*) gcMalloc ( sizeof(double) );
    
    *p=val;
    
    return p ;
}


/**/