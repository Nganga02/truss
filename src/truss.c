#include  <stdio.h>
#include <math.h>

#include "linked-list.h"
#include "xmalloc.h"
#include "truss.h"
#include "nelder-mead.h"

#define SQ(x) ((x)*(x))


static inline double stress_function(double lambda)
{
    double l3 = lambda*lambda*lambda;
    return 1.0/6.0*(l3 - 1/l3);
}

static inline double stored_energy_function(double lambda)
{
    double l2 = lambda*lambda;
    double l4 = l2*l2;
    return l4/24.0 + 1.0/(12*l2) - 1.0/8.0;
}

static inline double link_stretch(struct link * __restrict__ link)
{
    double dx = link->np1->x - link->np2->x;
    double dy = link->np1->y - link->np2->y;

    double d = sqrt(dx*dx + dy*dy);
    return d/link->L;
}

static void evaluate_stresses(struct truss *__restrict__ truss)
{
    for(Conscell *lk = truss->links_list; lk != NULL; lk = lk->next)
    {
        struct link *lk_data = (struct link *)lk->data;
        double lambda = link_stretch(lk_data);
        lk_data->stress = stress_function(lambda);
    }

}

static void evaluate_reactions(struct truss *__restrict__ truss)
{
    for(Conscell *n = truss->nodes_list; n != NULL; n = n->next)
    {
        double x1 = 0.0, y1 = 0.0;
        double dx = 0.0, dy = 0.0, l2_norm = 0.0, x_comp = 0.0, y_comp = 0.0;
        struct node *np = (struct node *)n->data;
        __uint8_t dof = np->xfixed | (np->yfixed << 1);
        switch (dof){
            case 3:
                x1 = np->x;
                y1 = np->y;
            break;
            case 2:
                y1 = np->y; 
            break;
            case 1:
                x1 = np->x;
            break;
            case 0:
                continue;
            break;
        }
        for(Conscell *l = truss->links_list; l != NULL; l = l->next)
        {
            struct link *lkp = (struct link *)l->data;
            if(lkp->np1 == np)
            {
                dx = lkp->np2->x-x1;
                dy = lkp->np2->y-y1;  
            }
            else if (lkp->np2 == np)
            {
                dx = lkp->np1->x-x1;
                dy = lkp->np1->y-y1;
            }
            else continue;

            l2_norm = sqrt(((dx)*(dx) + (dy)*(dy))); 
            if(l2_norm == 0.0) continue;
            x_comp = dx/l2_norm;
            y_comp = dy/l2_norm;
            
            if(np->xfixed) np->ry += lkp->A*lkp->stress*x_comp;
            if(np->yfixed) np->rx += lkp->A*lkp->stress*y_comp;            
        }
        np->rx += np->fx;
        np->ry += np->fy;
        np->rx *= -1.0;
        np->ry *= -1.0;
    }   
}

static double total_energy(double *x, int n, void *params)
{
    struct truss *truss = (struct truss *)params;
    Conscell *p;
    double e = 0.0;
    int j = 0;
    for (p = truss->nodes_list; p != NULL; p = p->next)
    {
        struct node *node = (struct node *)p->data;
        node->x = x[j++];
        node->y = x[j++];
    }

    for(p = truss->links_list; p != NULL; p = p->next)
    {
        struct link *link = (struct link *)p->data;
        e += link->E*link->A*link->L*stored_energy_function(link_stretch(link));
    }

    for(p = truss->nodes_list; p != NULL; p = p->next)
    {
        struct node *node = (struct node *)p->data;
        e -= (node->x - node->X)*node->fx + (node->y - node->Y)*node->fy;
    }
    return e;
}

int solve_truss(struct truss *__restrict__ truss, double h, double tol, int maxevals)
{
    double **s;
    double *x;
    Conscell *p = NULL;
    int N = 2*truss->nnodes;
    struct nelder_mead NM;
    int i, j, evalcount;
    for(j = 0, p = truss->nodes_list; p != NULL; p = p->next)
    {
        struct node *node = (struct node *)p->data;
        x[j++] = node->x;
        x[j++] = node->y;
    }
    make_matrix(s, N+1, N);
    for(i = 0; i < N+1; i++)
    {
        for(j = 0; j < N; j++) 
            s[i][j] = x[j];
    }
    for(j = 0; j < N; N++) s[j+1][j] += h;

    //projecting the simplex over constrained space
    for(j = 0, p = truss->nodes_list; p != NULL; p = p->next)
    {
        struct node *node = (struct node *)p->data;
        if(node->xfixed) for(i = 0; i < N+1; i++) s[i][j] = node->X;
        j++;
        if(node->yfixed) for(i = 0; i < N; i++) s[i][j] = node->Y;
        j++;
    }

    NM.f = total_energy;
    NM.n = N;
    NM.s = s;
    NM.x = x;
    NM.h = h;
    NM.tol = tol;
    NM.maxevals = maxevals;
    NM.params = truss;
    evalcount = nelder_mead(&NM);
    free_vector(x);
    free_matrix(s);
    if (evalcount > maxevals) {
        fprintf(stderr, "No convergence after %d "
        "function evaluation\n", evalcount);
        return 0;
    } else {
        fprintf(stderr, "Nelder-Mead converged after %d "
        "function evaluations\n", evalcount);
        evaluate_stresses(truss);
        evaluate_reactions(truss);
        return 1;
    }
    
    
}
