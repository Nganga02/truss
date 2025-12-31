/*Declaration of files to be used in the configuration of the truss solver*/


#ifndef H_TRUSS_H
#define H_TRUSS_H
#include "linked-list.h"
#include <stdbool.h>


/**
 * Represent the data for the joints in the truss
 */
struct node{
    int n; //node number
    double X, Y; //reference configuration
    double x, y; //deformed configuration
    bool xfixed, yfixed; //identifying fixed nodes
    double fx, fy; //load vector 
    double rx, ry; //reaction force for support nodes
};


/**
 * Represent the data for the links/members of the truss
 */
struct link{
    int n; // link number
    struct node *np1; //pointer to the first node
    struct node *np2; //pointer to the second node 
    double L; //natural length
    double A; //links cross sectional area
    double E; // young's modulus
    double stress; //link's stress
};


//Declaring a struct to hold information about a truss
struct truss{
    Conscell *nodes_list; //linked list to hold the truss nodes 
    Conscell *links_list; //linked lst to hold the truss links 
    int nnodes; //number of nodes in the truss
    int nlinks; //number of links in the truss
};

int solve_truss(struct truss *truss, double h, double tol, int maxevals);
#endif