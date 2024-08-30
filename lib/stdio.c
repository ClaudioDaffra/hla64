
#include "cxx.h"

// ....................................................................... stdioSetUTF8

void stdioSetUTF8( stdio_t *stdio )
{
#ifdef _MSC_VER

    // default value
    
    const wchar_t   _fontName[] =    L"SimSun-ExtB"     ;
    const SHORT     _fontWeight =    FW_NORMAL          ;
    const SHORT     _fontSizeY  =    16                 ;
    const SHORT     _fontSizeX  =    0                  ;
    const int        chcp       =    936                ;

    // set font

    stdio->info.cbSize       = sizeof(stdio->info)      ;
    stdio->info.dwFontSize.Y = _fontSizeY               ; // 16
    stdio->info.dwFontSize.X = _fontSizeX               ; // 0    
    stdio->info.FontWeight   = _fontWeight              ;

    wcscpy_s(     stdio->info.FaceName, _countof(_fontName) , _fontName ) ;
    
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), false, &stdio->info);
    
    // set stream
    
     _wsetlocale(LC_ALL , L"it_IT.UTF-8"    );      // set locale wide string
    _setmode(_fileno(stdout), _O_U8TEXT     );      // set Locale for stdio
    _setmode(_fileno(stderr), _O_U8TEXT     );      // set Locale for stdio    
    _setmode(_fileno(stdin ), _O_U8TEXT     );      // set Locale for stdio      

    // set CHCP : 936 1252 > win7 : 65001
    
    stdio->chcp = chcp ; 
    SetConsoleCP( chcp ) ;             
    SetConsoleOutputCP( chcp ); 

    // questo per evitare errori di conversione delle stringhe esadecimali
    setlocale ( LC_NUMERIC , "C" ) ;
    
    stdio->stream_stdout    = stdioStreamUTF8 ;
    stdio->stream_stdin     = stdioStreamUTF8 ;
    stdio->stream_stderr    = stdioStreamUTF8 ;
    
#else

    setlocale ( LC_ALL, "C.UTF-8") ;
    setlocale ( LC_NUMERIC , "C"      ) ;

    stdio->stream_stdin     = stdioStreamMBS ;
    stdio->stream_stdout    = stdioStreamMBS ;
    stdio->stream_stderr    = stdioStreamMBS ;
    
#endif
} 

// ....................................................................... stdioSetMBS

void stdioSetMBS( stdio_t *stdio )
{
#ifdef _MSC_VER
    //const wchar_t*   _fontName=L"SimSun-ExtB";
    const wchar_t   _fontName[]=L"SimSun-ExtB";
    SHORT           _fontWeight=FW_NORMAL;
    SHORT           _fontSizeY=16;
    SHORT           _fontSizeX=0;    

    // set font
    
    stdio->info.cbSize       = sizeof(stdio->info);
    stdio->info.dwFontSize.Y = _fontSizeY; // 16
    stdio->info.dwFontSize.X = _fontSizeX; // 0    
    stdio->info.FontWeight   = _fontWeight;

    wcscpy_s(     stdio->info.FaceName, _countof(_fontName) , _fontName ) ;
    
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), false, &stdio->info);
    int chcp=65001;            
    SetConsoleCP( chcp ) ;             
    SetConsoleOutputCP( chcp ); 
    
    setlocale ( LC_ALL,"it_IT.utf8"    ) ;
    setlocale ( LC_NUMERIC , "C"     ) ; 
    
    stdio->stream_stdin     = stdioStreamMBS ;
    stdio->stream_stdout     = stdioStreamMBS ;
    stdio->stream_stderr     = stdioStreamMBS ;
    
#else

    setlocale ( LC_ALL, "C.UTF-8") ;
    setlocale ( LC_NUMERIC , "C"      ) ;

    stdio->stream_stdin     = stdioStreamMBS ;
    stdio->stream_stdout     = stdioStreamMBS ;
    stdio->stream_stderr     = stdioStreamMBS ;
    
#endif
}
 
// .................................................................... open file wide stream

int cdFileWOpen( 
    FILE** pf,
    char* fileName, 
    const char* flag , 
    const char* ccs  
) 
{
    wchar_t buffer[128] ;
    buffer[0]=L'\0';

    wchar_t* _fileName  = cnvS8toWS ( fileName      ) ;
    wchar_t* _flag      = cnvS8toWS ( (char*)flag   ) ;
    wchar_t* _ccs       = cnvS8toWS ( (char*)ccs    ) ;

    if (_fileName==NULL) return 3 ;

    if ( wcslen(_flag)+wcslen(_ccs) > 14 )
    {
        return 2 ;
    }

    wcscat (buffer,_flag    );
    wcscat (buffer,L","     );
    wcscat (buffer,_ccs     );

#if defined(_WIN32) || defined(_WIN64)

    wchar_t *tmpw=wcsdup(_fileName); // questo per evitare seg fault
    
    for (uint32_t i=0;i<wcslen(_fileName);i++ )
    {
       if ( tmpw[i]==L'/' ) tmpw[i] = L'\\' ;
    }
    
    errno_t err=_wfopen_s( pf,tmpw  , buffer  );

    free(tmpw); 
    
    if ( err ) 
    {
        //fwprintf(stdout,L"\n? Error Open File : [%ls] for [%ls].",_fileName,_flag);
        return -1;
    }
    
#else
  
    char *tmp=strdup(fileName);
    if ( tmp != NULL )
    {
        for (uint32_t i=0;i<strlen(fileName);i++ )
        {
           if ( tmp[i]=='\\' ) tmp[i] = '/' ;
        }

        _ccs        = _ccs      ;
        buffer[0]   = L'\0'     ;
        _fileName   = _fileName ;

        *pf = fopen ( tmp , flag ) ;
        if ( *pf==NULL )
        {
            //fwprintf(stdout,L"\n? Error Open File : [%ls] for [%ls].",_fileName,_flag);
            free(tmp);
            return -1;
        }
        
        free(tmp);
    }
    else
    {
        fwprintf(stdout,L"\n? Error Allocate tmp : [%ls]..",fileName);
        return 0;
    }
        
#endif

    return 1 ;
}

// .................................................................... gc local file open wide

FILE* gcLocalFileOpenW(gc_t *map , char* fileName , const char* flag , const char* ccs )
{
    FILE* pf = NULL ;

    cdFileWOpen ( &pf, fileName , flag , ccs ) ;
    
    gcMapPut ( map , (void*)pf , gcFCloseDtor, GC_FAST );
 
    return pf ;
}
