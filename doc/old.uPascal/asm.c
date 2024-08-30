
#include "asm.h"

//  tabella opcode e wchar_t

asmOpCodeTab_t asmOpCodeTab[] =
{
  { L"NOP"                  } ,
  { L"MOVI cI -> vr+"       } ,
  { L"MOVR cR -> vr+"       } ,
  { L"CNV2RL left"          } ,
  { L"CNV2RR right"         } ,
  { L"FADD"                 } ,
  { L"FSUB"                 } ,
  { L"FMUL"                 } ,
  { L"FDIV"                 } ,
  { L"FMOd"                 } ,
  { L"ADD"                  } ,
  { L"SUB"                  } ,
  { L"MUL"                  } ,
  { L"DIV"                  } ,
  { L"MOD"                  } ,
  { L"PUSHIL"               } ,
  { L"PUSHRL"               } ,
  { L"NOT"                  } ,
  { L"FNOT"                 } ,
  { L"NEG"                  } ,
  { L"FNEG"                 } , 
  { L"CNV2IL left"          } ,
  { L"CNV2IR right"         } , 
  { L"MOVI vr -> &"     	} ,
  { L"MOVR vr -> &"      	} , 
  { L"MOVI vr <- &"     	} ,
  { L"MOVR vr <- &"      	} ,        
} ;

// puntatore temporaneo allocazione istruzioni

pAsmInstruction_t p = NULL;

//

// make instruction default parameter

pAsmInstruction_t makeInstr(void)
{
  pAsmInstruction_t temp = gcMalloc( sizeof(struct asmInstruction_s) ) ;

  if ( temp == NULL )
  {
    $asmInternal ( malloc , outOfMemory , L"asm.c" , L"pAsmInstruction_t makeInstr(void)") ;
  }
  else
  {
    temp->opcode        = op_nop  ;
    temp->label         = 0       ;
    temp->lhs.typeOp    = typeOpNull   ;
    temp->lhs.pointer   = NULL    ;
    temp->rhs.typeOp    = typeOpNull   ;
    temp->rhs.pointer   = NULL    ;
  }

  return temp ;
}

// visualizza il vettore di istruzioni

