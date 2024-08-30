
#include "../lib/cxx.h"
#include "Cmos.h"

// *********
//  STREAM
// *********

// ................................................................................  streamOpen

fileStream_t *    streamOpen( char *fileName , char *mod ,e_sender_t sender )
{
    char     bufferTemp[BUFFER_STRING_1024];
    char*     fileExt=NULL;

    snprintf ( bufferTemp ,BUFFER_STRING_1024, "%s",fileName  );
        
    if (strcmp(mod,"r")!=0) // non aggiungere nulla ai file in sola lettura
    {
        switch((int)sender)
        {
            case sender_loader:      fileExt=gcStrDup(".loader");
            break;
            case sender_lexer:       fileExt=gcStrDup(".lexer");     
            break;
            case sender_parser:      fileExt=gcStrDup(".parser");    
            break;
            case sender_ast:         fileExt=gcStrDup(".ast");    
            break;
            case sender_node:        fileExt=gcStrDup(".node");    
            break;        
            case sender_asm    :     fileExt=gcStrDup(".asm");    
            break;
            case sender_vm:          fileExt=gcStrDup(".vm");    
            break;        
            case sender_scanner:     fileExt=gcStrDup(".scanner");
            break;
            case sender_cpp:         fileExt=gcStrDup(".cpp");    
            break;        
            default:
                fileExt=NULL;
            break;
        }
    }
 
    if(fileExt!=NULL)
    {
        snprintf ( bufferTemp ,BUFFER_STRING_1024, "%s%s",fileName , fileExt );
    }

    FILE *fio = gcFileOpen(bufferTemp, mod);
    
    if (fio==NULL)
    {
        $pushErrLog( loader,fatal,init,fileNotFound,0,0, fileName,mod );
        return NULL ;
    }

    fileStream_t *fs = (fileStream_t *) gcMalloc( sizeof(fileStream_t) ) ;

    assert(fs!=NULL);

    fs->name     = gcStrDup((char*)bufferTemp);    
    fs->ptr      = fio ;
    
    return fs ;
}

// ................................................................................  streamNew

streamMBChar_t* streamNew( compiler_t *pCompiler )
{
    streamMBChar_t*     pStream = (streamMBChar_t*) new ( streamMBChar_t ) ;
    
    pStream->pCompiler = pCompiler ;

    char *fileStreamName  = pStream->pCompiler->fileInputName.data;

    pStream->pFileStream = streamOpen( fileStreamName , "r" , sender_loader ) ;

    if ( pStream->pFileStream == NULL )
    {
        $pushErrLog( loader ,critical,checkFileExists,noInputFiles,0,0,fileStreamName,"?");
        return NULL ;
    }

    if ( pStream->pFileStream->ptr == NULL )
    {
        $pushErrLog( loader ,fatal,checkFileExists,noInputFiles,0,0,pStream->pFileStream->name,"?");
        return NULL ;
    }

    // ............................................................................. source
    
    pStream->pSource = (string_source_t*) new (string_source_t) ;
    
    stringAlloc( (*pStream->pSource),BUFFER_STRING_4096 );
    
    stringFromS8((*pStream->pSource),"\0");

    // ............................................................................. vector_charMB_t
    
    pStream->pvCharMB = (vector_charMB_t*)  new ( vector_charMB_t ) ;

    vectorAlloc( (*pStream->pvCharMB), 128 );

    // ............................................................................. DEBUG vector_charMB_t
   
    if ( pStream->pCompiler->fDebug )
    {
        pStream->pFileDebugCharMB = streamOpen( pStream->pCompiler->fileInputName.data , "w+" , sender_loader ) ;
    }
    
    return pStream ;
}

// ................................................................................  streamDelete

void streamDelete(streamMBChar_t* pStream )
{
    if ( pStream )
    {
        if ( pStream->pCompiler )
            if ( pStream->pCompiler->fDebug )
                {
                    gcFree( pStream->pFileDebugCharMB ) ;
                }
        
        vectorDealloc( (*pStream->pvCharMB) );
        
        gcFree(pStream->pvCharMB);    
        
        stringDealloc( (*pStream->pSource) ) ;
            
        gcFree(pStream->pSource);
        
        gcFree(pStream->pFileStream);
        
        delete(pStream);
    }
}

// https://github.com/rui314/chibicc/blob/main/tokenize.c

// ................................................................................  streamRemoveBOM
// UTF-8 texts may start with a 3-byte "BOM" marker sequence.
// If exists, just skip them because they are useless bytes.
// (It is actually not recommended to add BOM markers to UTF-8
// texts, but it's not uncommon particularly on Windows.)

