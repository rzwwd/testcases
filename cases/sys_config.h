//
// This file contains all system dependent APIs and set up
// The APIs will be abstracted so that they are the same as
// much as possible across all popular languages
//

// All standard Linux/Unix libc functions

#if defined(__linux__) || defined(__APPLE__)

#define MEMSET          memset
#define MEMSET0         bzero

#ifdef __cplusplus
#define MEM_NEW(x)      new(x)
#define MEM_DELETE      delete
#define PUBLIC          public:
#else  // LANG_C
#define PUBLIC          
#define MEM_NEW(x)      malloc(sizeof(x))
#define MEM_DELETE      free
#define MEM_REALLOC     realloc
#endif // LANG_C

#define MEMCPY          memcpy
#endif // Linux || UNIX

