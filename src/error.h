
#ifndef cdError
#define cdError

#include "../lib/cxx.h"


// ................................................... sender

typedef enum sender_e
{
    sender_loader   ,   // caricatore file
    sender_lexer    ,   // lexer
    sender_parser   ,   // parser
    sender_ast      ,   // ast
    sender_node     ,   // node 
    sender_asm      ,   // assembler 
    sender_vm       ,   // Virtual Machine 
    sender_scanner  ,   // analizzatore sematico 
    sender_cpp		,	// preprocessore
	sender_compiler ,	// compiler
    
} e_sender_t ;

typedef struct sender_s
{
    const char*      value;
} sender_t ;

#define $sender(x) sender[sender_##x].value

extern sender_t sender[]  ;

// ................................................... type

typedef enum type_e
{
    type_info       ,   // informativa
    type_warning    ,   // avvertimento
    type_error      ,   // errore
    type_critical   ,   // errore critico
    type_fatal      ,   // errore fatale 
    type_internal       // interno al compilatore 
    
} e_type_t ;

typedef struct type_s
{
    const char*      value;  
} type_t ;

#define $type(x) type[type_##x].value

extern type_t type[]  ;
 
// ................................................... action

typedef enum action_e
{
    action_noErr            ,   // nessuno errore = 0
    action_noAction         ,   // nessuna azione
    action_checkFileExists  ,   // controllo esistenza file
    action_init             ,   // inizializzazione CTOR
    action_tokening         ,   // scannerizzo il file
    action_tokeningString   ,   // tokenizzo la stinga 
    action_tokeningNumber   ,   // tokenizzo un numero 	
    action_parsing   		,   // parsing 
    action_scanning   		,   // scanning
    action_coding   		,   // coding
	
} e_action_t ;

typedef struct action_s
{
    const char*      value;  
} action_t ;

#define $action(x) action[action_##x].value

extern action_t action[]  ;

// ................................................... errMessage

typedef enum errMessage_e
{
    errMessage_noErr                           ,   // nessun errore
    errMessage_fileNotFound                    ,   // file no trovato
    errMessage_noInputFiles                    ,   // nessun file in input
    errMessage_invalid_argument                ,   // argometo non valido
    errMessage_initError                       ,   // errore nell'inizializzazione
    errMessage_unexpectedToken                 ,   // token inatteso
    errMessage_outOfRange                      ,   // while converting number, out of range
    errMessage_EOF                             ,   // EOF  
    errMessage_multiLineComment	 		   	   ,   // multiLineComment (Open,Close)
    errMessage_expWithoutDigits	 		   	   ,   // exponent without digits
    errMessage_missingTerminating	 		   ,   // missing Terminating \"
    errMessage_invalidSuffix	 		   	   ,   // invalidSuffix
    errMessage_syntaxError	 		   	   	   ,   // syntaxError    
    errMessage_expected	 		   	   	   	   ,   // expected  
    errMessage_LValueRequired	 		  	   ,   // LValueRequired  
    errMessage_opNotAllowed	 		  	   	   ,   // opNotAllowed 	
    errMessage_duplicateDefinition	  	   	   ,   // duplicateDefinition 
    errMessage_invTypeArgUnaryPtr  	   	   	   ,   // invalid type argument of unary Pointer
    errMessage_lhsReqAsLeftOpOfAssignment  	   ,   // lvalue required as left operand of assignment
    errMessage_invTypeArgUnaryAddrOf  	   	   ,   // lvalue required as unary ‘&’ operand
    errMessage_wrongTypeArgToUnaryMinus		   ,   // wrong type argument to unary minus
    errMessage_notAllowedMultipleAssign		   ,   // notAllowedMultipleAssign
    errMessage_undeclared		   			   ,   // undeclared identifier
    errMessage_calledObjectNotFn   			   ,   // called object '?' is not a function or function pointer
    errMessage_noNestedBlock   			   	   ,   // nested block do not allow
    errMessage_expectedExpression			   ,   // expected expression before / 7 ( LHS not present )
    errMessage_invalidUTF8Seq			   	   ,   // invalid UTF-8 sequence
    errMessage_overflow			   	   		   ,   // overflow
    errMessage_invalidCastName			   	   ,   // invalid use of $ cast name
    errMessage_ptrArithmetic			   	   ,   // ptrArithmetic
    errMessage_tooFewArgument			   	   ,   // too few  arguments to function '...'
    errMessage_tooManyArgument			   	   ,   // too many arguments to function '...'
	
} e_errMessage_t;

typedef struct errMessage_s
{
    const char*      value;  
} errMessage_t ;

// ........................................................  errMessage[]

#define $errMessage(x) errMessage[errMessage_##x].value

extern errMessage_t errMessage[]  ;

// ........................................................  err log Message

struct errLog_s
{
    e_sender_t          sender      ;
    e_type_t            type        ;
    e_action_t          action      ;
    e_errMessage_t      errMessage  ;
    uint32_t            rowTok      ;
    uint32_t            colTok      ;
    const char*      	fileInput   ;
    const char*      	extra       ;
} ;

typedef struct errLog_s errLog_t ;

// field

vectorTypeDef(errLog_t*,errLog);  // vector_ID_t -> vector_errLog_t

// method

