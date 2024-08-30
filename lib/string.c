#include "string.h"

// ............................................... vIn len 16 / 32

size_t str16len(char16_t* strarg)
{
   if(!strarg) return 0; 
   
   char16_t* vIn = strarg;
   
   for(;*vIn;++vIn)
     ; 
     
   return vIn-strarg;
}


size_t str32len(char32_t* strarg)
{
   if(!strarg) return 0; 
   
   char32_t* vIn = strarg;
   
   for(;*vIn;++vIn)
     ; 
     
   return vIn-strarg;
}

size_t str8len(char* strarg)
{
   if(!strarg) return 0; 
   
   char* vIn = strarg;
   
   for(;*vIn;++vIn)
     ; 
     
   return vIn-strarg;
}

size_t strwlen(wchar_t* strarg)
{
   if(!strarg) return 0; 
   
   wchar_t* vIn = strarg;
   
   for(;*vIn;++vIn)
     ; 
     
   return vIn-strarg;
}

// https://en.cppreference.com/w/c/string/multibyte/mblen

size_t strmblen(const char* strarg)
{
    if(!strarg) return 0;
       
    size_t result = 0;
    const char* end = strarg + strlen(strarg);
    mblen(NULL, 0); // reset the conversion state
    while(strarg < end) 
    {
        int next = mblen(strarg, end - strarg);
        if(next == -1) {
           perror("strlen_mb");
           break;
        }
        strarg += next;
        ++result;
    }
    return result;
}

// ............................................... convert String 16 / 32 to Multi Byte

char* convS16toMBS( char16_t * pt )
{
    char buffer[cdMB_CUR_MAX];
    size_t i ;
    size_t length ;
    mbstate_t mbs = {0};

    stringTypeDef(char,s1) ;
    string_s1_t mbstemp ;
    stringAlloc(mbstemp,8);

    while(*pt)
    {
        length = c16rtomb(buffer,*pt,&mbs);
        if ((length==0)||(length>cdMB_CUR_MAX)) break ;
        for ( i=0;i<length;i++) stringPushBack ( mbstemp , buffer[i] ) ;
        ++pt ;
    }
    return mbstemp.data ;
}

char* convS32toMBS( char32_t * pt )
{
    char buffer[cdMB_CUR_MAX];
    size_t i ;
    size_t length ;
    mbstate_t mbs = {0};

    stringTypeDef(char,s1) ;
    string_s1_t mbstemp ;
    stringAlloc(mbstemp,8);

    while(*pt)
    {
        length = c32rtomb(buffer,*pt,&mbs);
        if ((length==0)||(length>cdMB_CUR_MAX)) break ;
        for ( i=0;i<length;i++) stringPushBack ( mbstemp , buffer[i] ) ;
        ++pt ;
    }
    return mbstemp.data ;
}

// ........................................................... convert S8 <-> MB ( WINDOWS ) and viceversa

// https://stackoverflow.com/questions/42793735/
// how-to-convert-between-widecharacter-and-multi-byte-character-string-in-windows

#ifdef _MSC_VER

static
WCHAR* convert_to_wstring(const char* vIn)
{
    int str_len = (int) strlen(vIn);
    int num_chars = MultiByteToWideChar(CP_UTF8, 0, vIn, str_len, NULL, 0);
    WCHAR* wstrTo = (WCHAR*) gcMalloc((num_chars + 1) * sizeof(WCHAR));
    if (wstrTo)
    {
        MultiByteToWideChar(CP_UTF8, 0, vIn, str_len, wstrTo, num_chars);
        wstrTo[num_chars] = L'\0';
    }
    return wstrTo;
}

static
CHAR* convert_from_wstring(const WCHAR* wstr)
{
    int wstr_len = (int) wcslen(wstr);
    int num_chars = WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_len, NULL, 0, NULL, NULL);
    CHAR* strTo = (CHAR*) gcMalloc((num_chars + 1) * sizeof(CHAR));
    if (strTo)
    {
        WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_len, strTo, num_chars, NULL, NULL);
        strTo[num_chars] = '\0';
    }
    return strTo;
}
#endif

