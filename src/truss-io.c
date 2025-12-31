#include <stdio.h>
#include <string.h>
#include <math.h>

#include "linked-list.h"
#include "fetch_line.h"
#include "xmalloc.h"
#include "truss.h"
#include "truss-io.h"

#define BUFLEN 128
#define SQ(x) ((x) * (x))
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"

// Functions for reading nodes
static void free_nodes_list(Conscell *nodes)
{
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
static struct node *make_node(int n, double x, double y, int xfixed, int yfixed)
{
    struct node *new_node = xmalloc(sizeof *new_node);
    new_node->n = n;
    new_node->X = new_node->x = x;
    new_node->Y = new_node->y = y;
    new_node->fx = new_node->fy = 0.0;
    new_node->rx = new_node->ry = 0.0;
    return new_node;
}

static struct node *process_node_line(char *str)
{
    double x, y;
    int n;
    if ((sscanf(str, "node %d:  %lf  %lf", &n, &x, &y)) == 3)
        return make_node(n, x, y, 0, 0);
    else if ((sscanf(str, "node %d:  *%lf  %lf", &n, &x, &y)) == 3)
        return make_node(n, x, y, 1, 0);
    else if ((sscanf(str, "node %d:  %lf  *%lf", &n, &x, &y)) == 3)
        return make_node(n, x, y, 0, 1);
    else if ((sscanf(str, "node %d:  *%lf  *%lf", &n, &x, &y)) == 3)
        return make_node(n, x, y, 1, 1);
    else
        return NULL;
}

static Conscell *get_nodes(FILE *stream, int *lineno)
{
    Conscell *nodes_list = NULL;
    struct node *node;
    char buf[BUFLEN];
    char *str;
    if ((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        fprintf(stderr, "*** error: no node data in input\n");
        return NULL;
    }
    if (strcmp(str, "BEGIN NODES") != 0)
    {
        fprintf(stderr, "***errror: no 'begin nodes' in input");
        return NULL;
    }
    while ((str = fetch_line(buf, BUFLEN, stream, lineno)) != NULL)
    {
        if (strcmp(str, "END NODES") == 0)
            return nodes_list;

        if ((node = process_node_line(str)) == NULL)
        { 
            fprintf(stderr, "*** error: nodes input is malformed");
            free_nodes_list(nodes_list);
            return NULL;
        }

        nodes_list = ll_push(nodes_list, node);
    }
    fprintf(stderr, "*** error: no 'end nodes' in input");
    free_nodes_list(nodes_list);
    return NULL;
}

// Function for reading links
static void free_links_list(Conscell *links)
{
    for(Conscell *p = links; p != NULL; p = p->next)
    {
        ((struct link *)p->data)->np1 = NULL;
        ((struct link *)p->data)->np2 = NULL;
    }
    ll_free(links);
}

static struct link *make_link(int n, int n1, int n2, double A, double E, Conscell *nodes_list)
{
    struct link *new_link;

    struct node *np1 = NULL, *np2 = NULL;
    Conscell *p;

    for (p = nodes_list; p != NULL; p = p->next)
    {
        struct node *node = (struct node *)p->data;
        if (node->n == n1)
            np1 = node;
        if (node->n == n2)
            np2 = node;
        if (np1 && np2)
            break;
    }
    if (p == NULL && (np1 == NULL || np2 == NULL))
        return NULL;

    new_link = xmalloc(sizeof *new_link);
    new_link->np1 = np1;
    new_link->np2 = np2;
    new_link->A = A;
    new_link->E = E;
    double dx = np2->x - np1->x;
    double dy = np2->y - np1->y;
    new_link->L = sqrt((SQ(dx) + SQ(dy)));
    return new_link;
}

static struct link *process_link_line(char *str, Conscell *nodes_list)
{
    double A, E;
    int n, n1, n2;

    // reading the string format of the string
    if (sscanf(str, "link %d: %d  %d  %lf  %lf", &n, &n1, &n2, &A, &E) == 5)
        return make_link(n, n1, n2, A, E, nodes_list);
    else
        return NULL;
}

static Conscell *get_links(FILE *stream, int *lineno, Conscell *nodes_list)
{
    Conscell *links_list = NULL;
    struct link *link;
    char buf[BUFLEN];
    char *str;
    if ((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        fprintf(stderr, "*** error: no link data in input\n");
        return NULL;
    }
    if (strcmp(str, "BEGIN LINKS") != 0)
    {
        fprintf(stderr, "***errror: no 'BEGIN LINKS' in input");
        return NULL;
    }
    while ((str = fetch_line(buf, BUFLEN, stream, lineno)) != NULL)
    {
        if (strcmp(str, "END LINKS") == 0)
            return links_list;

        if ((link = process_link_line(str, nodes_list)) == NULL)
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

// Functions for reading loads
static int process_load_line(char *str, Conscell *nodes_list)
{
    int n;
    double fx, fy;
    if (sscanf(str, "load %d:  %lf  %lf", &n, &fx, &fy) == 3)
    {
        for (Conscell *p = nodes_list; p != NULL; p = p->next)
        {
            // this is to access the
            if (((struct node *)p->data)->n == n)
            {
                ((struct node *)p->data)->fx = fx;
                ((struct node *)p->data)->fy = fy;
                return 1;
            }
        }
    }
    else return 0;
}

static int get_loads(FILE *stream, int *lineno, Conscell *nodes_list)
{
    char buf[BUFLEN];
    char *str;
    if ((str = fetch_line(buf, BUFLEN, stream, lineno)) == NULL)
    {
        fprintf(stderr, "*** error: no load data in input\n");
        return 0;
    }
    if (strcmp(str, "BEGIN LOADS") != 0)
    {
        fprintf(stderr, "***errror: no 'BEGIN LOADS' in input\n");
        return 0;
    }
    while ((str = fetch_line(buf, BUFLEN, stream, lineno)) != NULL)
    {
        if (strcmp(str, "END LOADS") == 0)
            return 1;

        if (!process_load_line(str, nodes_list))
        {
            fprintf(stderr, ANSI_COLOR_RED"***errror: loads input is malformed\n"ANSI_COLOR_RED);
            return 0;
        }
    }
    fprintf(stderr, ANSI_COLOR_RED "*** error: no 'end nodes' in input" ANSI_COLOR_RED);
    return 0;
}

// Functions with external linkage
struct truss *read_truss(FILE *stream)
{
    struct truss *truss;
    int lineno = 0;
    Conscell *nodes_list, *links_list;
    if ((nodes_list = get_nodes(stream, &lineno)) == NULL)
        return NULL;
    if ((links_list = get_links(stream, &lineno, nodes_list)) == NULL)
    {
        free_nodes_list(nodes_list);
        return NULL;
    }
    if (!get_loads(stream, &lineno, nodes_list))
    {
        free_nodes_list(nodes_list);
        free_links_list(links_list);
        return NULL;
    }

    nodes_list = ll_reverse(nodes_list);
    links_list = ll_reverse(links_list);

    truss = xmalloc(sizeof *truss);
    truss->nodes_list = nodes_list;
    truss->links_list = links_list;
    truss->nnodes = ll_length(nodes_list);
    truss->nlinks = ll_length(links_list);
    fprintf(stderr,
            ANSI_COLOR_GREEN "read %d nodes, %d links\n" ANSI_COLOR_GREEN,
            truss->nnodes, truss->nlinks);
    return truss;
}

/*
This function reads data from a truss struct and writes it to stdout
*/
void write_truss(struct truss *truss)
{
    printf("# Configuration of the deformed truss computed by the\n"
           "# Math 625 truss solver utility\n\n");

    /* ---------------------- NODES ---------------------- */
    printf("# node n:  x   y\nBEGIN NODES\n");
    for (Conscell *p = truss->nodes_list; p != NULL; p = p->next)
    {
        struct node *nd = (struct node *)p->data;

        printf("node %d:   %f  %f\n",
               nd->n, nd->x, nd->y);
    }
    printf("END NODES\n\n");

    /* ---------------------- LINKS ---------------------- */
    printf("# link n:  n1  n2  A  E\nBEGIN LINKS\n");
    for (Conscell *p = truss->links_list; p != NULL; p = p->next)
    {
        struct link *lk = (struct link *)p->data;

        printf("link %d:  %d  %d  %lf  %lf\n",
               lk->n,
               lk->np1->n, lk->np2->n,
               lk->A, lk->E);
    }
    printf("END LINKS\n\n");

    /* ---------------------- LOADS ---------------------- */
    printf("# load n:  fx  fy\nBEGIN LOADS\n");
    for (Conscell *p = truss->nodes_list; p != NULL; p = p->next)
    {
        struct node *nd = (struct node *)p->data;

        if (nd->fx != 0 || nd->fy != 0)
        {
            printf("load %d:  %lf  %lf\n",
                   nd->n, nd->fx, nd->fy);
        }
    }
    printf("END LOADS\n\n");

    /* ---------------------- REACTIONS ---------------------- */
    printf("# node n:  rx  ry\nBEGIN REACTIONS\n");
    for (Conscell *p = truss->nodes_list; p != NULL; p = p->next)
    {
        struct node *nd = (struct node *)p->data;

        if (nd->rx != 0 || nd->ry != 0)
        {
            printf("reaction %d:  %lf  %lf\n",
                   nd->n, nd->rx, nd->ry);
        }
    }
    printf("END REACTIONS\n\n");

    /* ---------------------- STRESSES ---------------------- */
    printf("# link n:  stress\nBEGIN STRESSES\n");
    for (Conscell *p = truss->links_list; p != NULL; p = p->next)
    {
        struct link *lk = (struct link *)p->data;

        printf("stress %d:  %lf\n", lk->n, lk->stress);
    }
    printf("END STRESSES\n");
}


void free_truss(struct truss *truss)
{
    //To avoid segmentation fault
    if(truss != NULL)
    {
        ll_free(truss->nodes_list);
        ll_free(truss->links_list);
        free(truss);
    }
    truss = NULL;
}
/*\(^_^)/*/