char* streamRemoveBOM(streamMBChar_t* pStream)
{
    if (!memcmp(pStream->pSource->data, "\xef\xbb\xbf", 3)) pStream->pSource->data += 3 ;

    return pStream->pSource->data ;
}
// ................................................................................  streamNormaliseNewLine
// Replaces \r or \r\n with \n.

char* streamNormaliseNewLine(streamMBChar_t* pStream) 
{
  char *p = pStream->pSource->data ;
  
  int i = 0, j = 0;

  while (p[i]) 
  {
    if (p[i] == '\r' && p[i + 1] == '\n') 
    {
      i += 2;
      p[j++] = '\n';
    } 
    else
    {
        if (p[i] == '\r') 
        {
          i++;
          p[j++] = '\n';
        } 
        else 
        {
          p[j++] = p[i++];
        }
    }
  }
  p[j] = '\0';
  
  return p;
}

// ................................................................................  streamStartsWith

bool streamStartsWith(char *p, char *q) 
{
  return strncmp(p, q, strlen(q)) == 0;
}

// ................................................................................  streamEncodeUTF8

// Encode a given character in UTF-8.
int streamEncodeUTF8(char *buf, uint32_t c) 
{
  if (c <= 0x7F) 
  {
    buf[0] = c;
    return 1;
  }

  if (c <= 0x7FF) 
  {
    buf[0] = 192 | (c >> 6);
    buf[1] = 128 | (c & 63);
    return 2;
  }

  if (c <= 0xFFFF) 
  {
    buf[0] = 224 | (c >> 12);
    buf[1] = 128 | ((c >> 6) & 63);
    buf[2] = 128 | (c & 63);
    return 3;
  }

  buf[0] = 240 | (c >> 18);
  buf[1] = 128 | ((c >> 12) & 63);
  buf[2] = 128 | ((c >> 6) & 63);
  buf[3] = 128 | (c & 63);
  
  return 4;
}

// ................................................................................  streamEncodeUTF8

// Read a UTF-8-encoded Unicode code point from a source file.
// We assume that source files are always in UTF-8.
//
// UTF-8 is a variable-width encoding in which one code point is
// encoded in one to four bytes. One byte UTF-8 code points are
// identical to ASCII. Non-ASCII characters are encoded using more
// than one byte.

uint32_t streamDecodeUTF8(streamMBChar_t* pStream,char **new_pos, char *p) 
{
  if ((unsigned char)*p < 128) 
  {
    *new_pos = p + 1;
    return *p;
  }

  char *start = p;
  int len;
  uint32_t c;

  if ((unsigned char)*p >= 240) 
  {
    len = 4;
    c = *p & 7;
  } else if ((unsigned char)*p >= 224) 
  {
    len = 3;
    c = *p & 15;
  } 
  else if ((unsigned char)*p >= 192) 
  {
    len = 2;
    c = *p & 31;
  } 
  else 
  {
   $pushErrLog( loader ,fatal,tokening,invalidUTF8Seq,0,0,pStream->pFileStream->name,start );
  }

  for (int i = 1; i < len; i++) 
  {
    if ((unsigned char)p[i] >> 6 != 2)
    {
      $pushErrLog( loader ,fatal,tokening,invalidUTF8Seq,0,0,pStream->pFileStream->name,start );
    }
    c = (c << 6) | (p[i] & 63);
  }

  *new_pos = p + len;
  return c;
}

// ................................................................................  streamFromHex

int streamFromHex(char c) 
{
  if ('0' <= c && c <= '9')
    return c - '0';

  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;

  return c - 'A' + 10;
}

// ................................................................................  streamReadUniversalChar

uint32_t streamReadUniversalChar(char *p, int len) 
{
  uint32_t c = 0;
  for (int i = 0; i < len; i++) 
  {
    if (!isxdigit(p[i]))
      return 0;
  
    c = (c << 4) | streamFromHex(p[i]);
  }
  return c;
}

// ................................................................................  streamConvertUniversalChars
// Replace \u or \U escape sequences with corresponding UTF-8 bytes.