void asmPrintVectorInstr(void)
{
   fwprintf ( pFileOutputAsm, L"\n\n# asmPrintVectorInstr\n" ) ;

    if ( !vectorSize(vInstr) )
    {
      fwprintf ( pFileOutputAsm, L"-> [ empty ]." ) ;
    }
    else
    if ( fDebug )
    {
      itVector(vInstr) itv ;

      for( itv = vectorBegin(vInstr) ; itv != vectorEnd(vInstr); itv++ )
      {
        fwprintf ( pFileOutputAsm , L"\n" ) ;

        if ( (*itv)->label != 0 ) fwprintf ( pFileOutputAsm ,L"[%04d]:: ",(*itv)->label ) ;

        switch ( (*itv)->opcode )
        {
          case op_nop :

                fwprintf ( pFileOutputAsm ,L"%-20ls",$opcode(op_nop)  ) ;

                break ;

          case op_load_cInt :

                #ifdef _MSC_VER
                fwprintf ( pFileOutputAsm ,L"%-20ls %lld",$opcode(op_load_cInt),(*itv)->lhs.integer ) ;
                #else
                fwprintf ( pFileOutputAsm ,L"%-20ls %ld" ,$opcode(op_load_cInt),(*itv)->lhs.integer ) ;
                #endif
                break ;

          case op_load_cReal :

                fwprintf ( pFileOutputAsm ,L"%-20ls %lf",$opcode(op_load_cReal),(*itv)->lhs.real ) ;
                break ;

          case op_cnv2rl :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_cnv2rl),L"(vr-1)" ) ;
                break ;

          case op_cnv2rr :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_cnv2rr),L"(vr)" ) ;
                break ;

          case op_cnv2il :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_cnv2il),L"(vr-1)" ) ;
                break ;

          case op_cnv2ir :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_cnv2ir),L"(vr)" ) ;
                break ;
                
          // reali

          case op_fadd :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_fadd),L"vr- , vr" ) ;
                break ;

          case op_fsub :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_fsub),L"vr- , vr" ) ;
                break ;

          case op_fmul :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_fmul),L"vr- , vr" ) ;
                break ;

          case op_fdiv :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_fdiv),L"vr- , vr" ) ;
                break ;

          case op_fmod :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_fmod),L"vr- , vr" ) ;
                break ;

          // interi

          case op_add :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_add),L"vr- , vr" ) ;
                break ;

          case op_sub :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_sub),L"vr- , vr" ) ;
                break ;

          case op_mul :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_mul),L"vr- , vr" ) ;
                break ;

          case op_div :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_div),L"vr- , vr" ) ;
                break ;

          case op_mod :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_mod),L"vr- , vr" ) ;
                break ;

          case op_pushil :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_pushil),L"vr- " ) ;
                break ;

          case op_pushrl :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_pushrl),L"vr- " ) ;
                break ;

          case op_not :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_not),L"vr" ) ;
                break ;

          case op_fnot :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_fnot),L"vr" ) ;
                break ;

          case op_neg :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_neg),L"vr" ) ;
                break ;

          case op_fneg :

                fwprintf ( pFileOutputAsm ,L"%-20ls %-20ls",$opcode(op_fneg),L"vr" ) ;
                break ;
                
          case op_store_rInt2m : 

                fwprintf ( pFileOutputAsm ,L"%ls%-08ls %018p"
                ,$opcode(op_store_rInt2m),(*itv)->lhs.id,(*itv)->lhs.address ) ;
                break ;

          case op_store_rReal2m : 

                fwprintf ( pFileOutputAsm ,L"%ls%-08ls %018p"
                ,$opcode(op_store_rReal2m),(*itv)->lhs.id,(*itv)->lhs.address ) ;
                break ;


          case op_load_rInt_m : 

                fwprintf ( pFileOutputAsm ,L"%ls%-08ls %018p"
                ,$opcode(op_load_rInt_m),(*itv)->lhs.id,(*itv)->lhs.address ) ;
                break ;

          case op_load_rReal_m : 

                fwprintf ( pFileOutputAsm ,L"%ls%-08ls %018p"
                ,$opcode(op_load_rReal_m),(*itv)->lhs.id,(*itv)->lhs.address ) ;
                break ;

          //

          default:

                $asmInternal
                ( debug , errUnknown , L"asm.c" , L"void printVectorInstr(void) -> switch ( (*itv)->opcode )") ;

                break ;
        }
      }
      fwprintf ( pFileOutputAsm , L"\n" ) ;
    }
}

//***********
// pushStackTermType
//***********

void pushStackTermType( eTypeOp_t typeOp )
{
  ptypeOp_t pt = gcMalloc ( sizeof ( typeOp_t ) ) ;

  if ( pt == NULL )
  {
    $asmInternal ( malloc , outOfMemory , L"asm.c" , L"pushStackTermType -> gcMalloc ( sizeof ( typeOp_t )") ;
  }
  else
  {
    switch ( typeOp )
    {
        case typeOpNull :
            $asmInternal ( assembling , notImplemetedYet , L"asm.c" , L"pushStackTermType -> switch ( typeOp ) -> case typeOpNull") ;
            break ;

        case typeOpInteger :

            pt->typeOp  = typeOpInteger ;
            break;

        case typeOpReal :

            pt->typeOp  = typeOpReal ;
            break;

        case typeOpPointer :
            $asmInternal ( assembling , notImplemetedYet , L"asm.c" , L"pushStackTermType -> switch ( typeOp ) -> case typeOpPointer") ;
            break;

       default :
            $asmInternal ( debug , errUnknown , L"asm.c" , L"pushStackTermType -> switch ( typeOp )") ;
            break ;

    }
  }
  stackPush ( sTypeOp , pt ) ;
}

//***********
// $makeIstrShort
//***********

