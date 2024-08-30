
#ifndef cdSTDIO
#define cdSTDIO
 
    #define stdioStreamUTF8    -1
    #define stdioStreamANSI     0
    #define stdioStreamMBS      1

    // questo per evitare warning comparazione ( signed wchar_t / unsigned wchar_t )
    #define _WEOF (wchar_t)WEOF

    // (not standard ) : error: use of undeclared identifier 'errno_t'
    typedef int errno_t;

    typedef struct stdio_s
    {
        #if defined(cdWINDOWS)    
            CONSOLE_FONT_INFOEX     info            ; 
        #else
            int                     info            ;
        #endif            
        int                         chcp            ;
        int                         stream_stdout   ;
        int                         stream_stdin    ;
        int                         stream_stderr   ;
    } stdio_t ;


void        stdioSetUTF8        ( stdio_t *stdio )     ;
void        stdioSetMBS         ( stdio_t *stdio )     ;

FILE*       gcLocalFileOpenW            (gc_t *map , char* fileName , const char* flag , const char* ccs ) ;
#define     gcFileOpenW(FN,FG,CCS)      gcLocalFileOpenW(gc,FN,FG,CCS)


#endif



/**/
