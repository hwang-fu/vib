#include "memory.h"

OWNED void * new_(COPIED uint64_t bytes)
{
    OWNED void * ptr = malloc( bytes );
    SCP(ptr);
    return ptr;
}

OWNED void * zeros_(COPIED uint64_t bytes)
{
    OWNED void * ptr = calloc(bytes, sizeof(char));
    SCP(ptr);
    return ptr;
}

OWNED void * realloc_smart(OWNED void * arg, COPIED uint64_t new_bytes)
{
    OWNED void * ptr = realloc(arg, new_bytes);
    SCP(ptr);
    return ptr;
}

COPIED void * dispose(OWNED void * arg)
{
    free_smart(arg);
    return nil;
}
