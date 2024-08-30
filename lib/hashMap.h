#ifndef CD_HASH_MAP
#define CD_HASH_MAP

// https://github.com/peterr-s/hash_map

#include <stdlib.h>
#include <string.h>
 
#define HM_ERR_ALLOC 1
#define HM_NOT_FOUND 2

#define HM_DEFAULT_LOAD_FACTOR 0.75
#define HM_DEFAULT_LEN 10

#define HM_NORMAL 0
#define HM_FAST 1
#define HM_DESTROY 2

union hmData
{
    unsigned char       u8;
    signed   char       s8;
    unsigned short int  u16;
    signed   short int  s16;
    unsigned int        u32;
    signed   int        s32;
    unsigned long int   u64;
    signed   long int   s64;
    float               f32;
    double              f64;
    long double         f80;
    void*               pvoid;
    char*               pchar;
    size_t              usize;
    //ssize_t             ssize;
} ;

extern union hmData hmValue ;

typedef struct hmNode_s hmNode_t;

struct hmNode_s
{
	void* key;
	void* value;
	hmNode_t* next;
};

typedef struct hash_map_s hashMap_t;

struct hash_map_s
{
	hmNode_t**  table                           ;
	size_t      (* hmHashFN)(void* key)         ;
	int         (* hmEqFN)(void* p1, void* p2)  ;
	size_t      table_len                       ;
	float       load_factor                     ;
	size_t      element_ct                      ;
};

short int hashMapInit
(
    hashMap_t* map, 
    size_t (*hmHashFN)(void* key), 
    int(* hmEqFN)(void* p1, void* p2), 
    size_t start_len, 
    float load_factor
);

void                hashMapDestroy  (hashMap_t* map, unsigned short int flags);

short int           hashMapPut      (hashMap_t* map, void* key, void* value, unsigned short int flags);
void*               hashMapGet      (hashMap_t* map, void* key, unsigned short int flags);
short int           hashMapDrop     (hashMap_t* map, void* key, unsigned short int flags);

size_t              hmPtrHash       (void* key);
int                 hmPtrEq         (void* p1, void* p2);

size_t              hmStringHash    (void* key);
int                 hmStringEq      (void* p1, void* p2);
 
 // -------------------------------------

#define hmNew(HM)                   hashMap_t *HM = gcMalloc( sizeof(hashMap_t) ); 

#define hmInitString(HM)            hashMapInit( HM , hmStringHash, hmStringEq, HM_DEFAULT_LEN, HM_DEFAULT_LOAD_FACTOR); 
#define hmPutString(HM,KEY,VALUE)   hashMapPut ( HM, (void*)(KEY) , (void*)VALUE , HM_NORMAL );
#define hmGetString(HM,KEY)         hashMapGet ( HM, (void*)(KEY) , HM_NORMAL );
#define hmDropString(HM,KEY)        hashMapDrop( HM, (void*)(KEY) , HM_NORMAL );

#define hmInitPtr(HM)            hashMapInit( HM , hmPtrHash, hmPtrEq, HM_DEFAULT_LEN, HM_DEFAULT_LOAD_FACTOR); 
#define hmPutPtr(HM,KEY,VALUE)   hashMapPut ( HM, (void*)(KEY) , (void*)VALUE , HM_FAST );
#define hmGetPtr(HM,KEY)         hashMapGet ( HM, (void*)(KEY) , HM_FAST );
#define hmDropPtr(HM,KEY)        hashMapDrop( HM, (void*)(KEY) , HM_FAST );

#define hmDestroy(HM)               hashMapDestroy(HM,HM_NORMAL);


 // -------------------------------------
 
#endif