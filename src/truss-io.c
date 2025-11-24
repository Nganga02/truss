#include <stdio.h>
#include <string.h>
#include <math.h>
#include "linked-list.h"
#include "fetch_line.h"
#include "xmalloc.h"
#include "truss.h"
#include "truss-io.h"
#define BUFLEN 128
#define SQ(x) ((x)*(x))

// Functions for reading nodes
static void free_nodes_list(Conscell *nodes){

}

static struct node *make_node(int n, double x, double y, int xfixed, int yfixed){

}

static struct node *process_node_line(char *str){

}

static Conscell *get_nodes(FILE *stream, int *lineno){

}


//Function for reading links
static void free_links_list(Conscell *links){

}

static struct link *make_link(int n, int n1, int n2, double A, double E, Conscell *nodes_list){

}

static struct link *process_link_line(char *str, Conscell *nodes_list){

}

static Conscell *get_links(FILE *stream, int *lineno, Conscell *nodes_list){

}

//Functions for reading loads
static int process_load_line(char *str, Conscell nodes_list){

}

static int get_loads(FILE *stream, int *lineno, Conscell *nodes_list){

}

//Functions with external linkage
struct truss *read_truss(FILE *stream){

}

void write_truss(struct truss *truss){

}

void free_truss(struct truss *truss){
    
}