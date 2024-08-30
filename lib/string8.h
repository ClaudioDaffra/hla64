#ifndef mstring8
#define mstring8

#include "cxx.h"

//.................................................... [] string(TYPE) 

// stringTypeDef(TYPE,ID)

/*
 
    string_ID_d            :    TYPE
    ID_d                   :    TYPE
    string_ID_s            :    struct string_ID_s
    string_ID_t            :    string_ID_s
    string_ID_p            :    string_ID_t*
*/

#define stringTypeDef(TYPE,ID)                          \
typedef TYPE MERGE ( MERGE ( string_ , ID )  , _d );    \
typedef TYPE MERGE ( ID  , _d );                        \
typedef struct MERGE ( MERGE ( string_ , ID )  , _s )   \
{                                                       \
    union {                                             \
    TYPE*   data ;                                      \
    TYPE*   text ;                                      \
    } ;                                                 \
    size_t  size        ;                               \
    size_t  capacity    ;                               \
}  MERGE ( MERGE ( string_ , ID )  , _t ) ;             \
typedef MERGE ( MERGE ( string_ , ID )  , _t ) * MERGE ( MERGE ( string_ , ID )  , _p )

// stringType(TYPE,ID) :  generic string

#define stringType(TYPE)    stringTypeDef(TYPE,TYPE)

// ........................................................... [] stringAlloc(ID,N) 
 
#define stringAlloc(ID,N)                                   \
(ID).data = (void*) gcMalloc ( sizeof((ID).data) * N );     \
assert((ID).data!=NULL);                                    \
(ID).size      = 0;                                         \
(ID).capacity  = N;    
  
// ........................................................... [] SIZE

#define stringSize(ID) ((ID).size)

// ........................................................... [] CAPACITY

#define stringCapacity(ID) ((ID).capacity)

// ........................................................... [] EMPTY

#define stringEmpty(ID) ((ID).size == 0) 

// ........................................................... [] PUSH_BACK

#define stringPushBack(ID, VAL) do {                                         \
    if ((ID).size + 1 >= (ID).capacity) {                                    \
        size_t N = ((ID).capacity += (ID).capacity);                         \
        (ID).data = gcRealloc  ( (ID).data   , (N) * sizeof((ID).data)  );   \
        (ID).capacity = (N);                                                 \
    } ;                                                                      \
    size_t len=stringSize(ID);                                               \
    (ID).data[len]   = (VAL);                                                \
    (ID).data[len+1] = 0    ;                                                \
    ++(ID).size ;                                                            \
} while (0)

// ........................................................... [] POP_BACK

#define stringPopBack(ID) do {              \
    if ((ID).size) {                        \
        (ID).data[stringSize(ID)-1] = 0;    \
        --(ID).size ;                       \
    };\
} while (0)

// ........................................................... [] CLEAR

#define stringClear(ID) do {        \
    (ID).size = 0       ;           \
    (ID).data[0] = 0    ;           \
} while (0)

// ........................................................... [] DATA

#define stringData(ID) (ID).data

// ........................................................... [] AT 

#define stringAt(ID, INDEX) (ID).data[INDEX]    

/*
// ........................................................... [] stringOfstring ... X , XY , XYZ
    
#define stringX(ID,NDX1)                stringAt(ID,NDX1)
#define stringXY(ID,NDX1,NDX2)          stringAt(stringAt(ID,NDX1),NDX2)
#define stringXYZ(ID,NDX1,NDX2,NDX3)    stringAt(stringAt(stringAt(ID,NDX1),NDX2),NDX3)

*/
// ........................................................... [] FRONT

#define stringFront(ID) (ID).data[0]

// ........................................................... [] BACK 

#define stringBack(ID) (ID).data[stringSize(ID) - 1]

// ........................................................... [] ITERATOR 

#define itString(ID)        MERGE( ID  , _d )*  
 
#define stringBegin(ID)     &(ID).data[0]
#define stringEnd(ID)       &(ID).data[(ID).size] 

#define stringRBegin(ID)    &(ID).data[(ID).size-1]
#define stringREnd(ID)      &(ID).data[0]

// ........................................................... [] SHRINK TO FIT

#define stringShrinkToFit(ID) do {                                                  \
    (ID).data = gcRealloc  ( (ID).data   , ((ID).size) * sizeof((ID).data)  );      \
    (ID).capacity = (ID).size;                                                      \
} while (0)    

// ........................................................... [] RESERVE 

#define stringReserve(ID, N) do {                                                   \
    if ((ID).capacity < (N)) {                                                      \
        (ID).data = gcRealloc  ( (ID).data   , (N) * sizeof((ID).data)  );          \
        (ID).capacity = (N);                                                        \
    } \
} while (0)

// ........................................................... [] stringInsertAtVal     
        
#define stringInsertAtVal(ID, POS, VAL) do {                                                \
    while ((ID).size + 1 > (ID).capacity) {                                                 \
        (ID).capacity *= 2;                                                                 \
        (ID).data = gcRealloc  ( (ID).data   , (ID).capacity * sizeof((ID).data)  );        \
    } ;                                                                                     \
    memmove((ID).data + POS + 1, (ID).data + POS, ((ID).size - POS) * sizeof((ID).data) );  \
    ++(ID).size;                                                                            \
    (ID).data[POS] = VAL;                                                                   \
} while (0)

// ........................................................... [] ERASE at
                
#define stringEraseAt(ID, POS) do {                                                             \
    if ( (ID).size ) {                                                                          \
    memmove((ID).data + POS, (ID).data + POS + 1    , ((ID).size - POS+1) * sizeof *(ID).data); \
    (ID).size -= 1;                                                                             \
    } ;                                                                                         \
} while (0)