wchar_t* cnvS8toWS(char * vIn)
{
    #if defined(_MSC_VER)   
        return convert_to_wstring(vIn);
    #else
        const size_t slen=strlen(vIn);
        wchar_t * vOut = gcMalloc ( (slen+1) * sizeof(wchar_t) ) ;
        mbstowcs (vOut, vIn, slen+1 );
        return vOut;        
    #endif

    return NULL;
}

// ........................................................... convert vIn to MB

char* cnvWStoS8( wchar_t* vIn )
{
    #if defined(_MSC_VER)
        return convert_from_wstring(vIn);
    #else
        size_t len = sizeof(wchar_t) * wcslen(vIn) ;
        char* vOut = gcCalloc ( sizeof(wchar_t),len );  
        wcstombs ( vOut, vIn, len ) ;
        vOut=gcRealloc(vOut,strlen(vOut)+1);
        return vOut ;  
    #endif

    return NULL ;
}

// ........................................................... convert real32 to S8 vIn

char* cnvR32toS8(float vIn)
{
    char vOutChar [17];
    
    #if defined(_MSC_VER)
    _gcvt_s(vOutChar,sizeof(vOutChar),vIn,8);
    #else
    gcvt(vIn,8,vOutChar);
    #endif

    return gcStrDup(vOutChar);
}

wchar_t* cnvR32toWS(float vIn)
{
    char vOutChar [17];
    
    #if defined(_MSC_VER)
    _gcvt_s(vOutChar,sizeof(vOutChar),vIn,8);
    #else
    gcvt(vIn,8,vOutChar);
    #endif

    wchar_t vOut[17];

    #if defined (_MSC_VER)
    mbstowcs_s(NULL,vOut,sizeof(vOut)/2,vOutChar,sizeof(vOutChar));
    #else
    mbstowcs (vOut, vOutChar, sizeof(vOut)/2 );
    #endif

    return gcWcsDup(vOut);
}

// ........................................................... convert real64 to S8 vIn 

char* cnvR64toS8(double vIn)
{
    const unsigned char maxBufferLen=32;
    char* vOut=gcCalloc(sizeof(char),maxBufferLen);
    const size_t digits=8;
    
    #if defined(_MSC_VER)
    _gcvt_s( vOut, 32, vIn, digits );
    #else
    gcvt(vIn,digits,vOut);
    #endif

    return gcStrDup(vOut);
}

wchar_t* cnvR64toWS(double vIn)
{
    char vOutChar [26];
    #if defined (_MSC_VER)
    _gcvt_s(vOutChar,sizeof(vOutChar),vIn,17);
    #else
    gcvt(vIn,17,vOutChar);
    #endif    
    
    wchar_t vOut[26];
    
    #if defined (_MSC_VER)
    mbstowcs_s(NULL,vOut,sizeof(vOut)/2,vOutChar,sizeof(vOutChar));
    #else
    mbstowcs (vOut, vOutChar, sizeof(vOut)/2 );
    #endif
    
    return gcWcsDup(vOut);
}

// ........................................................... convert I80 to S8 --

char* cnvI80toS8(long long vIn)
{
    const unsigned char maxBufferLen=32;
    char* vOut=gcCalloc(sizeof(char),maxBufferLen);
    snprintf(vOut, maxBufferLen, "%lld", vIn);

    return vOut ;
}

wchar_t* cnvI80toWS(long long vIn)
{
    const unsigned char maxBufferLen=32;
    wchar_t* vOut=gcCalloc(sizeof(wchar_t),maxBufferLen);
    swprintf(vOut, maxBufferLen, L"%lld", vIn);

    return gcWcsDup(vOut) ;
}

// ........................................................... convert I64 to S8 vIn

char* cnvI32toS8(int vIn)
{
    char vOut [12];
    
    #if defined (_MSC_VER)
    _itoa_s(vIn,vOut,sizeof(vOut),10);
    #else
    sprintf( vOut, "%d", vIn);
    #endif

    return gcStrDup(vOut) ;
}

