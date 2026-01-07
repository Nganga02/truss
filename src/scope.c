#include <stdio.h>
#include <stdlib.h>
void make_number(int *p){
*p = 42;
}

int main(void){
int *p = malloc(sizeof *p);
make_number(p);
printf("%d\n", *p);
free(p);
}
