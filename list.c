#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

#define ARRAY_SIZE 8  /* number of slots in each node's circular array */

/* -------------------------------------------------------------------------
 * NODE - one node in the doubly-linked list.
 * Each node holds up to ARRAY_SIZE void* items in a circular array.
 * 'first' is the index of the front item; items wrap around mod ARRAY_SIZE.
 * ------------------------------------------------------------------------- */
typedef struct node {
    void      **items;      /* circular array of void* pointers              */
    int         count;      /* number of items currently stored in this node */
    int         first;      /* index of the first (front) item               */
    struct node *next;      /* pointer to next node toward the tail          */
    struct node *prev;      /* pointer to prev node toward the head          */
} NODE;

/* -------------------------------------------------------------------------
 * LIST - the top-level list structure.
 * head points to the first node, tail to the last.
 * numItems tracks the total items across all nodes for O(1) count.
 * ------------------------------------------------------------------------- */
struct list {
    NODE *head;      /* first node in the linked list  */
    NODE *tail;      /* last node in the linked list   */
    int   numItems;  /* total items across all nodes   */
};

/* -------------------------------------------------------------------------
 * makeNode - helper: allocate and initialize a new empty node
 * O(1)
 * ------------------------------------------------------------------------- */
static NODE *makeNode(void)
{
    NODE *np = malloc(sizeof(NODE));
    assert(np != NULL);

    np->items = malloc(sizeof(void *) * ARRAY_SIZE); /* allocate circular array */
    assert(np->items != NULL);

    np->count = 0;   /* node starts empty     */
    np->first = 0;   /* front starts at index 0 */
    np->next  = NULL;
    np->prev  = NULL;

    return np;
}

/* -------------------------------------------------------------------------
 * createList - allocate and return a new empty list
 * O(1)
 * ------------------------------------------------------------------------- */
LIST *createList(void)
{
    LIST *lp = malloc(sizeof(LIST));
    assert(lp != NULL);

    lp->head     = NULL;
    lp->tail     = NULL;
    lp->numItems = 0;

    return lp;
}

/* -------------------------------------------------------------------------
 * destroyList - free all nodes and their arrays, then free the list struct
 * O(n) where n is the number of nodes
 * ------------------------------------------------------------------------- */
void destroyList(LIST *lp)
{
    assert(lp != NULL);

    NODE *cur = lp->head;
    while (cur != NULL) {
        NODE *next = cur->next;   /* save next before freeing current */
        free(cur->items);         /* free the circular array          */
        free(cur);                /* free the node itself             */
        cur = next;
    }

    free(lp);   /* free the list struct */
}

/* -------------------------------------------------------------------------
 * numItems - return the total number of items in the list
 * O(1)
 * ------------------------------------------------------------------------- */
int numItems(LIST *lp)
{
    assert(lp != NULL);
    return lp->numItems;
}

/* -------------------------------------------------------------------------
 * addFirst - add item as the first element in the list
 *
 * Steps:
 *   1. If the head node is full (or list is empty), prepend a new node.
 *   2. Decrement 'first' circularly and store the item there.
 * O(1)
 * ------------------------------------------------------------------------- */
void addFirst(LIST *lp, void *item)
{
    assert(lp != NULL);

    /* If list is empty or the head node is full, create a new head node */
    if (lp->head == NULL || lp->head->count == ARRAY_SIZE) {
        NODE *np = makeNode();

        if (lp->head == NULL) {
            /* list was empty: new node is both head and tail */
            lp->head = np;
            lp->tail = np;
        } else {
            /* prepend new node before current head */
            np->next       = lp->head;
            lp->head->prev = np;
            lp->head       = np;
        }
    }

    NODE *front = lp->head;

    /* Move 'first' one step back (circularly) to make room at the front */
    front->first = (front->first - 1 + ARRAY_SIZE) % ARRAY_SIZE;
    front->items[front->first] = item;   /* store item at new front slot */
    front->count++;

    lp->numItems++;
}

/* -------------------------------------------------------------------------
 * addLast - add item as the last element in the list
 *
 * Steps:
 *   1. If the tail node is full (or list is empty), append a new node.
 *   2. Store the item at the next open slot after the existing items.
 * O(1)
 * ------------------------------------------------------------------------- */
