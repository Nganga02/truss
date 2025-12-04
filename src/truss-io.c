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
Function used to generate a new node
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
    char *str;
    if((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        fprintf(stderr, "*** error: no node data in input\n");
        return NULL;
    }
    if(strcmp(str, "BEGIN NODES") != 0)
    {
        fprintf(stderr, "***errror: no 'begin nodes' in input");
        return NULL;
    }
    while((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        if(strcmp(str, "e  END NODES") == 0)
        return nodes_list;

        if((node = process_node_line(str)) == NULL)
        {
            fprintf(stderr, "***errror: nodes input is malformed");
            free_nodes_list(nodes_list);
            return NULL;
        }

        nodes_list = ll_push(nodes_list, node);
    }
    fprintf(stderr, "*** error: no 'end nodes' in input");
    free_nodes_list(nodes_list);
    return NULL;
}


//Function for reading links
static void free_links_list(Conscell *links){
    ll_free(links);
}

static struct link *make_link(int n, int n1, int n2, double A, double E, Conscell *nodes_list){
    struct link *new_link;
    struct node *np1, *np2;
    Conscell *p;


    for (p = nodes_list; p != NULL; p = p->next)
    {
        struct node *node = (struct node *) p->data;
        if(node->n == n1){
            np1 = node;
        }
        if(node->n == n2){
            np2 = node;
        }
        if(np1 != NULL && np2 != NULL) break;
    }
    if(p == NULL && (np1 == NULL || np2 == NULL)) return NULL;
    
    new_link = xmalloc(sizeof *new_link);
    new_link->np1 = np1;
    new_link->np2 = np2;
    new_link->A = A;
    new_link->E = E;
    double dx = np2->x - np1->x;
    double dy = np2->y - np2->y;
    new_link->L = sqrt((SQ(dx) + SQ(dy)));

}

static struct link *process_link_line(char *str, Conscell *nodes_list){
    double A, E;
    int n, n1, n2;

    //reading the string format of the string 
    if(sscanf("link %d: %d  %d  %f  %f", &n, &n1, &n2, &A, &E) == 5) 
        return make_link(n, n1, n2, A, E, nodes_list);
    else return NULL;
}

static Conscell *get_links(FILE *stream, int *lineno, Conscell *nodes_list){
    Conscell *links_list = NULL;
    struct link *link;
    char buf[BUFLEN];  
    char *str;
    if((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        fprintf(stderr, "*** error: no link data in input\n");
        return NULL;
    }
    if(strcmp(str, "BEGIN LINKS") != 0)
    {
        fprintf(stderr, "***errror: no 'BEGIN LINKS' in input");
        return NULL;
    }
    while((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        if(strcmp(str, "END LINKS") == 0)
        return links_list;

        if((link = process_link_line(str, nodes_list)) == NULL)
        {
            fprintf(stderr, "***errror: links input is malformed");
            free_nodes_list(links_list);
            return NULL;
        }

        links_list = ll_push(links_list, link);
    }
    fprintf(stderr, "*** error: no 'end nodes' in input");
    free_links_list(links_list);
    return NULL;
}

//Functions for reading loads
static int process_load_line(char *str, Conscell *nodes_list){
    int n;
    double fx, fy;
    if(sscanf(str, "load   %d: %lf %lf", &n, &fx, &fy) == 3)
    {
        for(Conscell *p = nodes_list; p != NULL; p->next)
        {
            //this is to access the 
            if(((struct node *)p->data)->n == n)
            {
                ((struct node *)p->data)->fx = fx;
                ((struct node *)p->data)->fy = fy;
                return 1;
            }
        }
    }
    return 0;
}

static int get_loads(FILE *stream, int *lineno, Conscell *nodes_list){
    char buf[BUFLEN];  
    char *str;
    if((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        fprintf(stderr, "*** error: no load data in input\n");
        return 0;
    }
    if(strcmp(str, "BEGIN LOADS") != 0)
    {
        fprintf(stderr, "***errror: no 'BEGIN LOADS' in input");
        return 0;
    }
    while((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        if(strcmp(str, "END LOADS") == 0)
        return 1;

        if(process_load_line(str, nodes_list))
        {
            fprintf(stderr, "***errror: loads input is malformed");
            return 0;
        }
    }
    fprintf(stderr, "*** error: no 'end nodes' in input");
    return 0;
}

//Functions with external linkage
struct truss *read_truss(FILE *stream){

}

void write_truss(struct truss *truss){

}

void free_truss(struct truss *truss){
    
}