wchar_t* cnvI32toWS(int vIn)
{
#if defined (_MSC_VER)
    wchar_t vOut [12];    
    _itow_s(vIn,vOut,sizeof(vOut)/2,10);
    return gcWcsDup(vOut);    
#else
    const unsigned char maxBufferLen=32;
    wchar_t* vOut=gcCalloc(sizeof(wchar_t),maxBufferLen);
    swprintf(vOut, maxBufferLen, L"%d", vIn);
    return vOut ;
#endif
}


// ........................................................... convert I64 to S8 vIn

char* cnvI64toS8(long vIn)
{
    const unsigned char maxBufferLen=32;
    char* vOut=gcCalloc(sizeof(char),maxBufferLen);
    snprintf(vOut, maxBufferLen, "%ld", vIn);

    return vOut ;
}

wchar_t* cnvI64toWS(long vIn)
{
#if defined (_MSC_VER)
    wchar_t vOut [12];    
    _itow_s(vIn,vOut,sizeof(vOut)/2,10);
    return gcWcsDup(vOut);    
#else
    const unsigned char maxBufferLen=32;
    wchar_t* vOut=gcCalloc(sizeof(wchar_t),maxBufferLen);
    swprintf(vOut, maxBufferLen, L"%ld", vIn);
    return vOut ;
#endif
}

// ........................................................... convert I64 to S8 vIn

char* cnvPTRtoS8(void* vIn)
{
    const unsigned char maxBufferLen=32;
    char* vOut=gcCalloc(sizeof(char),maxBufferLen);
    
    #if defined (_MSC_VER)
    snprintf(vOut, maxBufferLen, "0x%p", vIn);
    #else
    snprintf(vOut, maxBufferLen, "%p", vIn);
    #endif
    
    return vOut ;
}  

wchar_t* cnvPTRtoWS(void* vIn)
{
    const unsigned char maxBufferLen=32;
    wchar_t* vOut=gcCalloc(sizeof(wchar_t),maxBufferLen+1);
    
    #if defined (_MSC_VER)    
    swprintf(vOut, maxBufferLen*sizeof(wchar_t), L"0x%p", vIn);
    #else
    swprintf(vOut, maxBufferLen*sizeof(wchar_t), L"%p", vIn);
    #endif    
    
    return vOut ;
}  

// https://creativeandcritical.net/str-replace-c

char *repl_str(const char *str, const char *from, const char *to) 
{
    /* Adjust each of the below values to suit your needs. */

    /* Increment positions cache size initially by this number. */
    size_t cache_sz_inc = 16;
    /* Thereafter, each time capacity needs to be increased,
     * multiply the increment by this factor. */
    const size_t cache_sz_inc_factor = 3;
    /* But never increment capacity by more than this number. */
    const size_t cache_sz_inc_max = 1048576;

    char *pret, *ret = NULL;
    const char *pstr2, *pstr = str;
    size_t i, count = 0;
    #if (__STDC_VERSION__ >= 199901L)
    uintptr_t *pos_cache_tmp, *pos_cache = NULL;
    #else
    ptrdiff_t *pos_cache_tmp, *pos_cache = NULL;
    #endif
    size_t cache_sz = 0;
    size_t cpylen, orglen, retlen, tolen, fromlen = strlen(from);

    /* Find all matches and cache their positions. */
    
    pos_cache = gcMalloc(sizeof(*pos_cache) * cache_sz); // FIX MEMORY LEAK
    
    while ((pstr2 = strstr(pstr, from)) != NULL) {
        count++;

        /* Increase the cache size when necessary. */
        if (cache_sz < count) {
            cache_sz += cache_sz_inc;
            pos_cache_tmp = gcRealloc(pos_cache, sizeof(*pos_cache) * cache_sz);
            if (pos_cache_tmp == NULL) {
                goto end_repl_str;
            } else pos_cache = pos_cache_tmp;
            cache_sz_inc *= cache_sz_inc_factor;
            if (cache_sz_inc > cache_sz_inc_max) {
                cache_sz_inc = cache_sz_inc_max;
            }
        }

        pos_cache[count-1] = pstr2 - str;
        pstr = pstr2 + fromlen;
    }

    orglen = pstr - str + strlen(pstr);

    /* Allocate memory for the post-replacement string. */
    if (count > 0) {
        tolen = strlen(to);
        retlen = orglen + (tolen - fromlen) * count;
    } else    retlen = orglen;
    ret = gcMalloc(retlen + 1);
    if (ret == NULL) {
        goto end_repl_str;
    }

    if (count == 0) {
        /* If no matches, then just duplicate the string. */
        strcpy(ret, str);
    } else {
        /* Otherwise, duplicate the string whilst performing
         * the replacements using the position cache. */
        pret = ret;
        memcpy(pret, str, pos_cache[0]);
        pret += pos_cache[0];
        for (i = 0; i < count; i++) {
            memcpy(pret, to, tolen);
            pret += tolen;
            pstr = str + pos_cache[i] + fromlen;
            cpylen = (i == count-1 ? orglen : pos_cache[i+1]) - pos_cache[i] - fromlen;
            memcpy(pret, pstr, cpylen);
            pret += cpylen;
        }
        ret[retlen] = '\0';
    }

end_repl_str:
    /* gcFree the cache and return the post-replacement string,
     * which will be NULL in the event of an error. */
    gcFree(pos_cache);
    return ret;
}