void addLast(LIST *lp, void *item)
{
    assert(lp != NULL);

    /* If list is empty or the tail node is full, create a new tail node */
    if (lp->tail == NULL || lp->tail->count == ARRAY_SIZE) {
        NODE *np = makeNode();

        if (lp->tail == NULL) {
            /* list was empty: new node is both head and tail */
            lp->head = np;
            lp->tail = np;
        } else {
            /* append new node after current tail */
            np->prev       = lp->tail;
            lp->tail->next = np;
            lp->tail       = np;
        }
    }

    NODE *back = lp->tail;

    /* The next open slot is (first + count) % ARRAY_SIZE */
    int slot = (back->first + back->count) % ARRAY_SIZE;
    back->items[slot] = item;
    back->count++;

    lp->numItems++;
}

/* -------------------------------------------------------------------------
 * removeFirst - remove and return the first item in the list
 *
 * Steps:
 *   1. If the head node is empty, free it and make the next node the head.
 *   2. Read the item at 'first', advance 'first' circularly, decrement count.
 * O(1)
 * ------------------------------------------------------------------------- */
void *removeFirst(LIST *lp)
{
    assert(lp != NULL);
    assert(lp->numItems > 0);   /* list must not be empty */

    /* If head node is empty, remove it and advance head */
    if (lp->head->count == 0) {
        NODE *old = lp->head;
        lp->head  = old->next;

        if (lp->head != NULL)
            lp->head->prev = NULL;   /* new head has no prev */
        else
            lp->tail = NULL;         /* list became empty    */

        free(old->items);
        free(old);
    }

    NODE *front = lp->head;

    void *item  = front->items[front->first];           /* grab item        */
    front->first = (front->first + 1) % ARRAY_SIZE;    /* advance front    */
    front->count--;

    lp->numItems--;

    /* If the head node is now empty AND there are more nodes, clean it up */
    if (front->count == 0 && front->next != NULL) {
        lp->head       = front->next;
        lp->head->prev = NULL;
        free(front->items);
        free(front);
    }

    return item;
}

/* -------------------------------------------------------------------------
 * removeLast - remove and return the last item in the list
 *
 * Steps:
 *   1. If the tail node is empty, free it and make the prev node the tail.
 *   2. The last item sits at (first + count - 1) % ARRAY_SIZE; read and remove it.
 * O(1)
 * ------------------------------------------------------------------------- */
void *removeLast(LIST *lp)
{
    assert(lp != NULL);
    assert(lp->numItems > 0);   /* list must not be empty */

    /* If tail node is empty, remove it and retreat tail */
    if (lp->tail->count == 0) {
        NODE *old = lp->tail;
        lp->tail  = old->prev;

        if (lp->tail != NULL)
            lp->tail->next = NULL;   /* new tail has no next */
        else
            lp->head = NULL;         /* list became empty    */

        free(old->items);
        free(old);
    }

    NODE *back = lp->tail;

    /* Last item index in circular array */
    int slot  = (back->first + back->count - 1) % ARRAY_SIZE;
    void *item = back->items[slot];
    back->count--;

    lp->numItems--;

    /* If the tail node is now empty AND there are more nodes, clean it up */
    if (back->count == 0 && back->prev != NULL) {
        lp->tail       = back->prev;
        lp->tail->next = NULL;
        free(back->items);
        free(back);
    }

    return item;
}

/* -------------------------------------------------------------------------
 * getItem - return the item at position 'index' in the list
 *
 * To minimize traversal, we start from whichever end is closer:
 *   - index < numItems/2  → search forward from head
 *   - index >= numItems/2 → search backward from tail
 *
 * O(n/m) where n = total items, m = ARRAY_SIZE (we traverse nodes, not items)
 * ------------------------------------------------------------------------- */
void *getItem(LIST *lp, int index)
{
    assert(lp != NULL);
    assert(index >= 0 && index < lp->numItems);

    NODE *cur;

    if (index < lp->numItems / 2) {
        /* ---- Search forward from head ---- */
        cur = lp->head;
        while (index >= cur->count) {
            index -= cur->count;   /* subtract items in this node, move on */
            cur    = cur->next;
        }
    } else {
        /* ---- Search backward from tail ---- */
        /* Convert index to a "distance from the back" */
        int fromBack = lp->numItems - 1 - index;
        cur = lp->tail;
        while (fromBack >= cur->count) {
            fromBack -= cur->count;   /* subtract items in this node, move on */
            cur       = cur->prev;
        }
        /* fromBack is now the distance from the BACK of this node */
        index = cur->count - 1 - fromBack;
    }

    /* Map the local index into the circular array */
    return cur->items[(cur->first + index) % ARRAY_SIZE];
}
