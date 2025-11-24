#include "xmalloc.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
void *malloc_or_exit(size_t nbytes, const char *file, int line)
{
    void *x;
    if ((x = malloc(nbytes))== NULL)
    {
        fprintf(stderr, "%s: line %d: malloc() of size %zu bytes failed", file , line , nbytes);
        exit(EXIT_FAILURE);
    }
    else
    {
        return x;
    }
}

