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
    ll_free(nodes);
}


/*
Function user to generate a new node
Takes in 5 arguments 
n = the node number
x = x coordinate 
y = y coordinate
xfixed = true if node cannot move horizontal
yfixed = true if node cannot move vertical
*/
static struct node *make_node(int n, double x, double y, int xfixed, int yfixed){
    struct node *new_node = xmalloc(sizeof *new_node);
    new_node->n = n;
    new_node->X = new_node->x = x;
    new_node->Y = new_node->y = y;
    new_node->fx = new_node->fy = 0.0;
    new_node->rx = new_node->ry = 0.0;
}

static struct node *process_node_line(char *str){
    double x, y;
    int n;
    if((sscanf(str, "node   %d  %f  %f", &n, &x, &y)) ==3) return make_node(n, x, y, 0, 0);
    else if((sscanf(str, "node   %d  *%f  %f", &n, &x, &y)) ==3) return make_node(n, x, y, 1, 0);
    else if((sscanf(str, "node   %d  %f  *%f", &n, &x, &y)) ==3) return make_node(n, x, y, 0, 1);
    else if((sscanf(str, "node   %d  *%f  *%f", &n, &x, &y)) ==3) return make_node(n, x, y, 1, 1);
    else return NULL;
}

static Conscell *get_nodes(FILE *stream, int *lineno){
    Conscell *nodes_list = NULL;
    struct node *node;
    char buf[BUFLEN];  
    
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