
#include "../lib/cxx.h"
#include "Cmos.h"


// *********
//  MAIN
// *********

compiler_t*    compilerNew(void)
{
    compiler_t* cmos = (compiler_t*)gcMalloc(sizeof(compiler_t));

    cmos->fDebug        = 0        ;
    cmos->kHelp         = 0        ;
    cmos->cFlags        = 0        ;
    cmos->cReturn       = 0        ;
    cmos->fOptimise     = 0        ;
    cmos->cTabSize      = 4        ;
    cmos->stackMaxSize  = 256      ;
    cmos->pNameSpace    = gcStrDup("cd")    ;
    cmos->stackAddress  = gcStrDup("c100")  ;
    cmos->stackSize     = gcStrDup("ff")    ;
    
    stringAlloc( (cmos->fileInputName),128);
    stringFromS8((cmos->fileInputName),"stdin");
    
    stringAlloc( (cmos->fileOutputName),128);
    stringFromS8((cmos->fileOutputName),"stdout");

/*
    questa funzione alloca la lo scope primario,
    contentene la symbol table, dei tipi predefiniti 
    del linguaggio, in 
    stScope->pSymbolDef     : byte, char, int, word, real
    stScope->pTypeDef : no strutture predefinite


    *pScope = (symbolTable_t*) new (symbolTable_t);    // def new symbol table (pScope)

    (*pScope)->pScope              = stNewScope();        // def start scope global 0
    (*pScope)->pScope->prev        = NULL ;

    cmos->pScope = (*pScope);
*/    
    // 
    
    return cmos;
}

void compilerDelete(compiler_t* cmos)
{
    gcFree(cmos->pNameSpace);
    
    //vectorDealloc( (*cmos->pScope->pScope->pSymbolDef    ) ) ;
    
    //vectorDealloc( (*cmos->pScope->pScope->pTypeDef    ) ) ;

    //delete( cmos->pScope->pScope ) ;
    
    assert(cmos!=NULL);
    
    delete(cmos);
}

// .........................................................

