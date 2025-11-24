#ifndef H_MALLOC_H
#define H_MALLOC_H
#include <stdlib.h>
void *malloc_or_exit(size_t nbytes, const char *file, int line);

#define free_vector(v) do {free(v); v = NULL;} while(0)


#define xmalloc(nbytes) malloc_or_exit((nbytes), __FILE__, __LINE__)


#define make_vector(v, n) ((v) = xmalloc((n)* sizeof (v)))


#define make_matrix(a, m, n) do {                               \
    size_t loop_counter;                                        \
    make_vector(a, (m)+1);                                      \
    for (loop_counter = 0; loop_counter <(m); loop_counter++){  \
        make_vector((a)[loop_counter], (n));                    \
    }                                                           \
    (a)[m] = NULL;                                              \
}while(0)



#define free_matrix(a) do{                                      \
    if(a != NULL){                                               \
        size_t loop_counter;                                    \
        for(loop_counter = 0; (a)[loop_counter]!= NULL; loop_counter++){ \
            free_vector((a)[loop_counter]);}\
        free_vector(a); \
        a = NULL;\
    }\
}while(0)



#define print_vector(fmt, v, n) do{\
    size_t print_vectro_loop_counter;\
    for (print_vectro_loop_counter = 0; print_vectro_loop_counter < n; print_vectro_loop_counter++){\
        printf(fmt, v[print_vectro_loop_counter]);\
    }\
    putchar('\n');\
}while(0)




#define print_matrix(fmt, a, m, n) do{\
    printf("matrix\n");\
    size_t print_raw_loop_counter;\
    for (print_raw_loop_counter = 0; print_raw_loop_counter < (m); print_raw_loop_counter++){\
        print_vector((fmt), (a)[print_raw_loop_counter], (n));\
    }\
}while(0)



#endif  //H_MALLOC_H
