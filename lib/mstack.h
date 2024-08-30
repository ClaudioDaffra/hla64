#ifndef mstack
#define mstack

//.................................................... [] stack(TYPE) 

// stackTypeDef(TYPE,ID)

/*
    stack_ID_d            :   TYPE
    ID_d                  :   TYPE
    stack_ID_s            :   struct stack_ID_s
    stack_ID_t            :   stack_ID_s
    stack_ID_p            :   stack_ID_t*
*/

#define stackTypeDef(TYPE,ID)                          \
typedef TYPE MERGE ( MERGE ( stack_ , ID )  , _d );    \
typedef TYPE MERGE ( ID  , _d );                       \
typedef struct MERGE ( MERGE ( stack_ , ID )  , _s )   \
{                                                      \
    TYPE*   data ;                                     \
    size_t  size        ;                              \
    size_t  capacity    ;                              \
}  MERGE ( MERGE ( stack_ , ID )  , _t ) ;             \
typedef MERGE ( MERGE ( stack_ , ID )  , _t ) * MERGE ( MERGE ( stack_ , ID )  , _p )

// stackType(TYPE,ID) :  generic stack

#define stackType(TYPE)    stackTypeDef(TYPE,TYPE)

// ........................................................... [] stackAlloc(ID,N) 
 
#define stackAlloc(ID,N)                                    \
(ID).data = (void*) gcMalloc ( sizeof((ID).data) * N );     \
assert((ID).data!=NULL);                                    \
(ID).size      = 0;                                         \
(ID).capacity  = N;    
  
// ........................................................... [] SIZE

#define stackSize(ID) ((ID).size)

// ........................................................... [] CAPACITY

#define stackCapacity(ID) ((ID).capacity)

// ........................................................... [] EMPTY

#define stackEmpty(ID) ((ID).size == 0) 

// ........................................................... [] PUSH

#define stackPush(ID, VAL) do {                                             \
    if ((ID).size + 1 > (ID).capacity) {                                    \
        size_t N = ((ID).capacity += (ID).capacity);                        \
        (ID).data = gcRealloc  ( (ID).data   , (N) * sizeof((ID).data)  );  \
        (ID).capacity = (N);                                                \
    } ;                                                                     \
    (ID).data[stackSize(ID)] = (VAL);                                       \
    ++(ID).size ;                                                           \
} while (0)

// ........................................................... [] POP

#define stackPop(ID) do {  \
    if ((ID).size) --(ID).size; \
} while (0)

// ........................................................... [] PRINTF

#define stackPrintf(FORMAT,ID) do {                                                 \
for ( size_t i = 0 ; i < (ID).size ; i++)  printf ( FORMAT , (ID).data[i] ) ;       \
}while(0);

// ........................................................... [] FREE 

#define stackDealloc(ID) do {                        \
    if ( (ID).data != NULL ) gcFree( (ID).data );    \
} while(0)
   
// ........................................................... [] TOP 

#define stackTop(ID) (ID).data[stackSize(ID) - 1]
 

#endif

 
 
/**/