#define $makeIstrShort( ISTR )\
      p = makeInstr();\
      if (p!=NULL)\
      {\
        p->opcode      = ISTR;\
        vectorPushBack (  vInstr , p ) ;\
      }

//***********
// $makeIstrLong
//***********

#define $makeIstrLong( ISTR , ID , PMEM )\
      p = makeInstr();\
      if (p!=NULL)\
      {\
        p->opcode      = ISTR;\
        p->lhs.id      = gcWcsDup(ID);\
        p->lhs.address = PMEM;\
        vectorPushBack (  vInstr , p ) ;\
      }
      
//***********
// makeConvTypeOp 
//***********

eTypeOp_t makeConvTypeOp ( ptypeOp_t leftType , ptypeOp_t rightType )
{

	if ( leftType==NULL || rightType==NULL )
	{
		$asmInternal ( debug , errUnknown , L"asm.c" , L"asm.c :: makeConvTypeOp : -> ( leftType==NULL || rightType==NULL )") ;
		return typeOpNull ;
	}

	eTypeOp_t  fTypeOp=typeOpNull;

	if ( ( leftType->typeOp  == typeOpInteger) && ( rightType->typeOp  == typeOpReal   ) )
	{
		$makeIstrShort(op_cnv2rl) ;
		fTypeOp=typeOpReal;
	}
	if ( ( rightType->typeOp  == typeOpInteger) && ( leftType->typeOp  == typeOpReal   ) )
	{
		$makeIstrShort(op_cnv2rr) ;
		fTypeOp=typeOpReal;
	}
	if ( ( leftType->typeOp  == typeOpInteger) && ( rightType->typeOp  == typeOpInteger)  ) fTypeOp=typeOpInteger ;
	if ( ( leftType->typeOp  == typeOpReal   ) && ( rightType->typeOp  == typeOpReal   )  ) fTypeOp=typeOpReal    ;

	return fTypeOp ;
	
}

//***********
// $stackPopOpType
//***********

#define $stackPopOpType( OPTYPE )\
            ptypeOp_t OPTYPE ;\
            OPTYPE = stackTop( sTypeOp );\
            if ( stackSize(sTypeOp)>1) stackPop( sTypeOp );

//***********
//
// astAsm
//
//***********

