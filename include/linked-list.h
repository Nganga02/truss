#ifndef H_LINKED_LIST_H
#define H_LINKED_LIST_H

typedef struct conscell
{
    void *data;
    struct conscell *next;
} Conscell;

Conscell *ll_push(Conscell *newList, void *data);
Conscell *ll_pop(Conscell *listToPop);
void ll_free(Conscell *listToFree);
Conscell *ll_reverse(Conscell *listToreverse);
Conscell *ll_sort(Conscell *listSort,int (*cmp) (const void *a, const void *b, void *params), void *params);
Conscell *ll_filter(Conscell *listToFilter, int (*filter)(const void *a),Conscell **removed);
Conscell *ll_append(Conscell *list1, Conscell *list2);
void ll_map(Conscell *listToMap, double (*mapping)(const void *a), Conscell **mappedList);
int ll_length(Conscell *longList);

#endif //∗ H_LINKED_LISTS_H */
