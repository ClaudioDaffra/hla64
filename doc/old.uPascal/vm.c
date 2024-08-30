#include "vm.h"

extern asmOpCodeTab_t asmOpCodeTab[] ;

// vr = virtual register (used as stack)

// *******
// UTILITY
// *******

void vmPrintResult( typeOp_t result )
{
  if ( result.typeOp == typeOpInteger )
  {
      #ifdef _MSC_VER
      fwprintf ( stdout,L"\n\nRESULT [%lld].",result.integer ) ;
      #else
      fwprintf ( stdout,L"\n\nRESULT [%ld].",result.integer ) ;
      #endif
  }
  if ( result.typeOp == typeOpReal )
  {
      fwprintf ( stdout,L"\n\nRESULT [%lf].",result.real ) ;
  }
  if ( result.typeOp == typeOpPointer )
  {
      #ifdef _MSC_VER
      fwprintf ( stdout,L"\n\nRESULT [%p].",(void*)result.pointer ) ;
      #else
      fwprintf ( stdout,L"\n\nRESULT [%x].",result.pointer ) ;
      #endif
  }
  fwprintf ( stdout,L"\n" ) ;
}

// ***************
// Virtual Machine
// ***************

typeOp_t vmRun(void)
{
  // init first time stack
  static int fVM=0;
  if (fVM==0) {
    fVM=1;
    stackNew( vmStack , 128 ) ;
  }
  
 if ( fDebug ) fwprintf ( pFileOutputVM , L"\n# VM   -> running ...\n" );
	
  typeOp_t tempValue ; // unione contenete i valori ammessi dagli operandi
  vmPC = 0 ;

  if ( !vectorSize(vInstr) ) // check if vIstr has 1 istruction at least.
  {
      $VMInternal
      ( running , errUnknown , L"vm.c" , L"vectorSize(vInstr) == 0") ;
  }
  else
  do {

    if (fDebug)
    {
        #ifdef _MSC_VER
        fwprintf ( pFileOutputVM, L"\nPC [%04I32d] TOP [%04zd] OP [%04I32d] := "
                  ,vmPC
                  ,stackSize(vmStack)
                  ,vectorAt(vInstr,vmPC)->opcode
        );
        #else
        fwprintf ( pFileOutputVM, L"\nPC [%04d] TOP [%04d] OP [%04d] := "
                  ,vmPC
                  ,stackSize(vmStack)
                  ,vectorAt(vInstr,vmPC)->opcode
        );            
        #endif
    
        if ( stackSize(vmStack) )
        {
          #ifdef _MSC_VER
            switch ( stackTop(vmStack).typeOp  )
            {
                case typeOpNull    : fwprintf ( pFileOutputVM, L"typeOpNull")										; break ;
                case typeOpInteger : fwprintf ( pFileOutputVM, L"integer[%lld]"	,stackTop(vmStack).integer) 		; break ;
                case typeOpReal    : fwprintf ( pFileOutputVM, L"real[%lf]"    	,stackTop(vmStack).real)    		; break ;
                case typeOpID 	   : fwprintf ( pFileOutputVM, L"ID[%ls]"  		,(void*)stackTop(vmStack).id) 		; break ;
                case typeOpPointer : fwprintf ( pFileOutputVM, L"pointer[%p]"  	,(void*)stackTop(vmStack).pointer) 	; break ;
            };
          #else
            switch ( stackTop(vmStack).typeOp  )
            {
                case typeOpNull    : fwprintf ( pFileOutputVM, L"typeOpNull")										; break ;
                case typeOpInteger : fwprintf ( pFileOutputVM, L"integer[%ld]"  ,stackTop(vmStack).integer) 		; break ;
                case typeOpReal    : fwprintf ( pFileOutputVM, L"real[%lf]"     ,stackTop(vmStack).real)    		; break ;
                case typeOpID 	   : fwprintf ( pFileOutputVM, L"ID[%ls]" 		,(void*)stackTop(vmStack).id) 		; break ;               
                case typeOpPointer : fwprintf ( pFileOutputVM, L"pointer[%x]"  	,stackTop(vmStack).pointer) 		; break ;
            };
          #endif
        }
    } ;

    size_t k = 0 ;
    
    
    switch ( vectorAt(vInstr,vmPC)->opcode )
    {
      case op_nop :

            //if (fDebug) fwprintf ( pFileOutputVM,L"[%-10ls]",$opcode(op_nop) ) ;

          break ;

      case op_load_cInt :  // .................................................................... [ load immediate ]

              tempValue.integer = vectorAt(vInstr,vmPC)->lhs.integer ;
              tempValue.typeOp = typeOpInteger ;
              stackPush(vmStack,tempValue)  ;
              break ;

      case op_load_cReal :

              tempValue.real = vectorAt(vInstr,vmPC)->lhs.real ;
              tempValue.typeOp = typeOpReal ;
              stackPush(vmStack,tempValue)  ;
              break ;


	  case op_store_rInt2m : // ................................................................. [ store reg INT  in mem ]

			  *(int64_t*)vectorAt(vInstr,vmPC)->lhs.address = stackTop(vmStack).integer ;
			  fwprintf ( pFileOutputVM, L" -> *address int  [[%ld]]"		,*(int64_t*)vectorAt(vInstr,vmPC)->lhs.address 	) ;
			  //stackPop(vmStack);

			  break ;
	  
	  case op_store_rReal2m : // ................................................................ [ store reg REAL in mem ]
	  
			  *(double*)vectorAt(vInstr,vmPC)->lhs.address  = stackTop(vmStack).real ;
			  fwprintf ( pFileOutputVM, L" -> *address real [[%lf]]",*(double*)vectorAt(vInstr,vmPC)->lhs.address ) ;
			  //stackPop(vmStack);
			  
			  break ;
//			  
			  
	  case op_load_rInt_m : // ................................................................. [ load reg INT  from mem ]
	  
              tempValue.integer = *(int64_t*)vectorAt(vInstr,vmPC)->lhs.address ;
              tempValue.typeOp = typeOpInteger ;
              stackPush(vmStack,tempValue)  ;			  
			  fwprintf ( pFileOutputVM, L" -> integer  [[%ld]]"		, tempValue.integer 		) ;
			  
			  break ;
	  
	  case op_load_rReal_m : // ................................................................ [ load reg REAL from mem ]
	  
              tempValue.real = *(double*)vectorAt(vInstr,vmPC)->lhs.address ;
              tempValue.typeOp = typeOpReal ;
              stackPush(vmStack,tempValue)  ;			  
			  fwprintf ( pFileOutputVM, L" -> real  	[[%lf]]"	, tempValue.real 			) ;
			  			  			  
			  break ;			  
//	  

      case op_cnv2rl:  // .................................................................... [ conv to real ]

              k = stackSize(vmStack) - 1 ;
              (vmStack).data[ k ].real = (double) (vmStack).data[ k ].integer ;
              (vmStack).data[ k ].typeOp = typeOpReal ;
              break ;

      case op_cnv2rr:

              k = stackSize(vmStack) - 2 ;
              (vmStack).data[ k ].real = (double) (vmStack).data[ k ].integer ;
              (vmStack).data[ k ].typeOp = typeOpReal ;
              break ;

      case op_cnv2il:  // .................................................................... [ conv to integer ]

              k = stackSize(vmStack) - 1 ;
              (vmStack).data[ k ].integer = (int64_t) (vmStack).data[ k ].real ;
              (vmStack).data[ k ].typeOp = typeOpInteger ;
              break ;

      case op_cnv2ir:

              k = stackSize(vmStack) - 2 ;
              (vmStack).data[ k ].integer = (int64_t) (vmStack).data[ k ].real ;
              (vmStack).data[ k ].typeOp = typeOpInteger ;
              break ;

      case op_fmul: // .................................................................... [ * ]

              k = stackSize(vmStack) - 1 ;
              (vmStack).data[ k-1 ].real *= (vmStack).data[ k ].real ;
              (vmStack).data[ k-1 ].typeOp = typeOpReal ;
              stackPop(vmStack);
              break ;

       case op_mul:

              k = stackSize(vmStack) - 1 ;
              (vmStack).data[ k-1 ].integer *= (vmStack).data[ k ].integer ;
              (vmStack).data[ k-1 ].typeOp = typeOpInteger ;
              stackPop(vmStack);
              break ;

      case op_fdiv: // .................................................................... [ / ]

              k = stackSize(vmStack) - 1 ;

              if ( (vmStack).data[ k ].real == 0.0 )
              {
                $VMInternal( running , division_by_zero , L"vm.c" , L"case : op_fdiv") ; 
                break;  
              }
                           
              (vmStack).data[ k-1 ].real /= (vmStack).data[ k ].real ;
              (vmStack).data[ k-1 ].typeOp = typeOpReal ;
              stackPop(vmStack);
              break ;

       case op_div:

              k = stackSize(vmStack) - 1 ;

              if ( (vmStack).data[ k ].integer == 0 )
              {
                $VMInternal( running , division_by_zero , L"vm.c" , L"case : op_div") ; 
                break;  
              }

              (vmStack).data[ k-1 ].integer /= (vmStack).data[ k ].integer ;
              (vmStack).data[ k-1 ].typeOp = typeOpInteger ;
              stackPop(vmStack);
              break ;

      case op_fmod: // .................................................................... [ % ]

              k = stackSize(vmStack) - 1 ;

              if ( (vmStack).data[ k ].real == 0.0)
              {
                $VMInternal( running , division_by_zero , L"vm.c" , L"case : op_fmod") ; 
                break;  
              }
                           
              (vmStack).data[ k-1 ].real = fmod ( (vmStack).data[ k-1 ].real , (vmStack).data[ k ].real ) ;
              (vmStack).data[ k-1 ].typeOp = typeOpReal ;
              stackPop(vmStack);
              break ;

       case op_mod:

              k = stackSize(vmStack) - 1 ;

              if ( (vmStack).data[ k ].integer == 0 )
              {
                $VMInternal( running , division_by_zero , L"vm.c" , L"case : op_mod") ; 
                break;  
              }
                           
              (vmStack).data[ k-1 ].integer %= (vmStack).data[ k ].integer ;
              (vmStack).data[ k-1 ].typeOp = typeOpInteger ;
              stackPop(vmStack);
              break ;

      case op_fadd: // .................................................................... [ + ]

              k = stackSize(vmStack) - 1 ;    
              (vmStack).data[ k-1 ].real += (vmStack).data[ k ].real ;
              (vmStack).data[ k-1 ].typeOp = typeOpReal ;     
              stackPop(vmStack);
              break ;

       case op_add:

              k = stackSize(vmStack) - 1 ;
              (vmStack).data[ k-1 ].integer += (vmStack).data[ k ].integer ;
              (vmStack).data[ k-1 ].typeOp = typeOpInteger ;
              stackPop(vmStack);
              break ;

      case op_fsub: // .................................................................... [ - ]

              k = stackSize(vmStack) - 1 ;    
              (vmStack).data[ k-1 ].real -= (vmStack).data[ k ].real ;
              (vmStack).data[ k-1 ].typeOp = typeOpReal ;     
              stackPop(vmStack);
              break ;

       case op_sub:

              k = stackSize(vmStack) - 1 ;
              (vmStack).data[ k-1 ].integer -= (vmStack).data[ k ].integer ;
              (vmStack).data[ k-1 ].typeOp = typeOpInteger ;
              stackPop(vmStack);
              break ;

      case op_fnot: // .................................................................... [ ! ]

              k = stackSize(vmStack) - 1 ;    
              (vmStack).data[ k ].real = ! (vmStack).data[ k ].real ;
              (vmStack).data[ k ].typeOp = typeOpReal ;     
            
              break ;

       case op_not:

              k = stackSize(vmStack) - 1 ;
              (vmStack).data[ k ].integer = ! (vmStack).data[ k ].integer ;
              (vmStack).data[ k ].typeOp = typeOpInteger ;

              break ;
              
      case op_fneg: // .................................................................... [ - prefix ]

              k = stackSize(vmStack) - 1 ;    
              (vmStack).data[ k ].real = -1.0 * (vmStack).data[ k ].real ;
              (vmStack).data[ k ].typeOp = typeOpReal ;     
            
              break ;

       case op_neg:

              k = stackSize(vmStack) - 1 ;
              (vmStack).data[ k ].integer = -1 * (vmStack).data[ k ].integer ;
              (vmStack).data[ k ].typeOp = typeOpInteger ;

              break ;    
                      
/* not implemented yet

       case op_pushrl: // .................................................................... [ pushrl ]

              break ;

       case op_pushil: // .................................................................... [ pushrr ]

              break ;

*/
      default:

              $VMInternal
              ( running , errUnknown , L"vm.c" , L"typeOp_t VM(void -> switch ( vectorAt(vInstr,vmPC)->opcode )") ;

          break ;
    }

    ++vmPC ;

  } while ( vmPC < vectorSize(vInstr) && !kError ) ; // continua fino alla fine e che non vi siano errori

  //

  if (fDebug) fwprintf ( pFileOutputVM, L"\n" ) ;

  return stackTop( vmStack ) ;

}
