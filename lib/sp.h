
#ifndef CD_SMART_POINTER
#define CD_SMART_POINTER


struct smartPointer_s
{
	union{
		void*  const  	ptr	;
		void*  		 	var	;	
	};
} ;
 
typedef struct smartPointer_s * const smartPointer_t ;
 
struct smartPointer_s *  gcLocalSmartPointer( gc_t *gc , void* unsafePtr ) ;

#define sp(TYPE,PTR) 	 *(TYPE*)PTR->ptr
#define sptr(TYPE,PTR)   ((TYPE*)PTR->var)

#define gcSmartPointerMove(p1,p2)\
do{\
	gcFree(p1->var);\
	p1->var = p2->var;\
}while(0);

#define sptrData(TYPE,ID) ((TYPE*)ID->ptr)->data

#define smartPointerTypeDef(TYPE)\
struct MERGE(MERGE(smartPointer_,TYPE),_s)\
{\
	union{\
		TYPE*  const  	ptr	;\
		TYPE*  		 	var	;\
	};\
} ;\
typedef struct smartPointer_s * const MERGE(MERGE(smartPointer_,TYPE),_t) ;


#define spBegin(GCCD) \
{\
	gc_t *GCCD ;\
	GCCD=gcLocalStart();

#define spEnd(GCCD) \
	gcLocalStop(GCCD);\
};

#define smartPointerLocalMove(gc,p0,cd,p1)	\
			p0->var = p1->var ;\
			gcLocalPop(cd,p1->var);\
			gcLocalPush(gc , p1->var, 0 ) ;
			

#define smartPointerMove(p0,gcLocal,p1)	\
			p0->var = p1->var ;\
			gcLocalPop(gcLocal,p1->var);\
			gcLocalPush(gc,p1->var, 0 ) ;
			
//

#endif