wchar_t *repl_wcs(const wchar_t *str, const wchar_t *from, const wchar_t *to) 
{
    /* Adjust each of the below values to suit your needs. */

    /* Increment positions cache size initially by this number. */
    size_t cache_sz_inc = 16;
    /* Thereafter, each time capacity needs to be increased,
     * multiply the increment by this factor. */
    const size_t cache_sz_inc_factor = 3;
    /* But never increment capacity by more than this number. */
    const size_t cache_sz_inc_max = 1048576;

    wchar_t *pret, *ret = NULL;
    const wchar_t *pstr2, *pstr = str;
    size_t i, count = 0;
    #if (__STDC_VERSION__ >= 199901L)
    uintptr_t *pos_cache_tmp, *pos_cache = NULL;
    #else
    ptrdiff_t *pos_cache_tmp, *pos_cache = NULL;
    #endif
    size_t cache_sz = 0;
    size_t cpylen, orglen, retlen, tolen, fromlen = wcslen(from);

    /* Find all matches and cache their positions. */
    
    pos_cache = gcMalloc(sizeof(*pos_cache) * cache_sz); // FIX MEMORY LEAK
        
    while ((pstr2 = wcsstr(pstr, from)) != NULL) {
        count++;

        /* Increase the cache size when necessary. */
        if (cache_sz < count) {
            cache_sz += cache_sz_inc;
            pos_cache_tmp = gcRealloc(pos_cache, sizeof(*pos_cache) * cache_sz);
            if (pos_cache_tmp == NULL) {
                goto end_repl_wcs;
            } else pos_cache = pos_cache_tmp;
            cache_sz_inc *= cache_sz_inc_factor;
            if (cache_sz_inc > cache_sz_inc_max) {
                cache_sz_inc = cache_sz_inc_max;
            }
        }

        pos_cache[count-1] = pstr2 - str;
        pstr = pstr2 + fromlen;
    }

    orglen = pstr - str + wcslen(pstr);

    /* Allocate memory for the post-replacement string. */
    if (count > 0) {
        tolen = wcslen(to);
        retlen = orglen + (tolen - fromlen) * count;
    } else    retlen = orglen;
    ret = gcMalloc((retlen + 1) * sizeof(wchar_t));
    if (ret == NULL) {
        goto end_repl_wcs;
    }

    if (count == 0) {
        /* If no matches, then just duplicate the string. */
        wcscpy(ret, str);
    } else {
        /* Otherwise, duplicate the string whilst performing
         * the replacements using the position cache. */
        pret = ret;
        wmemcpy(pret, str, pos_cache[0]);
        pret += pos_cache[0];
        for (i = 0; i < count; i++) {
            wmemcpy(pret, to, tolen);
            pret += tolen;
            pstr = str + pos_cache[i] + fromlen;
            cpylen = (i == count-1 ? orglen : pos_cache[i+1]) - pos_cache[i] - fromlen;
            wmemcpy(pret, pstr, cpylen);
            pret += cpylen;
        }
        ret[retlen] = L'\0';
    }

end_repl_wcs:
    /* gcFree the cache and return the post-replacement string,
     * which will be NULL in the event of an error. */
    gcFree(pos_cache);
    return ret;
}