node_t* astAsm(node_t* n)
{
  static int fastAsm=0;

  if ( fastAsm==0 )
  {
      fastAsm=1;
      // SHOW : node Asm
      if ( fDebug ) fwprintf ( pFileOutputAsm,L"\n# asm    -> Assembling ...\n"  );
      // init vector instruction
      vectorNew( vInstr , 128 ) ;
      // init stack typeOp
      stackNew ( sTypeOp  , 128 ) ;
  }

  //

  if ( n == NULL ) return NULL  ;

  switch ( n->type )
  {
	// ---------------------------------------------------------------------------------------- case  nTypeUndefined	  
      case  nTypeUndefined :

            if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeUndefined"  );
            break;
	// ---------------------------------------------------------------------------------------- case  nTypeTermInteger
      case  nTypeTermInteger :

            if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeTermInteger"  );
            p = makeInstr();
            if (p!=NULL)
            {
              p->opcode      = op_load_cInt ; // EMIT LOAD INTEGER
              p->lhs.integer = n->term.integer ;
              vectorPushBack (  vInstr , p ) ;

              pushStackTermType( typeOpInteger ) ;
              //
            }

            break;
	// ---------------------------------------------------------------------------------------- case  nTypeTermReal
      case  nTypeTermReal :

            if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeTermReal"  );
            p = makeInstr();
            if (p!=NULL)
            {
              p->opcode    = op_load_cReal ; // EMIT LOAD REAL
              p->lhs.real  = n->term.real ;
              vectorPushBack (  vInstr , p ) ;

              pushStackTermType( typeOpReal ) ;
              //
            }
            break;
	  // ---------------------------------------------------------------------------------------- case  nTypeBinOp
      case  nTypeBinOp :  

            if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeBinOp"  );
            // go left and right and pop typeOp from stack

            astAsm( n->binOp.right ) ; // st1

            $stackPopOpType(rightType) ;

            astAsm( n->binOp.left  ) ; // st0

            $stackPopOpType(leftType) ;

            // conversion typeOp

            eTypeOp_t  fTypeOp=typeOpNull;

            fTypeOp = makeConvTypeOp( leftType , rightType ) ;

            pushStackTermType( fTypeOp ) ;

            // emit code

            switch ( fTypeOp )
            {
              case typeOpNull :

                    $asmInternal ( assembling , notImplemetedYet , L"asm.c" , L"astAsm -> switch ( fTypeOp ) -> case typeOpNull") ;
                    break ;

              case typeOpReal :

                    if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeBinOp -> typeOpReal"  );
                    switch ( n->binOp.sym )
                    {
                        case sym_add : $makeIstrShort(op_fadd) ; break ;
                         
                        case sym_sub : $makeIstrShort(op_fsub) ; break ;

                        case sym_mul : $makeIstrShort(op_fmul) ; break ;

                        case sym_div : $makeIstrShort(op_fdiv) ; break ;

                        case sym_mod : $makeIstrShort(op_fmod) ; break ;

                        default :
                              $asmInternal ( debug , errUnknown ,
                              L"asm.c" , L"node_t* astAsm(node_t* n) -> switch ( fTypeOp ) -> case typeOpReal : -> switch ( n->binOp.sym ") ;
                              break ;
                    }
                    break ;

              case typeOpInteger :

                    if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeBinOp -> typeOpInteger"  );
                    switch ( n->binOp.sym )
                    {
                        case sym_add : $makeIstrShort(op_add) ; break ;

                        case sym_sub : $makeIstrShort(op_sub) ; break ;

                        case sym_mul : $makeIstrShort(op_mul) ; break ;

                        case sym_div : $makeIstrShort(op_div) ; break ;

                        case sym_mod : $makeIstrShort(op_mod) ; break ;

                        default :
                              $asmInternal ( debug , errUnknown ,
                              L"asm.c" , L"node_t* astAsm(node_t* n) -> switch ( fTypeOp ) -> case typeOpInteger : -> switch ( n->binOp.sym ") ;
                              break ;
                    }
                    break;
                    
        default:

                    $asmInternal ( debug , errUnknown , L"asm.c" , L"node_t* astAsm(node_t* n) -> switch ( fTypeOp )") ;
                    break ;
            }

            if ( ( fTypeOp!=typeOpReal ) && ( fTypeOp!=typeOpInteger ) )
               $asmInternal ( debug , errUnknown , L"asm.c" , L"( fTypeOp!=typeOpReal ) && ( fTypeOp!=typeOpInteger )") ;

            break;
	// ---------------------------------------------------------------------------------------- case  nTypePrefix
      case  nTypePrefix :  

			if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypePrefix"  );
			
            astAsm( n->prefix.right  ) ;

            eTypeOp_t opType = stackTop( sTypeOp )->typeOp ;
               
            if (( opType != typeOpInteger )  &&  ( opType != typeOpReal ))
            {
               $asmInternal ( debug , errUnknown , L"asm.c" , 
               L"node_t* astAsm(node_t* n) -> case  nTypePrefix -> if (( opType != typeOpInteger )  &&  ( opType != typeOpReal ))") ;
               break ;
            }

            switch ( n->prefix.sym )
            {
                case sym_not  :
                  if ( opType == typeOpInteger )  { $makeIstrShort(op_not ) ; }
                  if ( opType == typeOpReal    )  { $makeIstrShort(op_fnot) ; }                 
                break ;

                case sym_sub  :  
                  if ( opType == typeOpInteger )  { $makeIstrShort(op_neg ) ; }
                  if ( opType == typeOpReal    )  { $makeIstrShort(op_fneg) ; }                
                break ;

                default:
                $asmInternal ( debug , errUnknown , L"asm.c" , L"node_t* astAsm(node_t* n) -> case  nTypePrefix") ;
                break;
            }
            break ;
    // ---------------------------------------------------------------------------------------- case  nTypeBlock       
	case  nTypeBlock :
	
			if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeBlock"  );
			for (size_t i = 0 ; i<n->programBlock.next.size; i++)
			{
				astAsm( n->programBlock.next.data[i] ) ; 
			}

			break; 
	// ---------------------------------------------------------------------------------------- case  nTypeDeclGlobalVar		
 	case  nTypeDeclGlobalVar :
 	
			if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeDeclGlobalVar"  );
			// emetti expr 
			
			astAsm( n->declGlobalVar.expr ) ; 

			// continua con var decl
			
			sym_t 		sym = n->declGlobalVar.sym 	;
			wchar_t* 	id 	= n->declGlobalVar.id 	;
			
			// #0 ST symbol Table
			
			// #1 cerca se il simbolo e' gia' presente
			psymTable_t pstTemp = stFindIDinMap(id); 
			if ( pstTemp ) 
			{
				$scannerErrorExtra ( assembling , duplicateSymbolName , id ) ;
				break ;
			}
			// #2 crea il simbolo e definiscilo come una variabile
			psymTable_t	pstNew 	= stMakeSymTable() ;  
			pstNew->kind 		= stKindVar ; 		 
			pstNew->id 			= gcWcsDup( id ) ;  
			
			// #2b definisci il tipo
			switch ( sym )
			{
				case sym_kw_integer : pstNew->type = stTypeInteger  ;	break ;
				case sym_kw_real 	: pstNew->type = stTypeReal 	;	break ;
				case sym_kw_string 	: pstNew->type = stTypeString 	;	break ;				
				default:
				    fwprintf(stdout,L"\n asm.c #2b case unkwown"); exit(-1);
				break;
			}
			
			// #3 altrimenti inserisci name space + id
			stDebugSymTableNode(pstNew) ;  
			whmapInsert( mapST, stGetFullName(pstNew->id)   , pstNew ); 	
			
			// #4 alloca memoria per la variabile
			assert( sizeof(double) == sizeof( int64_t) ) ;
			
			void* pmem = gcMalloc( sizeof(double) ) ;	// 8 byte
			pstNew->address = pmem ;
			
			{
				if (stackSize( sTypeOp )==0)
				{
					$asmInternal ( debug , errUnknown , L"asm.c" , L"case  nTypeDeclGlobalVar : -> (stackSize( sTypeOp )==0)") ;
					return NULL ;
				}
				else
				{
					 eTypeOp_t opType = stackTop( sTypeOp )->typeOp ;
					 switch ( sym )
					 {
						case sym_kw_integer : // se la variabile e' intera

							if ( opType == typeOpReal ) // e il valore sullo stack e' reale
							{
								$makeIstrShort(op_cnv2il) ; // convertilo ad intero
							}	

							$makeIstrLong(op_store_rInt2m, stGetFullName(id),pmem ) ; // ifine assegna l'intero alla variabile intera !!!

							break ;
						
						case sym_kw_real 	: // se la variabile e' di tipo reale
						
							if ( opType == typeOpInteger ) // e l-operatore in cima e' intero
							{
								$makeIstrShort(op_cnv2rl) ; // convertilo da intero a reale
							}	

							$makeIstrLong(op_store_rReal2m, stGetFullName(id),pmem ) ; // e assegnalo alla variabile

							break ;
						
						case sym_kw_string 	:
							$asmInternal ( assembling , notImplemetedYet , L"string" , id )  ;
							break ;	
										
						case sym_id 		:

							$asmInternalExtra ( assembling , notImplemetedYet , n->declGlobalVar.idType ) ;
							break ;
							
						default:
							$asmInternal ( debug , errUnknown , L"asm.c" , L"node_t* astAsm(node_t* n) -> case  nTypeDeclGlobalVar :") ; 
							break;
					 }
				 }
			}
			 

			break;   
	// ---------------------------------------------------------------------------------------- case  nTypeTermVar		
 	case  nTypeTermVar :	// ID semplice
 	{
			if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeTermVar"  );
			
			// #0 ST symbol Table

			// #1 cerca se il simbolo e' gia' presente
			psymTable_t pstTemp = stFindIDinMap(n->termVar.id); 

			if ( pstTemp==NULL ) 
			{
				$scannerErrorExtra ( assembling , undeclaredIdentifier , n->termVar.id ) ;
				break ;
			} 
			
			// ottieni l'indirizzo
			void* pMem = pstTemp->address ;
			
			// ottieni il tipo 
			switch ( pstTemp->type )
			{
				case stTypeInteger : 
				
					$makeIstrLong( op_load_rInt_m , stGetFullName(n->termVar.id) , pMem ); 	// load reg int form mem
					pushStackTermType( typeOpInteger ) ;				 					// push in stack term integer
					
				break ;
				
				case stTypeReal 	: 
				
					$makeIstrLong( op_load_rReal_m , stGetFullName(n->termVar.id) , pMem ); // load reg real form mem
					pushStackTermType( typeOpReal ) ;				 		 				// push in stack term integer	
					
				break ;
				
				//case stTypeString 	:  	;	break ;	
							
				default:
				
				    fwprintf(stdout,L"\n asm.c case  nTypeTermVar : case unkwown"); exit(-1);
				    
				break;
			}
	}			
 	break ;		
	// ---------------------------------------------------------------------------------------- case  nTypeAssign
	case  nTypeAssign :	// := 
	
		// risolve l'espressione RHS
		// e ritorna con push type
		
		astAsm( n->assign.rhs ) ;
		
		if ( fDebug ) fwprintf ( pFileOutputAsm,L"\nnTypeAssign"  );

		// check term var
		// chech (next) array ...
		if ( n->assign.lhs->type != nTypeTermVar )
		{
			$parserError ( assembling , LValueRequired ) ;
			return NULL ;
		}
		else
		{
			// #1 cerca se il simbolo e' gia' presente
			psymTable_t pstLHS = stFindIDinMap( n->assign.lhs->termVar.id ); 
		
			if ( pstLHS==NULL ) 
			{
				$scannerErrorExtra ( assembling , undeclaredIdentifier , n->assign.lhs->termVar.id ) ;
				break ;
			} 
			// #2 ottieni l'indirizzo
			void* pMem = pstLHS->address ;

			// #3 tipo di ritorno
			 eTypeOp_t opType = stackTop( sTypeOp )->typeOp ;
			 switch ( opType )
			 {
				case stTypeInteger :
					// se nello stack c'e' un intero e la variabile e' reale convertilo a reale
					if ( pstLHS->type == stTypeReal ) $makeIstrShort(op_cnv2rr) ; 
					$makeIstrLong( op_store_rInt2m , stGetFullName(n->assign.lhs->termVar.id) , pMem ); 	// store reg real to mem					
					break ;
				
				case stTypeReal :
					// se nello stack c-e' un reale e la variabile e' un intero convertilo a intero
					if ( pstLHS->type == stTypeInteger ) $makeIstrShort(op_cnv2ir) ; 
					$makeIstrLong( op_store_rReal2m , stGetFullName(n->assign.lhs->termVar.id) , pMem ); 	// store reg int to mem					
					break ;	
				break ;
				
				default:
					fwprintf(stdout,L"asm.c :: typeAssign :: switch(opType) :: default"); 
					exit(-1);
				break;
										
			}
			
			
		}
		break ;
	

			         
      default :

            $asmInternal ( debug , errUnknown , L"asm.c" , L"node_t* astAsm(node_t* n) -> switch ( n->typeOp )") ;
            break;
  }

  return NULL ;
}

// begin assemble and show assembler outout

node_t* astAssemble(node_t* n)
{
 
  astAsm(n);

  return n ;
}



/**/
