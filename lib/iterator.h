#ifndef cdIterator
#define cdIterator

#include "cxx.h"

#define iterator(ID)        ID##_d*
#define itNext(IT)          (IT++)
#define itPrev(IT)          (IT--) 
#define itAdvance(IT,N)     (IT+=N)
#define itRetreat(IT,N)     (IT-=N)

#define itDistance(P1,P2)   (    (ptrdiff_t)    (    (char*)P2 - (char*)P1)        )

#endif
