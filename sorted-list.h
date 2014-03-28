#ifndef SORTED_LIST_H
#define SORTED_LIST_H
/*
 * sorted-list.h
 */

/*
 * Sorted list type.  You need to fill in the type as part of your implementation.
 */

struct node_list{
	void *obj;
	int count;
	int deLink;
	struct node_list *next;
};
typedef struct node_list* node;

struct SortedList
{
	node head;
	int (*comp)(void *p1, void *p2);
};
typedef struct SortedList* SortedListPtr;

struct SortedListIterator
{
	node curr;
};
typedef struct SortedListIterator* SortedListIteratorPtr;

typedef int (*CompareFuncT)(void *, void *);

SortedListPtr SLCreate(CompareFuncT cf);

void SLDestroy(SortedListPtr list);

int SLInsert(SortedListPtr list, void *newObj);

int SLRemove(SortedListPtr list, void *newObj);

SortedListIteratorPtr SLCreateIterator(SortedListPtr list);

void SLDestroyIterator(SortedListIteratorPtr iter);

void *SLNextItem(SortedListIteratorPtr iter);

void display(SortedListPtr start);

#endif
