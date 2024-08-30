
#include "cxx.h"


// ....................................................................... gcLocalSmartPointerMalloc

struct smartPointer_s *  gcLocalSmartPointer( gc_t *gc , void* unsafePtr )
{
    if ( unsafePtr==0 ) return NULL ;

	struct smartPointer_s *  sp = (struct smartPointer_s*  ) gcLocalMalloc ( gc,sizeof(struct smartPointer_s) );
	
	if (sp != NULL )
	{
		sp->var = unsafePtr ;
	}
	else
	{
		sp->var = NULL; ;
	}

    return  sp ;
}