// ........................................................... [] ERASE N
        
#define stringEraseAtN(ID, POS, N) do {                                                         \
    if ( ((ID).size-(N))>0 ) {                                                                  \
    memmove((ID).data + POS, (ID).data + POS + (N)  , ((ID).size - POS+N) * sizeof *(ID).data );\
    (ID).size -= (N);                                                                           \
    }                                                                                           \
} while (0)

// ........................................................... [] RESIZE
    
#define stringResize(ID, N, VAL) do {                                       \
    if ((N) > (ID).capacity)                                                \
        (ID).data = gcRealloc ( (ID).data ,  (N) * sizeof((ID).data)   );   \
    if ( (ID).size<(N) )                                                    \
        for (int i = (ID).size; i < (N); ++i) (ID).data[i] = (VAL);         \
    (ID).size = (N);                                                        \
    (ID).data[N]=0;\
} while (0)

// ........................................................... [] COPY V1 <- V2 
 
#define stringCopy(ID, PTR ) do {                                                               \
    while ((ID).size + (PTR).size > (ID).capacity) {                                            \
        (ID).capacity *= 2;                                                                     \
        (ID).data = gcRealloc ( (ID).data , (ID).capacity * sizeof((ID).data)   );              \
    } ;                                                                                         \
    strcpy( (ID).data , (PTR).data ) ;                                                          \
    (ID).size = (PTR).size;                                                                     \
} while (0)

// ........................................................... [] APPEND 
 
#define stringAppend(ID, V2 ) do {                                                      \
    size_t V1z = (ID).size ;                                                            \
    size_t V2z = (V2).size ;                                                            \
    if ((ID).capacity < (V1z+V2z)) {                                                    \
        (ID).data = gcRealloc ( (ID).data , (V1z+V2z+1 ) *  sizeof((ID).data) ) ;       \
        (ID).capacity = V1z + V2z +1;                                                   \
    } ;                                                                                 \
    strcat((ID).data,(V2).data);                                                        \
    (ID).size = (V1z + V2z) ;                                                           \
} while (0)

// ........................................................... [] insert string at

#define stringInsertAtString(ID, POS, PTR ) do {                                                        \
    while ((ID).size + (PTR).size > (ID).capacity) {                                                    \
        (ID).capacity *= 2;                                                                             \
        (ID).data = gcRealloc ( (ID).data , (ID).capacity *  sizeof((ID).data)   );                     \
    }                                                                                                   \
    memmove((ID).data + POS + (PTR).size , (ID).data + POS, ((ID).size - POS + 1) * sizeof *(ID).data); \
    for (size_t i = 0; i < (PTR).size; i++)                                                             \
        (ID).data[POS + i] = (PTR).data[0 + i];                                                         \
    (ID).size += (PTR).size;                                                                            \
} while (0)

// ........................................................... [] stringInsertAtStringFromN( v1, POS1, v2 , POS2 , N )

#define stringInsertAtStringFromN(ID, POS, PTR , POS2 , N) do {                                     \
    while ((ID).size + (N) > (ID).capacity) {                                                       \
        (ID).capacity *= 2;                                                                         \
        (ID).data = gcRealloc ( (ID).data , (ID).capacity *  sizeof((ID).data)  );                  \
    } ;                                                                                             \
    memmove((ID).data + POS + (N), (ID).data + POS, ((ID).size - POS) * sizeof *(ID).data);         \
    for (size_t i = 0; i < (N); i++)                                                                \
        (ID).data[POS + i] = (PTR).data[POS2 + i];                                                  \
    (ID).size += (N);                                                                               \
    (ID).data[(ID).size]=0;\
} while (0)

// ........................................................... [] FREE 

#define stringDealloc(ID) do {                                              \
    if ( (ID).data != NULL ) { gcFree( (ID).data );   (ID).data=NULL;  }    \
} while(0)

/*   

// ........................................................... [] string ALLLOC string 

#define stringAllocstring(TYPE,ID,N)                        \
(ID).data = (void*) gcMalloc ( sizeof(TYPE) * N );          \
assert((ID).data!=NULL);                                    \
(ID).size      = 0;                                         \
(ID).capacity  = N;    
*/

#define stringCheckCapacity(TYPE,ID,LEN)\
    if (LEN>=(ID).capacity)\
    {\
        while (LEN > (ID).capacity) (ID).capacity *= 2;\
       (ID).data = (TYPE*) gcRealloc (  (ID).data, (ID).capacity *  sizeof(TYPE)   );\
    };
    
#define stringFromS8(ID,STR)\
do{\
    size_t len=strlen(STR);\
    stringCheckCapacity(char,ID,len);\
    strcpy ( (ID).data , STR );\
    (ID).size = len;\
}while(0);

#define stringFromWS(ID,WSTR)\
do{\
    char* STR=cnvWStoS8(WSTR);\
    size_t len=strlen(STR);\
    stringCheckCapacity(char,ID,len);\
    strcpy ( (ID).data , STR );\
    (ID).size = len;\
}while(0);

#define stringLen(ID)   strlen((ID).data)
#define mbstringLen(ID) strmblen((ID).data)

#define stringFormat(ID,FORMAT,...)\
do{\
char* strTemp=NULL;\
strTemp=s8Format_( FORMAT , __VA_ARGS__ );\
size_t len=strlen(strTemp);\
strcpy((ID).data,strTemp);\
(ID).size=len;\
(ID).capacity=len;\
}while(0);

#define stringCompare(ID1,ID2)  ( (  (ID1).size!=(ID2).size ) ? 1 : strcmp((ID1).data,(ID2).data) )

#define stringCompareN(ID1,ID2,N)  strncmp( (ID1).data,(ID2).data ,N )



#endif

 
 
/**/