int main (int argc , const char** argv )
{
    stdio_t stdio ;
    stdioSetMBS( &stdio );
    
    compiler_t* cmos = compilerNew(); // inizializza la variabile globale symbol table

    //

    if (argc==1) 
    { 
        $pushErrLog( loader ,info,noAction,noInputFiles,0,0,cmos->fileInputName.data,"argc=1" ); 
    }
    else
    {
        int optIndex=0;
        
        while(++optIndex<argc)
        {
            switch(argv[optIndex][0])
            {
                case '-' :
                        switch(argv[optIndex][1])
                        {
                            case 'f':    // -------------------------------------- -f 12.34
                                if (++optIndex>=argc)
                                {
                                    $pushErrLog( loader , error,tokeningNumber,expected,0,0,(char*)argv[optIndex],"-f <real>");
                                }
                                else
                                {
                                    char*    end     = NULL    ;
                                    double         numberReal64     = strtod (argv[optIndex], &end) ;
                                    char*    f40s    ;
                                    f40s        = convertDoubleToMBF( numberReal64 ) ;
                                    fprintf ( stdout , "real [%s] -> f40 { %s }\n", argv[optIndex] , f40s ) ;
                                    optIndex=argc; 
                                    compilerDelete(cmos);
                                    return 0; // EXIT
                                }
                            break;
                            
                            case 'i':    // -------------------------------------- -i
                                if (++optIndex>=argc)
                                {
                                    $pushErrLog( loader ,fatal,checkFileExists,noInputFiles,0,0,(char*)argv[optIndex],"-i ?");
                                }
                                else
                                {
                                   stringFromS8( (cmos->fileInputName) ,argv[optIndex]);
                                }
                            break;
 
                            case 'o':    // -------------------------------------- -o
                                if (++optIndex>=argc)
                                {
                                    $pushErrLog( loader , fatal,checkFileExists,noInputFiles,0,0,(char*)argv[optIndex],"-o ?");
                                }
                                else
                                {
                                    stringFromS8( (cmos->fileOutputName),argv[optIndex]);
                                }
                            break;

                            case '$':    // -------------------------------------- -$
                                if (++optIndex>=argc)
                                {
                                    $pushErrLog( loader , error,tokeningNumber,expected,0,0,(char*)argv[optIndex],"-$ c000:ff");
                                }
                                else
                                {
                                    cmos->stackAddress  = (char*)&argv[optIndex][0] ;
                                    cmos->stackAddress[4] = '\0';
                                    cmos->stackSize     = (char*)&argv[optIndex][5] ;
                                    cmos->stackMaxSize = (int)strtol(cmos->stackSize, NULL, 16);
                                }
                            break;
                            
                            case 't':    // -------------------------------------- -o
                                if (optIndex+1>=argc)
                                {
                                    $pushErrLog( loader,fatal,noAction,invalid_argument,0,0,argv[optIndex+1],"-t ?");
                                }
                                else
                                {
                                    char ch = argv[optIndex+1][0] ;
                                    if ( isdigit(ch) )
                                    {
                                        cmos->cTabSize=ch-'0';
                                        ++optIndex;
                                    }
                                    else
                                    {
                                        $pushErrLog( loader,fatal,noAction,invalid_argument,0,0,argv[optIndex+1],"-t ?");
                                    } 
                                }
                            break;

                            case 'g':    // -------------------------------------- -g
                                cmos->fDebug    =    true;
                            break;
 
                            case 'O':    // -------------------------------------- -O
                                cmos->fOptimise    =    true;
                            break;

                            case 'h':    // -------------------------------------- -h

                                fprintf ( stdout , "\n(%s) lang (%s) - (%s)\n",$PRG,$REV,$DATE ) ;
                                fprintf ( stdout , "\n Usage: cd -i <file> -o <file> options..." ) ;
                                fprintf ( stdout , "\n Options:" ) ;
                                fprintf ( stdout , "\n  -i <fi>        :   file input"                 ) ;
                                fprintf ( stdout , "\n  -o <fo>        :   file output"                ) ;
                                fprintf ( stdout , "\n  -O             :   optimise"                   ) ;
                                fprintf ( stdout , "\n  -g             :   debug info"                 ) ;
                                fprintf ( stdout , "\n  -t (n)         :   tab size"                   ) ;
                                fprintf ( stdout , "\n  -f (real)      :   real -> f40"                ) ;
                                fprintf ( stdout , "\n  -$ c000:ff     :   stack address/size"        ) ;
                                fprintf ( stdout , "\n  -h             :   help file"                  ) ;
                                fprintf ( stdout , "\n\n" ) ;
                                
                                kError        = 0;       // se -h visualizza solo informazioni
                                kWarning    = 0;
                                cmos->kHelp    = 1;
                                
                            break;

                            // -------------------------------------- -?
                            default:
                                $pushErrLog( loader,fatal,init,invalid_argument,0,0, argv[optIndex] ," opt not valid ");
                            break;
                        }
                break;
                
                default:
                    // -------------------------------------- ??
                    $pushErrLog( loader,fatal,init,unexpectedToken,0,0, argv[optIndex]," opt not recognize ");
                break;
            }
        }
    }

    // COMPILE !

   if ( !kError )
   {
       if ( cmos->fDebug==true )
       {
           fprintf ( stdout , "\nfi <%s>. "         , cmos->fileInputName.data              );
           fprintf ( stdout , "\nfo <%s>. "         , cmos->fileOutputName.data             );
           fprintf ( stdout , "\ndebug <%s>."       , (cmos->fDebug?"true":"false")         );
           fprintf ( stdout , "\noptimise <%s>."    , (cmos->fOptimise?"true":"false")      );
           fprintf ( stdout , "\ntab size (%d)."    , cmos->cTabSize                        );           
           fprintf ( stdout , "\n"                                                          );
       }

    if ( !kError ) // ...................................................... stream
    {
        streamMBChar_t*  pStream = streamNew( cmos );
        
        streamGetSource(pStream) ;

        if ( !kError ) // .................................................. lexer
        {
            lexer_t *pLex = lexerNew( pStream );
            
            lexerMakeVectorTokens( pLex ) ;

            if ( !kError ) // .............................................. parser 
            {
                parser_t *pPar = parserNew( pLex );
                
                parserRun( pPar ) ;
                
                if ( !kError ) // ......................................... assembler
                {
                    assembler_t* pASM = assemblerNew(pPar);
                    
                    assembleGO(pASM);
                    
                    assemblerDelete(pASM);
                }
                parserDelete( pPar ) ;
            }
            lexerDelete(pLex);
        }
        streamDelete(pStream);
    }

    // END

   }

    // ................................................................ The End 

    if ( (kError||kWarning) && !cmos->kHelp ) printErrLog();

    
    compilerDelete(cmos);
    
    return  0  ;
}