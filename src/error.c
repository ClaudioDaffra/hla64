
#include "error.h"

// ******************
// Error
// ******************

uint32_t kError     ;
uint32_t kWarning   ;

vector_errLog_t  vErrLog ;

// ................................................... sender

sender_t sender[] =
{
    { "loader"         } ,
    { "lexer"          } ,
    { "parser"         } ,
    { "AST"            } ,
    { "node"           } ,
    { "assembler"      } ,
    { "VM"             } ,
    { "scanner"        } ,
    { "preprocessor"   } ,
    { "compiler"   	   } ,
} ;

// ................................................... type

type_t type[] =
{
    { "! Info"       } ,
    { "Warning !"    } , 
    { "? Error"      } , 
    { "?? Critical"  } , 
    { "!! Fatal"     } ,
    { "## Internal"  } , 
} ;

// ................................................... action

action_t action[] =
{
    { "no Error"                 } ,
    { "no Action"                } ,
    { "check file exits"         } ,
    { "initializing"             } ,
    { "tokening"                 } ,
    { "tokening string"          } ,  
    { "tokening number"          } , 	
    { "parsing"                  } ,
    { "scanning"                 } ,	
    { "coding"                   } ,
} ;

// ................................................... errMessage

errMessage_t errMessage[] =
{
    { "no Error"                                                                       } ,
    { "file Not Found"                                                                 } ,
    { "no input files"                                                                 } ,
    { "invalid argument"                                                               } ,
    { "init Error"                                                                     } ,
    { "unexpected token"                                                               } ,
    { "out of range"                                                                   } ,
    { "end of file"                                                                    } ,
    { "multi line comment"                                                             } ,
    { "exponent has no digits"                                                         } ,
    { "missing terminating"                                                        	   } ,	
    { "invalid suffix"                                                        		   } ,
    { "syntax error"                                                        		   } ,	
    { "expected"                                                        		   	   } ,	
    { "lvalue required"                                   		   					   } ,	
    { "operator not allowed"                                   		   				   } ,	
    { "duplicate definition (no shadowing)"                       					   } ,
    { "invalid type argument of unary ‘*’"                     		   				   } ,
    { "lvalue required as left operand of assignment"           	   				   } ,	
    { "lvalue required as unary ‘&’ operand"           	   				   			   } ,	
    { "wrong type argument to unary minus"           	   				   			   } ,	
    { "not allowed multiple assign"           	   				   			   		   } ,	
    { "undeclared identifier"           	   				   			   		   	   } ,	
    { "called object is not a function or function pointer"           	  			   } ,
    { "nested block do not allow"           	  			  						   } ,
    { "expected expression before"													   } ,
    { "invalid UTF-8 sequence"													   	   } ,
    { "overflow"													   	   			   } ,
    { "undeclared $( type )"					   									   } ,
    { "ptr arithmetic not injected"					   					   		   	   } ,	
    { "too few arguments to function"					   					   		   } ,
    { "too many arguments to function"					   					   		   } ,
	
} ;

// ................................................... push err log

uint32_t pushErrLog
(
    e_sender_t          sender         ,
    e_type_t            type           ,
    e_action_t          action         ,
    e_errMessage_t      errMessage     ,
    uint32_t            rowTok         ,
    uint32_t            colTok         ,
    const char*         fileInput      ,
    const char*         extra        
)
{
    // se non è allocato essendo globale lo alloca la prima volta
    // condiviso da varie parti del compilaore
    
    errLog_t* err = gcMalloc( sizeof(errLog_t) );

    static int fErroLogInit=0; // vector Error Log
    
    if ( fErroLogInit==0 )
    {
        fErroLogInit    = 1 ;
        vectorAlloc( vErrLog , 16 );
        kError          = 0 ;
        kWarning        = 0 ; 
    }    
 
    if ( type == type_warning ) 
        ++kWarning ;
    else
        ++kError ;       

    err->sender          = sender    ;
    err->type            = type      ;
    err->action          = action    ;
    err->errMessage      = errMessage;
    err->rowTok          = rowTok    ;
    err->colTok          = colTok    ;
    err->fileInput       = gcStrDup((char*)fileInput) ;
    err->extra           = gcStrDup((char*)extra)     ;

    vectorPushBack( vErrLog ,err ) ; 

    return vectorSize(vErrLog);

}

// ................................................... print err log

int printErrLog(void)
{
    if ( vectorSize(vErrLog) )
    {
        fprintf ( stderr ,"\nError Log :: %d. \n",(int)vectorSize(vErrLog) ) ;
    }

    if ( !vectorSize(vErrLog) ) return 0 ;

    itVector(errLog) itv ;

    for( itv = vectorBegin(vErrLog) ; itv != vectorEnd(vErrLog); itv++ )
    {
        if ((*itv)->extra!=NULL)
            if ( ! strcmp ( (*itv)->extra , "EOF" ) ) 
                continue ;
            
        fprintf ( stderr , "\n" ) ; 
 
        if ( (*itv)->fileInput != NULL )
        {
            if ( strlen((*itv)->fileInput) > 20 )
                fprintf ( stderr,"[%-17.17s...]" ,(*itv)->fileInput ) ;
            else
                fprintf ( stderr,"[%-20.20s]"    ,(*itv)->fileInput ) ;
          
            if ( ( (*itv)->rowTok != 0 ) && ( (*itv)->colTok != 0 ) )
            {
                fprintf ( stderr , " %03d / %03d :"   , (*itv)->rowTok,(*itv)->colTok - 1        ) ; // COL-1 !!!
            }
            
            fprintf ( stderr , " " ) ;
        }
        
        fprintf ( stderr , "%-12s : "   , (char*) type      [(unsigned short)   (*itv)->type        ].value ) ; 
        fprintf ( stderr , "%-8s : "    , (char*) sender    [(unsigned short)   (*itv)->sender      ].value ) ;
        fprintf ( stderr , "%s . "      , (char*) action    [(unsigned short)   (*itv)->action      ].value ) ;
        fprintf ( stderr , "%s "        , (char*) errMessage[(unsigned short)   (*itv)->errMessage  ].value ) ;

        if ( (*itv)->extra != NULL )
        {
            char* temp = gcStrDup( (char*) (*itv)->extra ) ;
            fprintf ( stderr , ": (%s)", outputSpecialCharInString( temp ) );
        }
        fprintf ( stderr , "." ) ;  
       
    }
 
    fprintf (stderr,"\n");
    
    return 1 ;
}

 
/**/

