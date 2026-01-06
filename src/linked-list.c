#include <stdio.h>
#include <stdlib.h>

#include "xmalloc.h"
#include "linked-list.h"

struct point
{
    int x;
    int y;
};

// Take note of the type casting
static int cmp2(const void *aa, const void *bb, void *pp)
{
    struct point *a = (struct point *)aa;
    struct point *b = (struct point *)bb;
    struct point *p = (struct point *)pp;
    double dx, dy, d1, d2;

    dx = a->x - p->x;
    dy = a->x - p->y;
    d1 = dx * dx + dy * dy;
    dx = b->x - p->x;
    dy = b->x - p->y;
    d2 = dx * dx + dy * dy;

    if (d1 < d2)
        return -1;
    else if (d2 < d1)
        return 1;
    else
        return 0;
}

// This is applying a stack data structure for our case
Conscell *ll_push(Conscell *newList, void *data)
{
    Conscell *newCell = xmalloc(sizeof *newCell);
    *newCell = (Conscell){0};
    newCell->data = data;
    newCell->next = newList;
    return newCell;
}

Conscell *ll_pop(Conscell *listToPop)
{
    Conscell *popedList = listToPop->next;
    free(listToPop);
    return popedList;
}

void ll_free(Conscell *listTofree)
{
    while (listTofree != NULL)
    {
        Conscell *p = listTofree->next;
        free(listTofree->data);
        free(listTofree);
        listTofree = p;
    }
    /*This is to prevent dangling pointers*/
    listTofree = NULL; 
}

Conscell *ll_reverse(Conscell *listToReverse)
{
    Conscell *reversedList = NULL;
    for (Conscell *p = listToReverse; p != NULL; p = p->next)
    {
        reversedList = ll_push(reversedList, p->data);
    }
    return reversedList;
}

Conscell *ll_sort(Conscell *listToSort, int (*cmp)(const void *a, const void *b, void *params), void *params)
{
    Conscell *list1 = NULL;
    Conscell *list2 = NULL;
    Conscell *p, *q, *head;

    if (listToSort == NULL)
    {
        return listToSort;
    }
    head = listToSort;
    p = listToSort->next;
    //    printf("This is the head data: %d\n", *(int *)head->data);
    while (p != NULL)
    {
        q = p->next;

        if (cmp(p->data, head->data, params) < 0)
        {
            p->next = list1;
            list1 = p;
        }
        else
        {
            p->next = list2;
            list2 = p;
        }
        p = q; // Updating the value of p
    }

    // Doing recursion for both lists
    list1 = ll_sort(list1, cmp, params);
    list2 = ll_sort(list2, cmp, params);
    head->next = list2;

    if (list1 == NULL)
        return head;

    for (p = list1; p->next != NULL; p = p->next)
        ;
    p->next = head;
    return list1;
}

Conscell *ll_filter(Conscell *listToFilter, int (*filter)(const void *a), Conscell **removed)
{
    if (listToFilter == NULL)
        return listToFilter;

    if (!filter(listToFilter->data))
    {
        Conscell *p = listToFilter->next;
        listToFilter->next = *removed;
        *removed = listToFilter; // Making sure that removed always points to the beginning of the first conscell
        return ll_filter(p, filter, removed);
    }
    else
    {
        listToFilter->next = ll_filter(listToFilter->next, filter, removed); // Recursion algorithm
        return listToFilter;
    }
}

Conscell *ll_append(Conscell *list1, Conscell *list2)
{
    Conscell *q = xmalloc(sizeof q);
    for (Conscell *p = list1; p != NULL; p = p->next)
        q = p;
    q->next = list2;
    return list1;
}

void ll_map(Conscell *listToMap, double (*mapping)(const void *a), Conscell **mappedList)
{
    double result;
    for (Conscell *p = listToMap; p != NULL; p = p->next)
    {
        result = mapping(p->data);

        double *storedResult = xmalloc(sizeof(double));
        *storedResult = result;

        Conscell *newConscell = xmalloc(sizeof newConscell);
        newConscell->next = *mappedList;
        newConscell->data = storedResult;
        *mappedList = newConscell;
    }
}

int ll_length(Conscell *longList)
{
    int length = 0;
    for (Conscell *p = longList; p != NULL; p = p->next)
        length++;
    return length;
}