uint32_t pushErrLog
(
    e_sender_t          sender      ,
    e_type_t            type        ,
    e_action_t          action      ,
    e_errMessage_t      errMessage  ,
    uint32_t            rowTok      ,
    uint32_t            colTok      ,
    const char*      	fileInput   ,
    const char*      	extra        
) ;

int printErrLog(void); // error.printLog

// macro

#define $pushErrLog( SENDER,TYPE,ACTION,ERRMESSAGE,ROW,COL,FILE,EXTRA )\
        pushErrLog(\
            sender_##SENDER,\
            type_##TYPE,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            ROW,\
            COL,\
            FILE,\
            EXTRA\
        ) ;

#define $lexerErrorExtra( ACTION,ERRMESSAGE,EXTRA )\
        pushErrLog(\
            sender_lexer,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            pLex->pStream->pvCharMB->data[ pLex->lexPosStart ]->row,\
            pLex->pStream->pvCharMB->data[ pLex->lexPosStart ]->col,\
            pLex->pStream->pCompiler->fileInputName.data,\
            EXTRA\
        ) ;
		
#define $lexerErrorExtraNow( ACTION,ERRMESSAGE,EXTRA )\
        pushErrLog(\
            sender_lexer,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            pLex->pStream->pvCharMB->data[ pLex->pLexema->ndx ]->row,\
            pLex->pStream->pvCharMB->data[ pLex->pLexema->ndx ]->col,\
            pLex->pStream->pCompiler->fileInputName.data,\
            EXTRA\
        ) ;

#define $parserWarningExtra( ACTION,ERRMESSAGE,EXTRA )\
		do{\
		if (pPar->ndx >= pPar->pLex->pvToken->size ) pPar->ndx=pPar->pLex->pvToken->size-1;\
        pushErrLog(\
            sender_parser,\
            type_warning,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            pPar->pLex->pStream->pvCharMB->data[ pPar->pLex->pvToken->data[ pPar->ndx ]->tPosStart ]->row+0,\
            pPar->pLex->pStream->pvCharMB->data[ pPar->pLex->pvToken->data[ pPar->ndx ]->tPosStart ]->col+1,\
            pPar->pLex->pStream->pCompiler->fileInputName.data,\
            EXTRA\
        ) ;\
		}while(0);
		
#define $parserErrorExtra( ACTION,ERRMESSAGE,EXTRA )\
		do{\
		if (pPar->ndx >= pPar->pLex->pvToken->size ) pPar->ndx=pPar->pLex->pvToken->size-1;\
        pushErrLog(\
            sender_parser,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            pPar->pLex->pStream->pvCharMB->data[ pPar->pLex->pvToken->data[ pPar->ndx ]->tPosStart ]->row+0,\
            pPar->pLex->pStream->pvCharMB->data[ pPar->pLex->pvToken->data[ pPar->ndx ]->tPosStart ]->col+1,\
            pPar->pLex->pStream->pCompiler->fileInputName.data,\
            EXTRA\
        ) ;\
		}while(0);

#define $parserErrorExtraLast( ACTION,ERRMESSAGE,EXTRA )\
		do{\
		pPar->ndx--;\
		if (pPar->ndx >= pPar->pLex->pvToken->size ) pPar->ndx=pPar->pLex->pvToken->size-1;\
        pushErrLog(\
            sender_parser,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            pPar->pLex->pStream->pvCharMB->data[ pPar->pLex->pvToken->data[ pPar->ndx ]->tPosStart ]->row+0,\
            pPar->pLex->pStream->pvCharMB->data[ pPar->pLex->pvToken->data[ pPar->ndx ]->tPosStart ]->col+1,\
            pPar->pLex->pStream->pCompiler->fileInputName.data,\
            EXTRA\
        ) ;\
		pPar->ndx++;\
		}while(0);

#define $scannerWarningExtra( ACTION,ERRMESSAGE,EXTRA,ROW,COL )\
		do{\
        pushErrLog(\
            sender_scanner,\
            type_warning,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            ROW,\
            COL,\
            pPar->pLex->pStream->pCompiler->fileInputName.data,\
            EXTRA\
        ) ;\
		}while(0);

#define $asmErrorExtraLast( ACTION,ERRMESSAGE,EXTRA )\
		do{\
		pASM->pPar->ndx--;\
		if (pASM->pPar->ndx >= pASM->pPar->pLex->pvToken->size ) pASM->pPar->ndx=pASM->pPar->pLex->pvToken->size-1;\
        pushErrLog(\
            sender_asm,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            pNode->pData->row,\
            pNode->pData->col,\
            pASM->pPar->pLex->pStream->pCompiler->fileInputName.data,\
            EXTRA\
        ) ;\
		pASM->pPar->ndx++;\
		}while(0);

/*		
#define $asmWarningExtraLast( ACTION,ERRMESSAGE,EXTRA,ROW,COL )\
		do{\
		pASM->pPar->ndx--;\
		if (pASM->pPar->ndx >= pASM->pPar->pLex->pvToken->size ) pASM->pPar->ndx=pASM->pPar->pLex->pvToken->size-1;\
        pushErrLog(\
            sender_asm,\
            type_warning,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            ROW,\
            COL,\
            pASM->pPar->pLex->pStream->pCompiler->fileInputName.data,\
            EXTRA\
        ) ;\
		pASM->pPar->ndx++;\
		}while(0);
*/

		
extern vector_errLog_t  vErrLog 	; // error.c
extern uint32_t 		kError     	;
extern uint32_t 		kWarning   	;

/**/

#endif