wchar_t* wsFormat_(const wchar_t* format,...) 
{
    va_list args,a2 ;
    va_start (args  , format );
    va_start (a2    , format );

    // calcola la lunghezza della stringa 
    wchar_t buffer[gcMaxStringBuffer];

    int len = -1;
    
    len=vswprintf(buffer,gcMaxStringBuffer,format, args ); 
    assert(len>=0);

    wchar_t* temp  = gcMalloc( (len+1)*sizeof(wchar_t) );
    assert(temp!=NULL);
    
    // formattala
    len=vswprintf( temp , len+1, format, a2 );     
    assert(len>=0); 
    
    va_end (args);
    va_end (a2); 
    
    return temp;          
}
    

char* s8Format_(const char* format,...) 
{
    va_list args,a2 ;
    va_start (args  , format );
    va_start (a2    , format );

    // calcola la lunghezza della stringa 
    char buffer[gcMaxStringBuffer];

    int len = -1;
    
    len=vsprintf(buffer,format, args ); 
    assert(len>=0);

    char* temp  = gcMalloc( (len+1)*sizeof(wchar_t) );
    assert(temp!=NULL);
    
    // formattala
    len=vsprintf( temp , format, a2 );     
    assert(len>=0); 
    
    va_end (args);
    va_end (a2); 
    
    return temp;
}

wchar_t    ws_outputSpecialCharInChar( wchar_t _wchar )
{
    if ( _wchar==0 ) return L'§';
    
    // \r viene saltato \f \v \t \f \' \" \\ vengono trasformati
    wchar_t wchar = _wchar ;
    
    switch ( _wchar ) 
    {
        case L'\a'     : wchar = L'A'; break ;
        case L'\n'     : wchar = L'N'; break ;
        case L'\t'     : wchar = L'T'; break ;
        case L'\v'     : wchar = L'V'; break ;
        case L'\f'     : wchar = L'F'; break ;
        case L'\''     : wchar = L'\''; break ;
        case L'\"'     : wchar = L'\"'; break ;
        case L'\\'     : wchar = L'\\'; break ;
        default        : break;
    } 

    return wchar ;
}

wchar_t*    ws_outputSpecialCharInString( wchar_t* token )
{
    if ( token == NULL ) return gcWcsDup(L"{null}");
    
    for ( size_t i=0; i < wcslen( token ) ; i++ )
    {
        token[i] = ws_outputSpecialCharInChar( token[i] ) ;
    }
    return token ;
}


char    outputSpecialCharInChar( char _char )
{
    if ( _char==0 ) return '.';
    
    // \r viene saltato \f \v \t \f \' \" \\ vengono trasformati
    char mbchar = _char ;
    
    switch ( _char ) 
    {
        case '\a'     : mbchar = 'A'; break ;
        case '\n'     : mbchar = 'N'; break ;
        case '\t'     : mbchar = 'T'; break ;
        case '\v'     : mbchar = 'V'; break ;
        case '\f'     : mbchar = 'F'; break ;
        case '\''     : mbchar = '\''; break ;
        case '\"'     : mbchar = '\"'; break ;
        case '\\'     : mbchar = '\\'; break ;
        default       : break;
    } 

    return mbchar ;
}

char*    outputSpecialCharInString( char* token )
{
    if ( token == NULL ) return gcStrDup("{null}");
    
    for ( size_t i=0; i < strlen( token ) ; i++ )
    {
        token[i] = ws_outputSpecialCharInChar( token[i] ) ;
    }
    return token ;
}


/**/