void streamConvertUniversalChars(char *p) 
{
  char *q = p;

    while (*p) 
    {
        if (streamStartsWith(p, "\\u")) 
        {
            uint32_t c = streamReadUniversalChar(p + 2, 4);
            if (c) 
            {
                p += 6;
                q += streamEncodeUTF8(q, c);
            } 
            else 
            {
                *q++ = *p++;
            }
        } 
        else
        {            
            if (streamStartsWith(p, "\\U")) 
            {
                uint32_t c = streamReadUniversalChar(p + 2, 8);
                if (c) 
                {
                    p += 10;
                    q += streamEncodeUTF8(q, c);
                } 
                else 
                {
                    *q++ = *p++;
                }
            } 
            else 
            {
                if (p[0] == '\\') 
                {
                    *q++ = *p++;
                    *q++ = *p++;
                } 
                else 
                {
                    *q++ = *p++;
                }
            }
        }
    }

  *q = '\0';
}

// ................................................................................  streamGetSource

char* streamGetSource(streamMBChar_t* pStream)
{
    if (pStream == NULL ) return NULL ;
    
    if (pStream->pFileStream == NULL ) return NULL ;
    
    if (pStream->pFileStream->ptr)
    {
        int car;
        while ((car = fgetc(pStream->pFileStream->ptr)) != EOF)
        {
            stringPushBack( (*pStream->pSource),(unsigned char)car)  ;
        }
        // aggiungiamo 3 caratteri per velocizzare i controlli con 
        // il carattere in lettura e quello di lookahed (c0,c1,c2)
        stringPushBack( (*pStream->pSource),(unsigned char)0 )  ;    // c0
        stringPushBack( (*pStream->pSource),(unsigned char)0 )  ;    // c1
        stringPushBack( (*pStream->pSource),(unsigned char)0 )  ;    // c2
    }

    if (pStream->pSource->data==NULL)
    {
        $pushErrLog( loader ,critical,init,initError,0,0,pStream->pFileStream->name,"<buffer>");
        return NULL ;
    }

    pStream->pSource->data = streamNormaliseNewLine(pStream);
    
    pStream->pSource->data = streamRemoveBOM(pStream);

    streamConvertUniversalChars(pStream->pSource->data);
 
    return pStream->pSource->data ; ;
}

// ................................................................................  streamGetNumberOfBytesInCharM

int streamGetNumberOfBytesInCharMB(unsigned char val)
{
    if (val < 128)
    {
        return 1;
    }
    else 
        if (val < 224) 
        {
            return 2;
        } 
        else 
            if (val < 240) 
            {
                return 3;
            } 
            else 
            {
                return 4;
            }
        
    return  0;
}

// ................................................................................   streamConvertMBCtoUTF32

uint32_t streamConvertMBCtoUTF32(char mbs[])
{
    uint32_t c32;

    mbstate_t ss = {0} ;  // initial state
    mbrtoc32(&c32, mbs, MB_CUR_MAX, &ss);

    return c32 ;
}

// ................................................................................   streamPrintfMBChar

char* streamPrintfMBChar(char *mbs)
{
    unsigned char buffer[10];

    buffer[3]=buffer[2]=buffer[1]=buffer[0]=0;
    
    strcpy ( (char*)buffer , mbs ) ;
    
    char temp[BUFFER_STRING_1024];
    
    snprintf( temp,BUFFER_STRING_1024,"%02x%02x%02x%02x"
        ,(unsigned char)buffer[3]
        ,(unsigned char)buffer[2]
        ,(unsigned char)buffer[1]
        ,(unsigned char)buffer[0]
    );
    
    return gcStrDup(temp) ;
}

// ................................................................................   streamConvertSpecialChar

char streamConvertSpecialChar(char c) 
{                       
    switch ( c )
    {
        case '\r'    :    c='R' ; break ;
        case '\n'    :    c='N' ; break ;
        case '\v'    :    c='V' ; break ;
        case '\f'    :    c='F' ; break ;    
        case '\t'    :    c='T' ; break ;  
        case '\\'    :    c='\\'; break ;  
        default:
        break;                    
    }
    
  return c ;        
}

// ................................................................................   streamConvertSpecialCharinMBS

char* streamConvertSpecialCharinMBS(char* mbs) 
{

  for ( uint32_t i=0 ;i<strlen(mbs); i++)
      mbs[i]=streamConvertSpecialChar(mbs[i]);

  return mbs ;  
}

// ................................................................................   streamVectorCharMBDebugFile

void streamVectorCharMBDebugFile(streamMBChar_t* pStream )
{
    if ( pStream->pSource->data ) 
    {
        fprintf ( pStream->pFileDebugCharMB->ptr , "\n[[[\n") ;
        fprintf ( pStream->pFileDebugCharMB->ptr ,  "%s",pStream->pSource->data) ;
        fprintf ( pStream->pFileDebugCharMB->ptr , "\n]]]\n") ;
    }
}

// ................................................................................   streamVectorCharMBDebug
            
