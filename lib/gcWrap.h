#ifndef gcWrap
#define gcWrap

#define malloc 	gcMalloc
#define calloc 	gcCalloc
#define realloc gcRealloc
#define free    gcFree

#endif
