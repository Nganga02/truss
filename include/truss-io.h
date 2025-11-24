#ifndef H_TRUSS_IO_H
#define H_TRUSS_IO_H
#include <stdio.h>
#include "truss.h"
struct truss *read_truss(FILE *stream);
void write_truss(struct truss *truss);
void free_truss(struct truss *truss);
#endif /*H_TRUSS_IO_H*/