int streamVectorCharMBDebug(streamMBChar_t* pStream ) 
{
    if ( pStream->pCompiler->fDebug==true )
    {
    
        vector_charMB_t*    vCharMB =     pStream->pvCharMB; ;
        
        fileStream_t*         fo        =    pStream->pFileDebugCharMB;
        assert(fo!=NULL); 

        if (fo!=NULL )
        {
            fprintf ( fo->ptr , "\nfile :: [[%s]]\n",pStream->pFileDebugCharMB->name ) ;
            fprintf ( fo->ptr , "\nsize :: [[%d]]\n",(unsigned int)vectorSize( (*vCharMB) ) ) ;    

            if ( !vectorSize((*vCharMB)) ) return 0 ;

                itVector(charMB)     itv ;
                int                 counter=0;
                
                for( itv = vectorBegin((*vCharMB)) ; itv != vectorEnd((*vCharMB)); itv++ )
                {
                        fprintf ( fo->ptr , "\n[%3d] "           , counter++                                        ) ;
                        fprintf ( fo->ptr , " T[%3d]"           , (*itv)->type                                    ) ;
                        fprintf ( fo->ptr , " mbs[%s]"           , streamPrintfMBChar ( (char*)(*itv)->mbs )     ) ;
                        fprintf ( fo->ptr , " P[%3d]"            ,(*itv)->pos                                       ) ;
                        fprintf ( fo->ptr , " L[%02d]"           ,(*itv)->lvl                                       ) ;
                        fprintf ( fo->ptr , " RC[%03d / %03d]"  ,(*itv)->row ,(*itv)->col                          ) ;
                        fprintf ( fo->ptr , " U[0x%08x]"           ,(*itv)->c32                                     ) ;

                        char*_mbs = gcStrDup((*itv)->mbs);
                        if ((*itv)->mbs[0]!=0)
                            fprintf ( fo->ptr , " [%s]"           ,streamConvertSpecialCharinMBS(_mbs)              ) ;
                        else
                            fprintf ( fo->ptr , " [%1s]"           ,"§"                                              ) ;
                        gcFree(_mbs);
                }
            
                fprintf ( fo->ptr , "\n" ) ;

            streamVectorCharMBDebugFile( pStream ) ;
    
            gcFileClose(fo->ptr);
        }

    }

    return 0 ;
}

// ................................................................................  streamMakeVectorCharMB

int streamMakeCharType( uint32_t t )
{
    if ( t==0        ) return  charNull  ;    
    if ( isalpha(t) ) return  charAlpha ;
    if ( isdigit(t) ) return  charDigit ;
    if ( isgraph(t) ) return  charGraph ;
    if ( isspace(t) ) return  charBlank ;
    if ( iscntrl(t) ) return  charBlank ;
    if ( ispunct(t) ) return  charPunct ;
    if ( isprint(t) ) return  charPrint ;
    return  charOther ;
}
void streamMakeVectorCharMB(streamMBChar_t* pStream)
{
    assert ( pStream!=NULL ) ;
    assert ( pStream->pSource->data!=NULL ) ;

    uint32_t    row    =1    ;
    uint32_t    col    =1    ;
    uint32_t    pos    =0    ;  

    char buffer[] = { 0,0,0,0,0 } ;

    char* source = pStream->pSource->data ;
    
    while( source!=NULL )
    {
        if ( *source==0 ) break ;

        int len=streamGetNumberOfBytesInCharMB(*source);
    
        for(int i=0;i<len;i++) buffer[i]=*source++;

        buffer[len]=0;

        charMB_t *ch = (charMB_t*) gcMalloc( sizeof(charMB_t) ) ;

        //    make default 
        
        ch->mbs        =    gcStrDup(buffer);
        ch->row        =    row ;
        ch->col        =    col ;
        ch->pos        =    pos ;    
        ch->lvl        =    col / pStream->pCompiler->cTabSize ;    
        ch->c32        =    streamConvertMBCtoUTF32(ch->mbs);
        ch->type       =    streamMakeCharType(ch->c32) ;        
        ch->blk        =    0 ;  
        
        vectorPushBack( (*pStream->pvCharMB) , ch ) ;

             if ( strcmp(buffer,"\t")==0 )  { col+=pStream->pCompiler->cTabSize ;    }  
        else if ( strcmp(buffer,"\r")==0 )  { ++col                             ;    }        
        else if ( strcmp(buffer,"\n")==0 )  { col=1; ++row                      ;    }
        else { ++col; } ;  

        ++pos;
    }

}


