#ifndef LIST_H
#define LIST_H

/* Opaque pointer to the list structure */
typedef struct list LIST;

/*
 * createList - allocate and return a new empty list
 * O(1)
 */
LIST *createList(void);

/*
 * destroyList - free all memory associated with the list
 * O(n) where n is the number of nodes
 */
void destroyList(LIST *lp);

/*
 * numItems - return the total number of items in the list
 * O(1)
 */
int numItems(LIST *lp);

/*
 * addFirst - add item as the first element in the list
 * O(1)
 */
void addFirst(LIST *lp, void *item);

/*
 * addLast - add item as the last element in the list
 * O(1)
 */
void addLast(LIST *lp, void *item);

/*
 * removeFirst - remove and return the first element; list must not be empty
 * O(1)
 */
void *removeFirst(LIST *lp);

/*
 * removeLast - remove and return the last element; list must not be empty
 * O(1)
 */
void *removeLast(LIST *lp);

/*
 * getItem - return the item at the given index; index must be in range
 * O(n/m) where n is number of items and m is array size per node
 */
void *getItem(LIST *lp, int index);

#endif /* LIST_H */
