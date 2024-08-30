
#include "cxx.h"

// ........................................... str(x)len

size_t      str16len    (char16_t*      strarg) ;
size_t      str32len    (char32_t*      strarg) ;
size_t      str8len     (char*          strarg) ;
size_t      strwlen     (wchar_t*       strarg) ;
size_t      strmblen    (const char*    strarg) ;
#define     strmbslen(VIN)  strmblen(VIN)
 
// ........................................... cnv s16,32 to mbs

char*       convS16toMBS( char16_t * pt ) ; // ...
char*       convS32toMBS( char32_t * pt ) ; // ...

wchar_t*    cnvS8toWS       (char *     vIn ) ;
char*       cnvWStoS8       (wchar_t*   vIn ) ;
#define     cnvWStoMBS(VIN)  cnvWStoS8(VIN)

// ........................................... cnv number

char*       cnvI32toS8    (int vIn);
wchar_t*    cnvI32toWS    (int vIn);

#define     cnvI16toS8(VIN)    cnvI32toS8((int)VIN)
#define     cnvI16toWS(VIN)    cnvI32toWS((int)VIN)

char*       cnvI64toS8    (long vIn) ;
wchar_t*    cnvI64toWS    (long vIn) ;

char*       cnvI80toS8    (long long vIn) ;
wchar_t*    cnvI80toWS    (long long vIn) ;

char*       cnvPTRtoS8    (void* vIn) ;
wchar_t*    cnvPTRtoWS    (void* vIn) ;

char*       cnvR32toS8    (float vIn) ;
wchar_t*    cnvR32toWS    (float vIn) ;

char*       cnvR64toS8    (double vIn) ;
wchar_t*    cnvR64toWS    (double Vin) ;

char *repl_str(const char *str, const char *from, const char *to);
#define stringReplaceAll( ID , FROM , TO )\
do{\
(ID).data=repl_str((ID).data ,FROM,TO);\
size_t len=strlen((ID).data);\
(ID).size=len;\
(ID).capacity=len;\
}while(0);

wchar_t *repl_wcs(const wchar_t *str, const wchar_t *from, const wchar_t *to) ;
#define wstringReplaceAll( ID , FROM , TO )\
do{\
(ID).data=repl_wcs((ID).data ,FROM,TO);\
size_t len=wcslen((ID).data);\
(ID).size=len;\
(ID).capacity=len;\
}while(0);

wchar_t* wsFormat_(const wchar_t* format,...);
#define wsFormat(FORMAT,...) wsFormat_( FORMAT , __VA_ARGS__ )

char* s8Format_(const char* format,...);
#define s8Format(FORMAT,...) s8Format_( FORMAT , __VA_ARGS__ )

 
wchar_t    	ws_outputSpecialCharInChar   ( wchar_t _wchar ) ;
wchar_t*   	ws_outputSpecialCharInString ( wchar_t* token ) ;

char    	outputSpecialCharInChar( char _char ) ;
char*    	outputSpecialCharInString( char* token ) ;

/**/


